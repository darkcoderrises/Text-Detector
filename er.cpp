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

const int maxN = 51234567;
int directions_[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
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
    if (color_level == -1) return -1;
    if (color_level_[color_level] == color_level) return color_level;
    color_level_[color_level] = get_color(color_level_[color_level]);
    return color_level_[color_level];
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
        	if (color_[j*width_+i] == -1) {
            	cout << "*" << " ";
        	} else {
        		cout << get_color(color_[j*width_+i]) << " ";
        	}
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
                if ((j==0) || (j>0 && pixels[i] >= levels_[j-1]))
                    sorted_pixels[j].push_back(i);

    color_ = new int[width_ * height_];
    for (int i=0; i<width; i++)
        for (int j=0; j<height; j++)
            color_[get_pixel(i,j)] = -1;
    //cout <<"----INIT----\n";
    //printMatrix();

    current_color_ = -1;
    for (working_on_level_=0; working_on_level_<levels_.size(); working_on_level_++) {
        int level_color = current_color_;
        for (int i=0; i<sorted_pixels[working_on_level_].size(); i++) {
            int pixel = sorted_pixels[working_on_level_][i];
            int x=get_x(pixel), y=get_y(pixel);
            int max_color = -1;

            for (int dir=0; dir<8; dir++) {
                int X = x + directions_[dir][0], Y = y + directions_[dir][1];
                if (X<0 || X>=width_ || Y<0 || Y>=height_) continue;
                max_color = max(get_color(color_[get_pixel(X,Y)]), max_color);
            }

            if (max_color <= level_color  || max_color == -1) {
                current_color_++;
                Region *region = new Region(levels_[working_on_level_], current_color_);
                region->addPixel(x,y);
                color_[get_pixel(x,y)] = current_color_;
                color_level_[current_color_] = current_color_;

                for (int dir=0; dir<8; dir++) {
                    int X = x + directions_[dir][0], Y = y + directions_[dir][1];
                    if (X<0 || X>=width_ || Y<0 || Y>=height_) continue;

                    if (color_[get_pixel(X,Y)] == -1) continue;
                    if (current_color_ != get_color(color_[get_pixel(X,Y)])) {
                        region->addRegionUnder(er_result[get_color(color_[get_pixel(X, Y)])]);
                        color_level_[get_color(color_[get_pixel(X, Y)])] = current_color_;
                    }
                }

                er_result.push_back(region);
                continue;
            }

            er_result[max_color]->addPixel(x,y);
            color_[get_pixel(x,y)] = max_color;
            for (int dir =0; dir<8; dir++) {
                int X = x + directions_[dir][0], Y = y + directions_[dir][1];
                if (X<0 || X>=width_ || Y<0 || Y>=height_) continue;

                if (get_color(color_[get_pixel(X,Y)]) > level_color) {
                    er_result[max_color]->merge(er_result[get_color(color_[get_pixel(X,Y)])]);
                    color_level_[get_color(color_[get_pixel(X,Y)])] = max_color;
                }
                else if (get_color(color_[get_pixel(X,Y)]) != -1) {
                    er_result[max_color]->addRegionUnder(er_result[get_color(color_[get_pixel(X,Y)])]);
                }
            }
        }

        // cout << "======================================\n";
        // printMatrix();
        // cout << "======================================\n";
    }

    return er_result;
}

Region* checkOverlap(Region* er, bool* visited) {
    int number_of_overlap = 0;
    visited[er->color_] = true;
    Region* parent = er->parent_;
    Region* max_element = er;
    
    while (parent && (er->area_ > 0.3* parent->area_)) {
    	visited[parent->color_] = true;
        number_of_overlap ++;
        if (max_element->getStability() < parent->getStability()) {
            max_element = parent;
        }
        parent = parent->parent_;
        if (parent) visited[parent->color_] = true;
    }

    if (number_of_overlap>=3) return max_element;
    return nullptr;
}

vector<Region *> ER::non_maximum_suppression(vector<Region *> er) {
    bool *visited = new bool[er.size()];
    vector < Region * > result;

    for (int i = 0; i < er.size(); i++) {
    	if (visited[er[i]->color_]) 
    		continue;
        Region *now = er[i];
        Region *max_ov = checkOverlap(now, visited);

        if (max_ov) {
            result.push_back(max_ov);
            visited[max_ov->color_] = true;
        }
    }

    return this->clean_er_tree(result);
}

vector<Region *> ER::clean_er_tree(vector<Region *> er) {
    vector<Region *> result;
    for (int i=0; i<er.size(); i++) {
        Region *now = er[i];
        if (now->getAspectRatio() > 5) continue;
        if (now->area_ < 0.0005*width_*height_ || now->area_ > 0.1*width_*height_) continue;
        result.push_back(now);
    }

    return result;
}
