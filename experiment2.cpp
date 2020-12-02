#include "rtree.h"
#include "generate.h"
#include "saveGraph.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <vector>

using namespace std;

const string indexFileName = "exp2.bin";

const int32_t pageSize = 4096;
const int32_t maxEntries = 64;
const int32_t minEntries = 16;
const int32_t bitmapSize = 5;
const int n_pois = 5000;
const int radius = 2;
vector<long double> factors{0.1, 0.3, 0.5, 0.7, 0.9};
const int n_sizes = 5;


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
    
    poi_count = generateData(n_pois, factors[query], bitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
}

// Returns number of disk IOs
void buildRTree() {
    
    rtree = RTree::CreateIndex(indexFileName, pageSize, maxEntries, minEntries, bitmapSize);

    for (int32_t i = 0; i < poi_count; i++) {
        rtree -> insert(MBRs[i], bitmaps[i], poiID[i], docID[i], events[i], eventCounts[i]);
    }

}


int main(int argc, char* argv[]) {

    
    vector<long double> timeTaken(n_sizes);
    vector<int> diskIO(n_sizes);

    string outputFNameTime = "exp2_time.data";
    string outputFNameIO = "exp2_IO.data";

    for (int i = 0; i < n_sizes; i++) {

        remove(indexFileName.c_str());
        getData(i);

        cout << "Zipf Factor : " << factors[i] << endl;

        buildRTree();

        rtree -> diskIO_ = 0;

        // Get query rectangle mid point
        long double centerX = 0;
        long double centerY = 0;

        for (int j = 0; j < poi_count; j++) {
            centerX += MBRs[j].getCenterX();
            centerY += MBRs[j].getCenterY();
        }

        centerX /= poi_count;
        centerY /= poi_count;


        Rectangle query(centerX - radius, centerX + radius, centerY - radius, centerY + radius);

        auto start = chrono::high_resolution_clock::now();

        vector<int> pois = rtree -> queryMBR(query);

        auto end = chrono::high_resolution_clock::now();

        
        diskIO[i] = rtree -> diskIO_;

        timeTaken[i] = chrono::duration_cast<chrono::nanoseconds>(end - start).count(); 

        timeTaken[i] *= 1e-6; // Cast to milliseconds

        cout << "DISK IOs : " << diskIO[i] << endl;
        cout << "Execution Time : " << fixed << timeTaken[i] << setprecision(12) << "ms" << endl;

        cout << endl;
    }

    cout << endl;
    cout << "Writing output to file: " << outputFNameTime << endl;

    saveData(outputFNameTime, factors, timeTaken);

    cout << "Writing output to file: " << outputFNameIO << endl;
    saveData(outputFNameIO, factors, diskIO);

    return 0;
}