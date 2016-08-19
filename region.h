//
// Created by harshil on 19/8/16.
//

#ifndef TEXT_RECORG_REGION_H
#define TEXT_RECORG_REGION_H

#include <vector>

using namespace std;

class Region {
public:
    int level_;
    vector<int> points_;
    Region(int level_=0);

    bool checkOverlap(int parent_level);
    int findNumberOfOverlap(vector<int> levels);

private:
    Region* parent_;
    vector<Region *> children_;
};

#endif //TEXT_RECORG_REGION_H