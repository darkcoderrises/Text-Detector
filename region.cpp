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

void Region::setChildParent() {
    for (int i=0; i<children_.size();i++) {
        children_[i]->setParent(this);
    }
}

bool Region::checkOverlap(int parent_level) {
    Region* parent = this;
    while (parent_level --) {
        parent = parent->getParent();
        if (parent == NULL)
            return false;
    }
    return parent->area_ >= 0.7 * this->area_;
}

int Region::findNumberOfOverlap(vector<int> levels) {
    return 0;
}

void Region::addRegionUnder(Region *region) {
    this->colors_under_.insert(region->color_);
    this->children_.push_back(region);
}

void Region::addPixel(int x, int y) {
    this->min_x_ = min(this->min_x_, x);
    this->max_x_ = max(this->max_x_, x);
    this->max_y_ = max(this->max_y_, y);
    this->min_y_ = min(this->min_y_, y);

    this->area_ = (this->max_y_ - this->min_y_ + 1)*(this->max_x_ - this->min_x_ +1 );
}

void Region::print() {
    int k=0;
    if (parent_ != NULL) k = parent_->color_;

    cout << "\n------------------\nLEVEL : " << level_ << " : " << color_ << " : " << area_ << " : " << k << endl << "Colors under : ";
    for (set<int>::iterator it=colors_under_.begin(); it!=colors_under_.end(); ++it)
        std::cout << *it << " ";

    cout << "\n------------------\n";
}
