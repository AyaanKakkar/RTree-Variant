#include "rtree.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <generate.h>
#include <vector>

using namespace std;

const string indexFileName = "exp1.bin";

const int32_t pageSize = 4096;
const int32_t maxEntries = 100;
const int32_t minEntries = 2;
const int32_t bitmapSize = 2;
const int n_pois = 5000;
const int zipf_skewness = 0.7;
const double query_sizes[] = {0.5, 1, 1.5, 2, 2.5, 3};
const int n_sizes = 6;

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

    if (query == 0) {
        // Real Data
    }
    else {
        poi_count = generateData(n_pois, zipf_skewness, bitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
    }
}

// Builds RTree
void buildRTree() {
    
    rtree = RTree::CreateIndex(indexFileName, pageSize, maxEntries, minEntries, bitmapSize);

    for (int32_t i = 0; i < poi_count; i++) {
        rtree -> insert(MBRs[i], bitmaps[i], poiID[i], docID[i]);
    }

}


int main(int argc, char* argv[]) {

    
    long double timeTaken[n_sizes];
    int diskIO[n_sizes];


    for (int i = 0; i < 2; i++) { // 0 for real data // 1 for synthetic

        if (i == 0) continue;

        remove(indexFileName.c_str());
        getData(i);

        buildRTree();
        
        // Get query rectangle mid point
        long double centerX = 0;
        long double centerY = 0;

        for (int j = 0; j < poi_count; j++) {
            centerX += MBRs[j].getCenterX();
            centerY += MBRs[j].getCenterY();
        }

        centerX /= poi_count;
        centerY /= poi_count;


        for (int j = 0; j < n_sizes; j++) {

            double radius = query_sizes[j];
            
            cout << "Query Size : " << radius << endl;

            Rectangle query(centerX - radius, centerX + radius, centerY - radius, centerY + radius);

            rtree -> diskIO_ = 0;

            auto start = chrono::high_resolution_clock::now();

            vector<int> pois = rtree -> queryMBR(query);

            auto end = chrono::high_resolution_clock::now();
           
            diskIO[i] = rtree -> diskIO_;

            timeTaken[i] = chrono::duration_cast<chrono::nanoseconds>(end - start).count(); 

            timeTaken[i] *= 1e-6; // Cast to milliseconds

            cout << "DISK IOs : " << diskIO[i] << endl;
            cout << "Execution Time : " << fixed << timeTaken[i] << setprecision(9) << "ms" << endl;

            cout << endl;
        }
    }

    return 0;
}