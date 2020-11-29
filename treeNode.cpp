#include "treeNode.h"
#include "rtree.h"
#include <iostream>

using namespace std;

// Constructor
TreeNode::TreeNode(int32_t maxEntries, int32_t minEntries, int32_t bitmapSize) {
    
    // Initialize values
    nodeID_ = -1; // This would be the page where the node is stored
    maxEntries_ = maxEntries;
    minEntries_ = minEntries;
    bitmapSize_ = bitmapSize;

    currEntries_ = 0;
    
    // Initialzie MBRs
    currentMBR_ = Rectangle();

    MBR_ = new Rectangle[maxEntries_ + 1];

    // Initialize pointers    
    childPointers_ = new int32_t[maxEntries_ + 1];

    // Initialize bitmaps
    nodeBitmap_ = new int32_t[bitmapSize_]; 
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        nodeBitmap_[idx] = 0;
    }

    bitmap_ = new int32_t*[maxEntries_ + 1];
    for (int32_t idx = 0; idx < maxEntries_ + 1; idx++) {
        bitmap_[idx] = new int32_t[bitmapSize_];
        for (int32_t bitmapIdx = 0; bitmapIdx < bitmapSize_; bitmapIdx++) {
            bitmap_[idx][bitmapIdx] = 0;
        }
    }

    // Initialize events data
    childEvents_ = new list<Event>[maxEntries_ + 1];
    
    childTimeSlots_ = new list<list<Event>::iterator> *[maxEntries_ + 1];
    for (int32_t idx = 0; idx < maxEntries_ + 1; idx++) {
        childTimeSlots_[idx] = new list<list<Event>::iterator>[TIMESLOTS];
    }

    // Initialize document data
    doc_ = -1;
    docs_ = new int32_t[maxEntries_ + 1];
}


// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
int32_t TreeNode::insert(Rectangle MBR, int32_t* bitmap, int32_t pointer, int32_t doc, Event* events, int32_t eventsCount, RTree* rTree) {
    
    // cout << "Tree Node Reached" << endl;

    // Update the MBR
    currentMBR_ = Rectangle::combine(currentMBR_, MBR);

    // Add the bitmap to the entire node's bitmap
    addBitmap(bitmap);

    // Calculate the index of the child to traverse to
    int32_t childIdx = -1;
    long double minAreaEnlargement = 0;
    long double childArea = 0;

    for (int32_t index = 0; index < currEntries_; index++) {

        long double areaEnlargment = MBR_[index].getAreaEnlargement(MBR);
        long double area = MBR_[index].getArea();
        
        if (childIdx == -1 || areaEnlargment < minAreaEnlargement 
        || areaEnlargment == minAreaEnlargement && childArea > area) {
            childIdx = index;
            minAreaEnlargement = areaEnlargment;
            childArea = area;
        }
    } 


    // cout << "Adding Events to Child" << endl;
    // Add new events to child data
    addEventsToChild(events, eventsCount, childIdx);

    // cout << "Events Added" << endl;

    // Add bitmap to child
    addBitmapToChild(bitmap, childIdx);

    // cout << "Getting Child" << endl;
    // Get that child node
    TreeNode* childNode = rTree -> getNode(childPointers_[childIdx]);

    // cout << "Got Child" << endl;
    // Insert into that node
    int32_t newNodeID = childNode -> insert(MBR, bitmap, pointer, doc, events, eventsCount,rTree);

    // Update that node's MBR
    MBR_[childIdx] = childNode -> currentMBR_;

    // Perform addition if necessary
    if (newNodeID != -1) {
        
        // Update that child's bitmap and events
        updateChildBitmap(childNode->nodeBitmap_, childIdx);
        updateChildEvents(childNode->childEvents_, childNode -> currEntries_, childIdx);

        // Get the new node
        TreeNode* newNode = rTree -> getNode(newNodeID);

        // Add that node info
        addEntry(newNode -> currentMBR_, newNode -> nodeBitmap_, newNode -> childEvents_, newNode -> currEntries_, newNode -> doc_, newNodeID);   
        
        // Free the memory
        delete newNode;
    }

    // Free memory
    delete childNode;

    // Perform splitting if necessary and return the new node formed
    if (currEntries_ > maxEntries_) {
        return split(rTree);
    }

    // Create doc
    createDoc(rTree);

    // Save changes to the disk
    rTree -> saveNode(this);

    return -1;
}

