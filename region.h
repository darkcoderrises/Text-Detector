//
// Created by harshil on 19/8/16.
//

#ifndef TEXT_RECORG_REGION_H
#define TEXT_RECORG_REGION_H

#include <vector>
#include <set>

using namespace std;

class Region {
public:
    int level_, color_, area_;
    set<int> colors_under_;
    Region(int level_=0, int color_=-1);

    Region* getParent() {
        return parent_;
    }

    void setParent(Region* k) {
        this->parent_ = k;
    }

    Region* checkOverlap();
    int findNumberOfOverlap(vector<int> levels);
    void addRegionUnder(Region* region);
    void addPixel(int x, int y);
    void print();
    void setChildParent();
    double getStability();
    int max_x_=0, min_x_ = 1000000, max_y_=0, min_y_ = 100000;

private:
    Region* parent_ = nullptr;
    vector<Region *> children_;
};

#endif //TEXT_RECORG_REGION_H
