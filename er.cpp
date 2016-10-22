//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

ER::ER(vector<int> levels, double minArea_, double maxArea_) {
    this->levels_ = levels;
    this->minArea_ = minArea_;
    this->maxArea_ = maxArea_;
}

const int maxN = 1123456;
int directions_[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
int width_, height_;
int *color_, working_on_level_; //For dfs
int color_level_[maxN], current_color_; // For counting
bool *visited;
vector<int> pixels_;
vector<Region *> er_result;

int get_x(int pixel) {
    if (width_>0) return pixel%width_;
    return -1;
}

int get_y(int pixel) {
    if (width_>0) return pixel/width_;
    return -1;
}

int get_pixel(int x, int y) {
    return x+y*width_;
}

bool ER::check_level(int x, int y) {
    int pixel = get_pixel(x,y);
    return pixels_[pixel] <= this->levels_[working_on_level_];
}

int get_color(int color_level) {
    if (color_level_[color_level] == color_level) return color_level;
    return (color_level_[color_level] = get_color(color_level_[color_level])) & color_level_[color_level];
}

void ER::dfs(int pixel, Region* region) {
    int x = get_x(pixel), y=get_y(pixel);
    region->addPixel(x, y);
    visited[pixel] = true;

    for (int i=0; i<4; i++) {
        int x1 = x+directions_[i][0], y1 = y+directions_[i][1];
        int pixel1 = get_pixel(x1, y1);

        if (!(x1 < width_ && x1 >=0 && y1 < height_ && y1 >= 0 && this->check_level(x1, y1) && !visited[pixel1])) continue;

        if (color_[pixel1] == -1) {
            color_[pixel1] = get_color(color_[pixel]);
            dfs(pixel1, region);
        }
        else if (get_color(color_[pixel1]) != get_color(color_[pixel])){
            region->addRegionUnder(er_result[get_color(color_[pixel1])]);
            color_level_[color_[pixel1]] = color_level_[color_[pixel]];
            dfs(pixel1, region);
        }

    }
}

void print_color() {
    cout << "----COLOR----\n";
    for (int i=0; i<=   current_color_; i++) {
        cout << "(" << i << "," << color_[i] << "," << get_color(color_[i]) << ") ";
    }
    cout << endl;
}

void print_vector(vector<int> a) {
    for (int i=0; i<a.size(); i++)
        cout << a[i] << " ";
    cout << endl;
}

void printMatrix() {
    for (int i=0; i<width_; i++) {
        for (int j=0; j<height_; j++) {
            cout << color_[j*width_ + i] << " ";
        }
        cout << endl;
    }
}

vector<Region*> ER::find(vector<int> pixels, int width, int height) {
    // Initialising some variables;
    width_ = width;
    height_ = height;
    pixels_ = pixels;
    er_result.clear();

    // COUNT SORTING
    // sorted_pixel[i] would contain the pixels with intensity less than level[i]
    vector<int> sorted_pixels[levels_.size()];
    for(int i=0; i<pixels.size(); i++)
        for (int j=0; j<levels_.size(); j++)
            if (pixels[i] <= levels_[j])
                //if ((j==0) || (j>0 && pixels[i] >= levels_[j-1]))
                    sorted_pixels[j].push_back(i);

    color_ = new int[width_ * height_];
    for (int i=0; i<width; i++)
        for (int j=0; j<height; j++)
            color_[get_pixel(i,j)] = -1;
    //cout <<"----INIT----\n";
    //printMatrix();

    current_color_ = -1;
    for (working_on_level_=0; working_on_level_<levels_.size(); working_on_level_++) {
        visited = new bool[width_ * height_];
        for (int i = 0; i < sorted_pixels[working_on_level_].size(); i++) {
            int pixel = sorted_pixels[working_on_level_][i];
            if (visited[pixel] == 0) {
                //cout << "Current color " << current_color_ <<endl;
                current_color_ += 1;
                color_level_[current_color_] = current_color_;
                color_[pixel] = current_color_;
                Region *region = new Region(levels_[working_on_level_], current_color_);
                //region->addColorUnder(current_color_);

                dfs(pixel, region);
                region->setChildParent();
                er_result.push_back(region);

                //print_color();
            }
        }

        cout << "---- LEVEL -----\n";
        //printMatrix();
    }

    //print_color();

    return er_result;
}

vector<Region *> ER::non_maximum_suppression(vector<Region *> er) {
    bool *visited = new bool[er.size()];
    vector<Region *> result;

    for (int i=0; i<er.size(); i++) {
        Region* now = er[i];
        Region* max_ov = now->checkOverlap();

        if (max_ov) {
            if (!visited[max_ov->color_]) {
                result.push_back(max_ov);
                visited[max_ov->color_] = true;
            }
        }
    }

    return result;
}
