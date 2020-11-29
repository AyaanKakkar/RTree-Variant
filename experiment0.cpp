#include "rtree.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <generate.h>

using namespace std;

const string indexFileName = "exp0.bin";

const int32_t pageSize = 4096;
const int32_t maxEntries = 100;
const int32_t minEntries = 2;
const int32_t bitmapSize = 2;
const double skewness_zipf = 0.7;
const int sizes[] = {10, 100, 1000, 10000};
const int n_sizes = 4;


int32_t poi_count;
int32_t* poiID = nullptr;

Rectangle* MBRs = nullptr;

int32_t* eventCounts = nullptr;
Event** events = nullptr;

int32_t* docID = nullptr;

int32_t **bitmaps = nullptr;

RTree* rtree = nullptr;

bool flag = true;


void getData(int query) {
    
    if (flag) {
        flag = false;
    }
    else {
        delete[] poiID;
        delete[] MBRs;
        delete[] docID;
        delete rtree;
        for (int i = 0; i < poi_count; i++) {
            delete[] events[i];
            delete[] bitmaps[i];
        }
        delete[] eventCounts;
        delete[] events;
        delete[] bitmaps;
    }

    if (query == -1) {
        // Real Data
    }
    else {
        poi_count = generateData(query, skewness_zipf, bitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
    }
}



// Returns number of disk IOs
int buildRTree() {
    
    rtree = RTree::CreateIndex(indexFileName, pageSize, maxEntries, minEntries, bitmapSize);

    for (int32_t i = 0; i < poi_count; i++) {
        rtree -> insert(MBRs[i], bitmaps[i], poiID[i], docID[i]);
    }
    return rtree -> diskIO_;
}


int main(int argc, char* argv[]) {

    
    long double timeTaken[n_sizes];
    int diskIO[n_sizes];


    for (int i = 0; i < n_sizes; i++) {

        remove(indexFileName.c_str());
        getData(sizes[i]);

        cout << "Query Size : " << sizes[i] << endl;

        auto start = chrono::high_resolution_clock::now();

        diskIO[i] = buildRTree();

        auto end = chrono::high_resolution_clock::now();
        
        timeTaken[i] = chrono::duration_cast<chrono::nanoseconds>(end - start).count(); 

        timeTaken[i] *= 1e-6; // Cast to milliseconds

        cout << "DISK IOs : " << diskIO[i] << endl;
        cout << "Build Time : " << fixed << timeTaken[i] << setprecision(9) << endl;

        cout << endl;

    }

    return 0;
}