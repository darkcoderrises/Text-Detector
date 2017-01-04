//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int direction[][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}}, **gt_arr;
ofstream result_x("Result/result_x", ofstream::out|ofstream::app), result_y("Result/result_y", ofstream::out|ofstream::app), 
boundary("Result/boundary", ofstream::out|ofstream::app);

double* calcHistogram(Point p1, Point p2, Mat image) {
    double* histogram = new double[1<<8];

    for (int i=0; i<(1<<8); i++) 
    	histogram[i] = 0;

    int number_of_items = 0;
    for (int i=(p1.x+1); i<=(p2.x-1); i++) {
        for (int j=(p1.y+1); j<=(p2.y-1); j++) {
        	number_of_items += 1;
            double sum=0;
            for (int direc=0; direc<8; direc++) {
                int x = i+direction[direc][1], y = j+direction[direc][0];
                sum += (double) image.at<uchar>(y,x);
            }
            sum /= 8;
            unsigned char code = 0;
            for (int direc=0; direc<8; direc++) {
                int x = i+direction[direc][1], y = j+direction[direc][0];
                code |= (((double) image.at<uchar>(y,x)) < sum) << (7-direc);
            }

            histogram[(int) code] += 1;
        }
    }

    for (int i=0; i<(1<<8); i++)
    	histogram[i] = 100 * ((double) histogram[i]) / ((double) number_of_items);

    return histogram;
}

Mat gt_image_mat;
void readGroundTruth(String image_path) {
    gt_image_mat = imread(image_path, CV_LOAD_IMAGE_GRAYSCALE);

    gt_arr = new int*[gt_image_mat.rows];
    for (int i=0; i<gt_image_mat.rows; i++) {
    	gt_arr[i] = new int[gt_image_mat.cols];
    }

    for (int i=0; i<gt_image_mat.rows; i++) {
    	uchar *ptr = gt_image_mat.ptr<uchar>(i);
    	for (int j=0; j<gt_image_mat.cols; j++) {
			gt_arr[i][j] = ((int) ptr[j]) < 255;
    		if (i==0 && j == 0) continue;
    		else if (i==0) gt_arr[i][j] += gt_arr[i][j-1];
    		else if (j==0) gt_arr[i][j] += gt_arr[i-1][j];
    		else gt_arr[i][j] += gt_arr[i][j-1] + gt_arr[i-1][j] - gt_arr[i-1][j-1];
    	} 
    }
}	

bool checkGroundTruth(Point p1, Point p2) {
    int area = (p1.y-p2.y)*(p1.x-p2.x);
    int points = gt_arr[p2.y][p2.x] -gt_arr[p1.y][p2.x] - gt_arr[p2.y][p1.x] + gt_arr[p1.y][p1.x];

    return points > 0.3*area;
}

void runOnImage(String name) {
    String image_name = "database/" + name + ".jpg";
    String gt_image = "database/gt_" + name + ".png";

    cerr << image_name << " " << gt_image << endl;
    vector<int> levels;
    for (int i=1; i<256; i++) levels.push_back(i);

    ER er(levels), er_inv(levels);

    Mat image = imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);
    Mat im1 = imread(image_name, CV_LOAD_IMAGE_COLOR);

    im1.convertTo(im1, CV_8U);
    image.convertTo(image, CV_8U);
    vector<int> points[2];
    readGroundTruth(gt_image);

    cout << image.rows << " * " << image.cols << endl;
    for (int i=0;i<image.rows;i++) {
        uchar* ptr = image.ptr<uchar>(i);
        for (int j=0; j<image.cols;j++) {
            points[0].push_back((int)(ptr[j]));
            points[1].push_back(255-(int)(ptr[j]));
        }
    }

    vector<Region *> result_i = er_inv.find(points[1], image.cols, image.rows);
    vector<Region *> result = er.find(points[0], image.cols, image.rows);
    vector<Region *> suppressed_inv = er.non_maximum_suppression(result_i);
    vector<Region *> suppressed = er.non_maximum_suppression(result);
    for (int i=0; i<suppressed_inv.size();i++){
         suppressed.push_back(suppressed_inv[i]);
    }

    cout << " Printing suppressed\n";
    for (int i=0;i<suppressed.size();i++) {
        Point p1,p2;
        p1.x = suppressed[i]->min_x_;
        p1.y = suppressed[i]->min_y_;
        p2.x = suppressed[i]->max_x_;
        p2.y = suppressed[i]->max_y_;

        if (!((p2.x-p1.x)>=3 && (p2.y-p1.y)>=3)) continue;

        double* hist = calcHistogram(p1, p2, image);

        boundary << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;

        for (int t=0; t<(1<<8); t++) {
            result_x << hist[t] << " ";
        }
        result_x << endl;


        if (checkGroundTruth(p1, p2)) { 
            rectangle(im1, p1, p2, CV_RGB(0,255,0),1);
            result_y << 1 << endl;
        }
        else {
            rectangle(im1, p1, p2, CV_RGB(255,0,0),1);
            result_y << -1 << endl;
        }
	}
//     imshow(gt_image, gt_image_mat);
// 	imshow("im1", im1);
//     waitKey(0);
//     destroyWindow("im1");
}

int main(int argc, char *argv[]) {
    runOnImage(argv[1]);
    result_y.close();
    result_x.close();
    boundary.close();
    return 0;
}
