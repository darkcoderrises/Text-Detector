#ifndef TEXT_H
#define TEXT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class TextRegion {
public:
    Mat im;
    Point p1, p2;
    int min_x, max_x, min_y, max_y;

    TextRegion(Mat im, Point p1, Point p2);
    bool compare(vector<TextRegion> strongRegions);
};

#endif
