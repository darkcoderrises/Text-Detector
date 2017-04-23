#include "text.h"
#include <iostream>

using namespace cv;
using namespace std;

TextRegion::TextRegion(Mat im, Point p2, Point p1) {
    cvtColor(im, this->im, COLOR_RGB2YCrCb);
    
    this->p1 = p1;
    this->p2 = p2;
    this->min_x = min(p1.x, p2.x);
    this->max_x = max(p2.x, p1.x);
    this->min_y = min(p1.y, p2.y);
    this->max_y = max(p2.y, p1.y); 
}

bool checkLocation(TextRegion regionWeak, TextRegion regionStrong) {
    int x1 = (regionWeak.min_x + regionWeak.max_x)/2;
    int y1 = (regionWeak.min_y + regionWeak.max_y)/2;

    int x2 = (regionStrong.min_x + regionStrong.max_x)/2;
    int y2 = (regionStrong.min_y + regionStrong.max_y)/2;

    int width = regionStrong.max_x - regionStrong.min_x;
    int height = regionStrong.max_y - regionStrong.min_y;
    int distance = min(width, height);

    return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) <= 4*distance*distance;
}

bool checkSize(TextRegion regionWeak, TextRegion regionStrong) {
    int width1 = regionWeak.max_x - regionWeak.min_x;
    int width2 = regionStrong.max_x - regionStrong.min_x;
    int diff1 = max(width1 - width2, width2 - width1);
    
    int height1 = regionWeak.max_y - regionWeak.min_y;
    int height2 = regionStrong.max_y - regionStrong.min_y;
    int diff2 = max(height2 - height1, height1 - height2);

    int threshhold = 2*min(min(width1, width2), min(height1, height2));
    return threshhold >= diff1 && threshhold >= diff2;
}

bool checkColor(TextRegion region1, TextRegion region2, int i, int j){
    Vec3b color1 = region1.im.at<Vec3b>(i,j);
    Vec3b color2 = region2.im.at<Vec3b>(i,j);

    for (int k=0; k<3; k++) {
        int color1k = (int) color1.val[k];  
        int color2k = (int) color2.val[k];
        int diff = color1k - color2k;

        if (diff > 200 || diff < -200) return false;
    }
    return true;
}

bool checkColor(TextRegion regionWeak, TextRegion regionStrong) {
    for (int i=0; i<24; i++) {
        for (int j=0; j<24; j++) {
            bool val = checkColor(regionWeak, regionStrong, i, j);
            if (!val) return false;
        }
    }

    return true;
}

bool TextRegion::compare(vector<TextRegion> regions) {
    TextRegion weak = *this;
    for (int i=0; i<regions.size(); i++) {
        TextRegion strong = regions[i];
        bool val = checkLocation(weak, strong) & checkColor(weak, strong)
            & checkSize(weak, strong);

        if (val) return true;
    }

    return false;
}
