#ifndef _GENERATE
#define _GENERATE
#include <cstdint>
#include <rectangle.h>
#include <event.h>
#include <vector>

int32_t generateData(int32_t size, double skew, int32_t bitmapSize, int32_t* &poiID, Rectangle* &MBRs, int32_t* &eventCounts, Event** &events, int32_t* &docID, int32_t** &bitmaps);

vector<int> getBucketFreq(int32_t size, double skew, int32_t bucketsize);

#endif