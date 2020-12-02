#include "load.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

string pathPoiDB = "Data/db1.csv";
string pathEventsDB = "Data/db3.csv";

int32_t loadData(int32_t &bitmapSize, int32_t* &poiID, Rectangle* &MBRs, int32_t* &eventCounts, Event** &events, int32_t* &docID, int32_t** &bitmaps) {
    
    ifstream poiDB(pathPoiDB);

    // Assuming poiID,latitude,longitude,bitmap format for POI DB file
    // and poiID,eventID,startTime,endTime,bitmap format for Events DB file

    string line;
    getline(poiDB, line); // Get first line

    // Create vectors to store the data
    vector<int32_t> ids;
    vector<long double> latitudes;
    vector<long double> longitudes;
    vector<vector<int32_t>> bmaps;
    vector<int32_t> docs;
    unordered_map<int32_t, int32_t> idIdx;

    // Read POI DB
    while (getline(poiDB, line)) {
        stringstream ss(line);

        if (line.length() < 2) continue; // Blank line

        docs.push_back(ids.size());

        int val_id;
        ss >> val_id;
        ids.push_back(val_id);

        idIdx[val_id] = ids.size() - 1;

        ss.ignore();

        long double lat;
        ss >> lat;
        latitudes.push_back(lat);

        ss.ignore();

        long double lon;
        ss >> lon;
        longitudes.push_back(lon);

        ss.ignore();

        vector<int32_t> bmap;

        char x;
        ss >> x;
        while (ss) {
            bmap.push_back(x - '0');
            ss >> x;
        }
        bmaps.push_back(bmap);
    }

    poiDB.close();

    int n_poi = ids.size();

    vector<Event>* eventList = new vector<Event>[n_poi];


    // Open events DB
    ifstream eventsDB(pathEventsDB);

    // Read first line
    getline(eventsDB, line);

    while (getline(eventsDB, line)) {
        
        stringstream ss(line);

        if (line.length() < 2) continue; // Blank line

        int poi;
        ss >> poi;

        ss.ignore();

        int eventID;
        ss >> eventID;

        ss.ignore();

        int startTime;
        ss >> startTime;
        
        ss.ignore();

        int endTime;
        ss >> endTime;
        
        ss.ignore();

        vector<uint8_t> bmap;

        char x;
        ss >> x;
        while (ss) {
            bmap.push_back(x - '0');
            ss >> x;
        }
        
        uint8_t* bmapArray = new uint8_t[bmap.size()];
        for (int i = 0; i < bmap.size(); i++) bmapArray[i] = bmap[i];

        Event temp(startTime, endTime, bmap.size(), bmapArray, eventID);

        eventList[idIdx[poi]].push_back(temp);
    }

    // int32_t* &poiID, Rectangle* &MBRs, int32_t* &eventCounts, Event** &events, int32_t* &docID, int32_t** &bitmaps
    
    // Start intializing
    bitmapSize = bmaps[0].size();
    poiID = new int32_t[n_poi];
    MBRs = new Rectangle[n_poi];
    eventCounts = new int32_t[n_poi];
    events = new Event*[n_poi];
    docID = new int32_t[n_poi];
    bitmaps = new int32_t*[n_poi];

    for (int i = 0; i < n_poi; i++) {
        poiID[i] = ids[i];
        Rectangle temp(longitudes[i], longitudes[i], latitudes[i], latitudes[i]);
        MBRs[i] = temp;
        eventCounts[i] = eventList[i].size();
        events[i] = new Event[eventCounts[i]];
        for (int j = 0; j < eventList[i].size(); j++) {
            events[i][j] = eventList[i][j];
        }
        docID[i] = docs[i];
        bitmaps[i] = new int32_t[bitmapSize];
        for (int j = 0; j < bitmapSize; j++) {
            bitmaps[i][j] = bmaps[i][j];
        }
    }

    return n_poi;
}