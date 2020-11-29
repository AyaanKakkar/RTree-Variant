#include "event.h"

// Constructor
Event::Event(uint16_t startTime, uint16_t endTime, int32_t bitmapSize, uint8_t* bitmap, int32_t id) {
    
    // Initialize values
    startTime_ = startTime;
    endTime_ = endTime;
    eventID_ = id;
    bitmapSize_ = bitmapSize;
    
    // Copy bitmap
    bitmap_ = new uint8_t[bitmapSize_];
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        bitmap_[idx] = bitmap[idx];
    }

}



// Checks if the event is occurring at teh given time interval
bool Event::existsAtTimeInterval(uint16_t startTime, uint16_t endTime) {
    return endTime_ >= startTime && startTime_ <= endTime;
}