// Add an entry in the node
void TreeNode::addEntry(Rectangle MBR, int32_t* bitmap, list<Event>* events, int32_t eventsListCount, int32_t doc, int32_t pointer) {

    // Update MBRs
    MBR_[currEntries_] = MBR;
    
    if (currEntries_ == 0) {
        currentMBR_ = MBR_[currEntries_];
    }
    else {
        currentMBR_ = Rectangle::combine(currentMBR_, MBR_[currEntries_]);
    }

    // Add doc data
    docs_[currEntries_] = doc;

    // Update bitmaps and events
    updateChildBitmap(bitmap, currEntries_);
    updateChildEvents(events, eventsListCount, currEntries_);

    childPointers_[currEntries_] = pointer;
    currEntries_++;
}

// Split the node and return the ID of the new node
int32_t TreeNode::split(RTree* rTree) {
    
    // Using linear splitting;
    
    int32_t index1 = 0;
    int32_t index2 = 1;
    long double maxDistance = Rectangle::distance(MBR_[index1], MBR_[index2]);
    
    // Loop over all possilbe pairs and find the pair with the largest distance 
    for (int32_t i = 0; i < currEntries_; i++) {
        for (int32_t j = i + 1; j < currEntries_; j++) {
            long double distance = Rectangle::distance(MBR_[i], MBR_[j]);
            if (distance > maxDistance) {
                index1 = i;
                index2 = j;
            } 
        }
    }   

    // Create 2 new nodes
    TreeNode* splitNode1 = createSplitNode();
    TreeNode* splitNode2 = createSplitNode();

    // Add Entries in index1 and index2
    addEntryToSplitNode(index1, splitNode1);
    addEntryToSplitNode(index2, splitNode2);

    int32_t remainingEntries = currEntries_ - 2;

    // Iterate over all remaining entries
    for (int32_t idx = 0; idx < currEntries_; idx++) {
        if (idx != index1 && idx != index2) {
            // Choose a node, and add the entry to that node
            addEntryToSplitNode(idx, chooseSplitNode(idx, splitNode1, splitNode2, remainingEntries));
            remainingEntries--;
        }
    }

    // Transfer the contents of one node to the current node
    copyNodeContent(splitNode1);

    // Create all docs
    createDoc(rTree);
    splitNode2 -> createDoc(rTree);

    // Save the changes to the disk
    rTree -> saveNode(splitNode2);  
    rTree -> saveNode(this);

    // Return ID
    int32_t returnID = splitNode2 -> nodeID_;

    // Free the memory
    delete splitNode1;
    delete splitNode2;

    // Return the newly creater node's ID
    return returnID;

}

// Helper function to choose node to add entry to
TreeNode* TreeNode::chooseSplitNode(int32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, int32_t remainingEntries) {
    // Check if minimum criteria needs to be satisfied
    if (splitNode1 -> currEntries_ == minEntries_ - remainingEntries) {
        return splitNode1;
    }
    else if (splitNode2 -> currEntries_ == minEntries_ - remainingEntries) {
        return splitNode2;
    }
    else {
        // Calculate area enlargment
        long double enlargementNode1 = splitNode1 -> currentMBR_.getAreaEnlargement(MBR_[entryIdx]);
        long double enlargementNode2 = splitNode2 -> currentMBR_.getAreaEnlargement(MBR_[entryIdx]);

        if (enlargementNode1 > enlargementNode2) {
            return splitNode2;
        }
        else if (enlargementNode2 > enlargementNode1) {
            return splitNode1;
        }
        else {        
            // If they are equal then assign on the basis of area
            long double areaNode1 = splitNode1 -> currentMBR_.getArea();
            long double areaNode2 = splitNode2 -> currentMBR_.getArea();

            if (areaNode1 > areaNode2) {
                return splitNode2;
            }
            else if(areaNode2 > areaNode1) {
                return splitNode1;
            }
            else {
                // Assign on the basis of number of entries
                if (splitNode1 -> currEntries_ > splitNode2 -> currEntries_) {
                    return splitNode2;
                }
                else {
                    return splitNode1;
                }
            }
        }
    }
}

