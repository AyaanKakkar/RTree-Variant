#include "rtree.h"
#include "generate.h"
#include "load.h"
#include "saveGraph.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>

using namespace std;

const string indexFileName = "exp0.bin";

const int32_t pageSize = 4096;
const int32_t maxEntries = 16;
const int32_t minEntries = 4;
const int32_t defaultBitmapSize = 5;
const double skewness_zipf = 0.7;
vector<int> sizes{100, 1000, 10000};
const int n_sizes = 3;

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

    if (query == -1) {
        poi_count = loadData(bitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
    }
    else {
        bitmapSize = defaultBitmapSize;
        poi_count = generateData(query, skewness_zipf, bitmapSize, poiID, MBRs, eventCounts, events, docID, bitmaps);
    }
}



// Returns number of disk IOs
int buildRTree() {
    
    rtree = RTree::CreateIndex(indexFileName, pageSize, maxEntries, minEntries, bitmapSize);

    for (int32_t i = 0; i < poi_count; i++) {
        rtree -> insert(MBRs[i], bitmaps[i], poiID[i], docID[i], events[i], eventCounts[i]);
    }
    return rtree -> diskIO_;
}


int main(int argc, char* argv[]) {
       
    if (argc == 2 && (argv[1][0] == 'r' || argv[1][0] == 'R')) {
        // Real Data
        string outputFNameTime = "exp0_real_time.data";
        string outputFNameIO = "exp0_real_IO.data";

        remove(indexFileName.c_str());
        getData(-1);

        cout << "Using Real Data" << endl;

        auto start = chrono::high_resolution_clock::now();

        int diskIO = buildRTree();

        auto end = chrono::high_resolution_clock::now();
        
        long double timeTaken = chrono::duration_cast<chrono::nanoseconds>(end - start).count(); 

        timeTaken *= 1e-6; // Cast to milliseconds

        cout << "DISK IOs : " << diskIO << endl;
        cout << "Build Time : " << fixed << timeTaken << setprecision(12) << "ms" << endl;

        cout << endl;

        cout << endl;
        cout << "Writing output to file: " << outputFNameTime << endl;
        saveData(outputFNameTime, vector<int> {poi_count}, vector<long double>{timeTaken});

        cout << "Writing output to file: " << outputFNameIO << endl;
        saveData(outputFNameIO, vector<int> {poi_count}, vector<int> {diskIO});
    }
    else {
        // Synthetic Data
        string outputFNameTime = "exp0_synthetic_time.data";
        string outputFNameIO = "exp0_synthetic_IO.data";

        vector<long double> timeTaken(n_sizes);
        vector<int> diskIO(n_sizes);  

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
            cout << "Build Time : " << fixed << timeTaken[i] << setprecision(12) << "ms" << endl;

            cout << endl;
        }


        cout << endl;
        cout << "Writing output to file: " << outputFNameTime << endl;
        saveData(outputFNameTime, sizes, timeTaken);

        cout << "Writing output to file: " << outputFNameIO << endl;
        saveData(outputFNameIO, sizes, diskIO);

    }
    return 0;
}