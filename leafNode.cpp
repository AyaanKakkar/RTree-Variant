#include "leafNode.h"
#include "rtree.h"
#include <iostream>
#include <list>

using namespace std;

LeafNode::LeafNode(int32_t maxEntries, int32_t minEntries, int32_t bitmapSize) : TreeNode(maxEntries, minEntries, bitmapSize) {

}

// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
int32_t LeafNode::insert(Rectangle MBR, int32_t* bitmap, int32_t pointer, int32_t doc, Event* events, int32_t eventsCount, RTree* rTree) {

    // cout << "Leaf Node Reached" << endl;

    addLeafEntry(MBR, bitmap, pointer, events, eventsCount, doc);
    
    // cout << "Leaf Node Entries Added" << endl;


    // Perform splitting if necessary
    if (currEntries_ > maxEntries_) {
        // cout << "Leaf Node Splitting" << endl;
        return split(rTree);
    }

    // cout << "Leaf Node Creating Doc" << endl;
    // Create doc
    createDoc(rTree);


    // cout << "Leaf Node Saving" << endl;
    // Save changes to the disk
    rTree -> saveNode(this);

    // cout << "Leaf Node Insert Ending" << endl;
    return -1;
}

// Adds an entry to the leaf
void LeafNode::addLeafEntry(Rectangle MBR, int32_t* bitmap, int32_t pointer, Event* events, int32_t eventsCount, int32_t doc) {
    // Convert array of events to a list of events
    list<Event> eventsList;
    for (int32_t idx = 0; idx < eventsCount; idx++) {
        eventsList.push_back(events[idx]);
    }

    // Call add Entry
    addEntry(MBR, bitmap, &eventsList, 1, doc, pointer);
}

// Print the tree
void LeafNode::printTree(RTree* rTree) {
    cout << "Node ID : " << nodeID_ << endl;
    cout << "Node MBR :- ";
    currentMBR_.print();
    cout << endl;
    cout << "Node Bitmap :- ";
    cout << "[";
    for (int idx = 0; idx < bitmapSize_; idx++) {
        cout << nodeBitmap_[idx];
        if (idx != bitmapSize_ - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
    cout << "Doc ID : " << doc_ << endl;
    cout << "Current Entries : " << currEntries_ << endl;
    cout << "Child Pointers : ";
    for (int idx = 0; idx < currEntries_; idx++) {
        cout << childPointers_[idx];
        cout << ", MBR :- ";
        MBR_[idx].print();
        cout << "; Doc ID : " << docs_[idx];
        cout << "; Bitmap :- ";
        cout << "[";
        for (int bitmapIdx = 0; bitmapIdx < bitmapSize_; bitmapIdx++) {
            cout << bitmap_[idx][bitmapIdx];
            if (bitmapIdx != bitmapSize_ - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }

    cout << endl;
}


// Create a new split node
TreeNode* LeafNode::createSplitNode() {
    return new LeafNode(maxEntries_, minEntries_, bitmapSize_);
}

// Query
void LeafNode::query(Rectangle MBR, vector<int32_t>& a, RTree* rTree) {
    
    Rectangle b = Rectangle::intersection(currentMBR_, MBR);

    if (b.isValid()) {
        for (int i = 0; i < currEntries_; i++) {
            Rectangle x = Rectangle::intersection(MBR_[i], MBR);
            if (x.isValid()) {
                a.push_back(childPointers_[i]);
            }
        }
    }
}
