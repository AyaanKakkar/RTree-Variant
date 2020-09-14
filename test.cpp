#include "rtree.h"
#include <iostream>

using namespace std;

int main() {
    RTree* rtree = RTree::createIndex("testFile.bin", 1000, 3, 1, 2);
    
    Rectangle mbr[20];
    
    for (int i = 0; i < 20; i++) {
        Rectangle temp(1 - i, 1 + i, 1 - i, 1 + i);
        mbr[i] = temp;
        uint32_t *dataTemp = new uint32_t[2];
        dataTemp[0] = 1;
        dataTemp[1] = 2;
        rtree->insert(mbr[i], dataTemp, i);
    }

    return 0;
}