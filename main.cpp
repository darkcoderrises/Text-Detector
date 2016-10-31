//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main() {
    //int arr[6] = {40, 80, 120, 160, 200, 255};
    vector<int> levels;
    for (int i=1; i<256; i++) levels.push_back(i);

    ER er(levels);
    Mat image = imread("a.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat im1 = imread("a.jpg", CV_LOAD_IMAGE_COLOR);
    im1.convertTo(im1, CV_8U);
    image.convertTo(image, CV_8U);
    vector<int> points;

    for (int i=0;i<image.rows;i++) {
        uchar* ptr = image.ptr<uchar>(i);
        for (int j=0; j<image.cols;j++) {
            points.push_back((int)(ptr[j]));
        }
    }

    vector<Region *> result = er.find(points, image.cols, image.rows);
    for(int i=0;i<result.size();i++) {
        //result[i]->print();
    }
    vector<Region *> suppressed = er.non_maximum_suppression(result);
    cout << " Printing suppressed\n";
    for (int i=0;i<suppressed.size();i++) {
        Point p1,p2;
        p1.x = suppressed[i]->min_x_;
        p1.y = suppressed[i]->min_y_;
        p2.x = suppressed[i]->max_x_;
        p2.y = suppressed[i]->max_y_;

        rectangle(im1, p1, p2, CV_RGB(0,255,0),1);
        //cout << p1.x << " " << p1.y << " " << p2.x-p1.x << " " << p2.y-p1.y << endl;
    }
    cout << suppressed.size() << endl;
    imshow("Im1", im1);
    waitKey(0);
    return 0;
}
