#ifndef _LOADDATA
#define _LOADDATA
#include <cstdint>
#include <rectangle.h>
#include <event.h>
#include <vector>

int32_t loadData(int32_t* bitmapSize, int32_t* &poiID, Rectangle* &MBRs, int32_t* &eventCounts, Event** &events, int32_t* &docID, int32_t** &bitmaps);

#endif