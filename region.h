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
    int level_, color_;
    set<int> colors_under_;
    Region(int level_=0, int color_=-1);

    bool checkOverlap(int parent_level);
    int findNumberOfOverlap(vector<int> levels);
    void addColorUnder(int color);
    void print();

private:
    Region* parent_;
    vector<Region *> children_;
};

#endif //TEXT_RECORG_REGION_H