// Create a split node
TreeNode* TreeNode::createSplitNode() {
    return new TreeNode(maxEntries_, minEntries_, bitmapSize_);
}

// Add entry to a split node
void TreeNode::addEntryToSplitNode(int32_t entryIdx, TreeNode* splitNode) {
    splitNode->addEntry(MBR_[entryIdx], bitmap_[entryIdx], &childEvents_[entryIdx], 1, docs_[entryIdx], childPointers_[entryIdx]);
}

// Copy content from a node
void TreeNode::copyNodeContent(TreeNode* node) {
    currEntries_ = node -> currEntries_;

    // Copy MBR
    currentMBR_ = node -> currentMBR_;

    for (int32_t idx = 0; idx < currEntries_; idx++) {
        MBR_[idx] = node -> MBR_[idx];
        childPointers_[idx] = node -> childPointers_[idx];
    }

    // Copy document data
    doc_ = node -> doc_;

    for (int32_t idx = 0; idx < currEntries_; idx++) {
        docs_[idx] = node -> docs_[idx];
    }

    copyNodeBitmap(node);
    copyNodeEvents(node);
}

// Destructor
TreeNode::~TreeNode() {
    delete MBR_;
    delete childPointers_;
    freeBitmap();
    freeEvents();
}

// Add Bitmap to the current node
void TreeNode::addBitmap(int32_t* bitmap) {
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        nodeBitmap_[idx] += bitmap[idx];
    }
}

// Add bitmap to child data
void TreeNode::addBitmapToChild(int32_t* bitmap, int32_t childIdx) {
    // Update child bitmap 
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        bitmap_[childIdx][idx] += bitmap[idx];
    }
}

// Add events to child data
void TreeNode::addEventsToChild(Event* events, int32_t eventsCount, int32_t childIdx) {
    // Loop over all events
    for (int32_t idx = 0; idx < eventsCount; idx++) {
        // Create a copy of the event
        Event temp = events[idx];

        // Add copy to list of all events
        childEvents_[childIdx].push_back(temp);

        // Get iterator to that event
        list<Event>::iterator it = childEvents_[childIdx].end();
        advance(it, -1);

        // Loop over all time slots
        for (int32_t slotIdx = 0; slotIdx < TIMESLOTS; slotIdx++) {
            
            uint16_t slotDuration = (60 * 24) / TIMESLOTS;
            uint16_t startTime = slotDuration * slotIdx;
            uint16_t endTime = startTime + slotDuration - 1;
            
            // Add to time slot if required
            if (temp.existsAtTimeInterval(startTime, endTime)) {
                childTimeSlots_[childIdx][slotIdx].push_back(it);
            }
        }
    }
}

// Update events of a child
void TreeNode::updateChildEvents(list<Event>* events, int32_t eventsListCount, int32_t childIdx) {
    
    // Clear everything
    childEvents_[childIdx].clear();
    for (int32_t idx = 0; idx < TIMESLOTS; idx++) {
        childTimeSlots_[childIdx][idx].clear();
    }

    // Loop over all lists
    for (int32_t listIdx = 0; listIdx < eventsListCount; listIdx++) {
        // Append list to the end 
        childEvents_[childIdx].insert(childEvents_[childIdx].end(), events[listIdx].begin(), events[listIdx].end());
    }

    // Hash into time slots
    // Loop over all events
    for (list<Event>::iterator it = childEvents_[childIdx].begin(); it != childEvents_[childIdx].end(); it++) {
        // Loop over all time slots
        for (int32_t slotIdx = 0; slotIdx < TIMESLOTS; slotIdx++) {
            
            uint16_t slotDuration = (60 * 24) / TIMESLOTS;
            uint16_t startTime = slotDuration * slotIdx;
            uint16_t endTime = startTime + slotDuration - 1;
            
            // Add to time slot if required
            if (it -> existsAtTimeInterval(startTime, endTime)) {
                childTimeSlots_[childIdx][slotIdx].push_back(it);
            }
        }
    }
}

