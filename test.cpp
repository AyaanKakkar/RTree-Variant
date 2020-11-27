#include "rtree.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int N = 10000;
int pageSize = 1000;
int maxEntries = 4;
int minEntries = 2;
int bitmapSize = 2;

int main(int argc, char* argv[]) {

    if (argc == 2) {
        N = atoi(argv[1]);
    } 
    
    RTree* rtree = RTree::CreateIndex("testFile.bin", pageSize, maxEntries, minEntries, bitmapSize);
    
    for (int i = 0; i < N; i++) {
        int minX = rand() % 10;
        int maxX = minX + (rand() % 10);
        int minY = rand() % 10;
        int maxY = minY + (rand() % 10);
        
        Rectangle temp(minX, maxX, minY, maxY);
        
        int32_t *dataTemp = new int32_t[bitmapSize];
        
        for (int j = 0; j < bitmapSize; j++) {
            dataTemp[j] = j+1;
        }
        
        rtree->insert(temp, dataTemp, i, i);
        
        if (N < 50) {
            rtree->printTree();
            cout << "Disk IO : " << rtree->diskIO_ << endl;
        }

        delete dataTemp;
    }

    cout << "Overall Disk IO : " << rtree->diskIO_ << endl;

    return 0;
}