//
// Created by harshil on 19/8/16.
//

#ifndef TEXT_RECORG_ER_H
#define TEXT_RECORG_ER_H

#include "region.h"
#include <vector>
#include <set>

using namespace std;

class ER {
public:
    vector<int> levels_;
    vector<Region *> find(vector<int> pixels, int width, int height);
    ER(vector<int> levels, double minArea_=0.001, double maxArea_=0.5);
    bool check_level(int x, int y);
    void dfs(int pixel, Region* region);
    vector<Region *> non_maximum_suppression(vector<Region*> er);
    vector<Region *> clean_er_tree(vector<Region*> er);

private:
    double minArea_, maxArea_;
};


#endif //TEXT_RECORG_ER_H