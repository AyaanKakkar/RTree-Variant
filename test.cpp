#include "rtree.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int N = 10000;

int main(int argc, char* argv[]) {
    
    if (argc == 2) {
        N = atoi(argv[1]);
    } 
    
    RTree* rtree = RTree::createIndex("testFile.bin", 1000, 4, 2, 2);
    
    for (int i = 0; i < N; i++) {
        int minX = rand() % 10;
        int maxX = minX + (rand() % 10);
        int minY = rand() % 10;
        int maxY = minY + (rand() % 10);
        Rectangle temp(minX, maxX, minY, maxY);
        uint32_t *dataTemp = new uint32_t[2];
        dataTemp[0] = 1;
        dataTemp[1] = 2;
        rtree->insert(temp, dataTemp, i);
        if (N < 50) {
            rtree->printTree();
        }
        delete dataTemp;
    }

    return 0;
}