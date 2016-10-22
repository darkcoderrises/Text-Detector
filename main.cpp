//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

int main() {
    int arr[6] = {40, 80, 120, 160, 200, 255};
    vector<int> levels;
    for (int i=0; i<6; i++) levels.push_back(arr[i]);

    ER er(levels);
    srand (time(NULL));
    vector<int> points;

    int size =1000;
    for (int i=0;i<size*size;i++) {
        points.push_back(rand()%255+1);
    }

    /*for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            cerr << points[i + j * size] << " ";
        }
        cerr << endl;
    }*/

    vector<Region *> result = er.find(points, size, size);
    for(int i=0;i<result.size();i++) {
        //result[i]->print();
    }
    vector<Region *> suppressed = er.non_maximum_suppression(result);
    cout << " Printing suppressed\n";
    for (int i=0;i<suppressed.size();i++) {
        //cout << suppressed[i]->color_ <<" ";
    }
    cout << endl;
    return 0;
}
