//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>

using namespace std;

int main() {
    int arr[] = {40, 80, 120, 160, 200};
    vector<int> levels;
    for (int i=0; i<5; i++) levels.push_back(arr[i]);

    ER er(levels);
    return 0;
}