// Change the bitmap of a child
void TreeNode::updateChildBitmap(int32_t* bitmap, int32_t childIdx) {
    // Update node bitmap
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        nodeBitmap_[idx] += (bitmap[idx] - bitmap_[childIdx][idx]);
    }

    // Update child bitmap 
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        bitmap_[childIdx][idx] = bitmap[idx];
    }
}

// Copy the bitmap contents of a node
void TreeNode::copyNodeBitmap(TreeNode* node) {
    // Update node bitmap
    for (int32_t idx = 0; idx < bitmapSize_; idx++) {
        nodeBitmap_[idx] = node -> nodeBitmap_[idx];
    }

    // Update child bitmap 
    for (int32_t entryIdx = 0; entryIdx < node -> currEntries_; entryIdx++) {
        for (int32_t idx = 0; idx < bitmapSize_; idx++) {
            bitmap_[entryIdx][idx] = node -> bitmap_[entryIdx][idx];
        }
    }

}

// Copy the events data of a node
void TreeNode::copyNodeEvents(TreeNode* node) {
    
    for (int32_t childIdx = 0; childIdx < node -> currEntries_; childIdx++) {
        // Clear everything
        childEvents_[childIdx].clear();
        for (int32_t idx = 0; idx < TIMESLOTS; idx++) {
            childTimeSlots_[childIdx][idx].clear();
        }

        // Copy list
        childEvents_[childIdx] = node -> childEvents_[childIdx];

        // Hash into time slots
        // Loop over all events
        for (list<Event>::iterator it = childEvents_[childIdx].begin(); it != childEvents_[childIdx].end(); it++) {
            // Loop over all time slots
            for (int32_t slotIdx = 0; slotIdx < TIMESLOTS; slotIdx++) {
                
                uint16_t slotDuration = (60 * 24) / TIMESLOTS;
                uint16_t startTime = slotDuration * slotIdx;
                uint16_t endTime = startTime + slotDuration - 1;
                
                // Add to time slot if required
                if (it -> existsAtTimeInterval(startTime, endTime)) {
                    childTimeSlots_[childIdx][slotIdx].push_back(it);
                }
            }
        }
    }
}

// Print the tree
void TreeNode::printTree(RTree* rTree) {
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
        cout << "EVENTS : " << childEvents_[idx].size() << endl; 
    }

    cout << endl;
    for (int idx = 0; idx < currEntries_; idx++) {
        cout << "~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        cout << "Parent Node ID : " << nodeID_ << endl;
        TreeNode* childNode = rTree -> getNode(childPointers_[idx]);
        childNode -> printTree(rTree);
    }
    cout << endl;
}

// Free the bitmap
void TreeNode::freeBitmap() {
    
    delete [] nodeBitmap_;

    delete [] bitmap_;
}

// Free events data
void TreeNode::freeEvents() {
    for (int32_t idx = 0; idx < maxEntries_ + 1; idx++) {
        childEvents_[idx].clear();
        for (int32_t slotIdx = 0; slotIdx < TIMESLOTS; slotIdx++) {
            childTimeSlots_[idx][slotIdx].clear();
        }
        delete[] childTimeSlots_[idx];
    }
    delete [] childEvents_;
    delete [] childTimeSlots_;
}

// Create or update document
void TreeNode::createDoc(RTree* rTree) {
    // Implementation left
    // Do an aggregation of documents
    if (doc_ == -1) {
        doc_ = rTree -> nextDoc_;
        rTree -> nextDoc_--;
    }
}



// Query
void TreeNode::query(Rectangle MBR, vector<int32_t>& a, RTree* rTree) {
    
    Rectangle b = Rectangle::intersection(currentMBR_, MBR);

    if (b.isValid()) {
        for (int i = 0; i < currEntries_; i++) {
            Rectangle x = Rectangle::intersection(MBR_[i], MBR);
            if (x.isValid()) {
                TreeNode* node = rTree -> getNode(childPointers_[i]);
                node -> query(MBR, a, rTree);
            }
        }
    }
}


