//
// Created by harshil on 19/8/16.
//

#include "region.h"
#include <iostream>

using namespace std;

Region::Region(int level_, int color_) {
    this->level_ = level_;
    this->color_ = color_;
}

bool Region::checkOverlap(int parent_level) {
    return true;
}

int Region::findNumberOfOverlap(vector<int> levels) {
    return 0;
}

void Region::addColorUnder(int color) {
    colors_under_.insert(color);
}

void Region::print() {
    cout << "\n------------------\nLEVEL : " << level_ << " : " << color_ << endl << "Colors under : ";
    for (set<int>::iterator it=colors_under_.begin(); it!=colors_under_.end(); ++it)
        std::cout << *it << " ";

    cout << "\n------------------\n";
}