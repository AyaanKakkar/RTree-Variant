#include "event.h"

// Constructor
Event::Event(uint16_t startTime, uint16_t endTime, int32_t bitmapSize, uint8_t* bitmap, long double x, long double y, int32_t id) {
    
    // Initialize values
    startTime_ = startTime;
    endTime_ = endTime;
    x_ = x;
    y_ = y;
    eventID_ = id;
    bitmapSize_ = bitmapSize;
    
    // Copy bitmap
    bitmap_ = new uint8_t[bitmapSize_];
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        bitmap_[idx] = bitmap[idx];
    }

}

// Check if the event is located in a particular region
bool Event::locatedInside(Rectangle rect) {
    return rect.containsPoint(x_, y_);
}


// Checks if the event is occurring at teh given time interval
bool Event::existsAtTimeInterval(uint16_t startTime, uint16_t endTime) {
    return endTime_ >= startTime && startTime_ <= endTime;
}
