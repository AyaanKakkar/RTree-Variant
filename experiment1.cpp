#include "rtree.h"
#include "generate.h"
#include "load.h"
#include "saveGraph.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <vector>

using namespace std;

const string indexFileName = "exp1.bin";

const int32_t pageSize = 4096;
const int32_t maxEntries = 64;
const int32_t minEntries = 16;
const int32_t defaultBitmapSize = 5;
const int n_pois = 5000;
const int zipf_skewness = 0.7;
const int n_sizes = 6;
vector<double> query_sizes(n_sizes);

int32_t bitmapSize;

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
        poi_count = loadData(bitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
    }
    else {
        bitmapSize = defaultBitmapSize;
        poi_count = generateData(n_pois, zipf_skewness, defaultBitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
    }
}

// Builds RTree
void buildRTree() {
    
    rtree = RTree::CreateIndex(indexFileName, pageSize, maxEntries, minEntries, bitmapSize);

    for (int32_t i = 0; i < poi_count; i++) {
        rtree -> insert(MBRs[i], bitmaps[i], poiID[i], docID[i], events[i], eventCounts[i]);
    }

}


int main(int argc, char* argv[]) {

    int indexQuery;
    string outputFNameTime;
    string outputFNameIO;

    if (argc == 2 && (argv[1][0] == 'r' || argv[1][0] == 'R')) {
        // Real Data
        indexQuery = 0;
        outputFNameTime = "exp1_real_time.data";
        outputFNameIO = "exp1_real_IO.data";
        query_sizes[0] = 0.1; query_sizes[1] = 0.2; query_sizes[2] = 0.3;
        query_sizes[3] = 0.4; query_sizes[4] = 0.5; query_sizes[5] = 0.6;
        cout << "Using Real Data" << endl;
    }
    else {
        // Synthetic Data
        indexQuery = 1;
        outputFNameTime = "exp1_synthetic_time.data";
        outputFNameIO = "exp1_synthetic_IO.data";
        query_sizes[0] = 0.5; query_sizes[1] = 1; query_sizes[2] = 1.5;
        query_sizes[3] = 2; query_sizes[4] = 2.5; query_sizes[5] = 3;
        cout << "Using Synthetic Data | Zipf Factor: " << zipf_skewness << " | POIs: " << n_pois << endl;
    }

    vector<long double> timeTaken(n_sizes);
    vector<int> diskIO(n_sizes);
    
    remove(indexFileName.c_str());
    getData(indexQuery);

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
        
        diskIO[j] = rtree -> diskIO_;

        timeTaken[j] = chrono::duration_cast<chrono::nanoseconds>(end - start).count(); 

        timeTaken[j] *= 1e-6; // Cast to milliseconds

        cout << "DISK IOs : " << diskIO[j] << endl;
        cout << "Execution Time : " << fixed << timeTaken[j] << setprecision(12) << "ms" << endl;

        cout << endl;
    }

    cout << endl;
    cout << "Writing output to file: " << outputFNameTime << endl;
    vector<int> outputTime;
    for (auto x: timeTaken) {
        outputTime.push_back((int)x);
    }
    saveGraph(outputFNameTime, query_sizes, outputTime);

    cout << "Writing output to file: " << outputFNameIO << endl;
    saveGraph(outputFNameIO, query_sizes, diskIO);

    return 0;
}