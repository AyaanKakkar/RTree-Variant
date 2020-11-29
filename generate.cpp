#include <generate.h>
#include <cstdint>
#include <vector>
#include <rectangle.h>
#include <event.h>
#include <ctime>
#include <cstdlib>
#include <cmath>

const int EVENT_COUNT = 1;

const int bucketsize = 64;
const int rows = 8;
const int cols = 8;

const int EVENT_BITMAP = 3;

int32_t generateData(int32_t size, double skew, int32_t bitmapSize, int32_t* &poiID, Rectangle* &MBRs, int32_t* &eventCounts, Event** &events, int32_t* &docID, int32_t** &bitmaps) {
    
    int poiCount = size;
    
    poiID = new int[poiCount];

    for (int i = 0; i < poiCount; i++) {
        poiID[i] = i;
    }
    
    MBRs = new Rectangle[poiCount];
    
    eventCounts = new int[poiCount];
    events = new Event*[poiCount];
    
    for (int i = 0; i < poiCount; i++) {
        eventCounts[i] = EVENT_COUNT;
        events[i] = new Event[EVENT_COUNT];
        for (int j = 0; j < EVENT_COUNT; j++) {
            int eventID = i*EVENT_COUNT + j;
            int startTime = rand() % 700;
            int endTime = (rand() % 700) + startTime;
            uint8_t* bitmap = new uint8_t[EVENT_BITMAP];
            for (int k = 0; k < EVENT_BITMAP; k++) {
                bitmap[k] = rand() % 2;
            }
            Event temp(startTime, endTime, EVENT_BITMAP, bitmap, eventID);
            events[i][j] = temp;
        }
    }

    docID = new int[poiCount];

    for (int i = 0; i < poiCount; i++) {
        docID[i] = i;
    }
    
    bitmaps = new int*[poiCount];
    for (int i = 0; i < poiCount; i++) {
        bitmaps[i] = new int[bitmapSize];

        for (int j = 0; j < bitmapSize; j++) {
            bitmaps[i][j] = rand() % 2;
        }
    }

    vector<int> bucket = getBucketFreq(size, skew, bucketsize);

    int matrix[rows][cols];

    vector<int> temp(bucketsize);
    for (int i = 0; i < bucketsize; i++) {
        temp[i] = i;
    }

    for (int i = 0; i < bucketsize; i++) {
        int idx = rand() % (bucketsize - i);
        matrix[(int) i / cols][i % cols] = bucket[temp[idx]];
        temp[idx] = temp[bucketsize - i - 1];
    }

    int currIdx = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < matrix[i][j]; k++) {
                int t1 = rand() % (RAND_MAX - 1);
                int t2 = t1 + (rand() % (RAND_MAX - t1));
                long double minX = (long double)j + (((long double)t1) / RAND_MAX);
                long double maxX = (long double)j + (((long double)t2) / RAND_MAX);
                int t3 = rand() % (RAND_MAX - 1);
                int t4 = t3 + (rand() % (RAND_MAX - t3));
                long double minY = (long double)i + (((long double)t3) / RAND_MAX);
                long double maxY = (long double)i + (((long double)t4) / RAND_MAX);

                Rectangle temp(minX, maxX, minY, maxY);
                MBRs[currIdx++] = temp;
            }
        }
    }

    return poiCount;
}


vector<int> getBucketFreq(int32_t size, double skew, int32_t bucketsize) {
    // Calculate bottom
    double bottom = 0.0;
    for(int i=1;i <= bucketsize; i++) {
      bottom += (1/pow(i, skew));
    }
    
    
    // Calculate bucketFrequency
    vector<int> bucket(bucketsize);

    int remainSize = 0;
    for (int i=0;i<bucketsize;i++) {
        bucket[i] = (size * ((1.0 / pow((i+1), skew)) / bottom));
        remainSize += bucket[i];
    }
    remainSize = size - remainSize;
    for (int i=0; i<remainSize;i++){
        bucket[i]++;
    }

    return bucket;
}