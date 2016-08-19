//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>

using namespace std;

ER::ER(vector<int> levels, double minArea_, double maxArea_) {
    this->levels_ = levels;
    this->minArea_ = minArea_;
    this->maxArea_ = maxArea_;
}

Region* ER::find(vector<int> pixels, int width, int height) {
    return new Region(0);
}