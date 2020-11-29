#ifndef _EVENT
#define _EVENT

#include <cstdint>
#include "rectangle.h"

using namespace std;

class Event {
    
    /*
        Used to declare an event to be used inside R-Tree
    */
    
    private:
    
    
    public:
        Event(){};
        Event(uint16_t startTime, uint16_t endTime, int32_t bitmapSize, uint8_t* bitmap, int32_t id);
        bool locatedInside(Rectangle rect);
        bool existsAtTimeInterval(uint16_t startTime, uint16_t endTime);

        uint16_t startTime_; // Starting time in minutes from 12 A.M.
        
        uint16_t endTime_;   // Ending time in minutes from 12 A.M.

        int32_t bitmapSize_; // Size of bitmap

        uint8_t* bitmap_; // Bitmap for type of event

        int32_t eventID_; // ID of event
};

#endif