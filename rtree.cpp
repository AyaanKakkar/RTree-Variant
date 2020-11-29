#include <typeinfo>
#include <iostream>
#include <cstring>
#include <deque>
#include <vector>
#include "rtree.h"

#define MIN(a, b) (a) < (b) ? a : b

using namespace std;

// Create a RTree with the given paramters
RTree* RTree::CreateIndex(string fileName, int32_t pageSize, int32_t maxEntries, int32_t minEntries, int32_t bitmapSize) {
    // Create a new RTree
    RTree* rTree = new RTree();
    
    // Set parameters
    rTree -> rootPageOffset_ = 1;
    rTree -> pageSize_ = pageSize;
    rTree -> maxEntries_ = maxEntries;
    rTree -> minEntries_ = minEntries;
    rTree -> bitmapSize_ = bitmapSize;
    rTree -> treeSize_ = 0;
    rTree -> rootID_ = -1;
    rTree -> rootNode_ = nullptr;
    rTree -> nodesAccessed_ = 0;
    rTree -> diskIO_ = 0;
    rTree -> nextPage_ = 1;
    rTree -> nextDoc_ = -2;

    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out | ios::trunc);

    // Save the tree
    rTree -> saveTree();

    return rTree;
}

// Insert an entry into the RTree
void RTree::insert(Rectangle MBR, int32_t* bitmap, int32_t pointer, int32_t doc, Event* events, int32_t eventsCount) {
    if (treeSize_ == 0) {
        // If it's an empty tree then create a root node
        rootNode_ = new LeafNode(maxEntries_, minEntries_, bitmapSize_);
    }
    
    // Insert into the root
    int32_t newNodeID = this -> rootNode_ -> insert(MBR, bitmap, pointer, doc, events, eventsCount, this);

    if (newNodeID != -1) {
        // Get the new node
        TreeNode* newNode = this -> getNode(newNodeID);

        // Create a new root
        TreeNode* newRoot = new TreeNode(maxEntries_, minEntries_, bitmapSize_);

        // Add the new node
        newRoot -> addEntry(newNode -> currentMBR_, newNode -> nodeBitmap_, newNode -> childEvents_, newNode -> currEntries_, newNode -> doc_, newNodeID); 

        // Add the current root
        newRoot -> addEntry(rootNode_ -> currentMBR_, rootNode_ -> nodeBitmap_, rootNode_ -> childEvents_, rootNode_ -> currEntries_, rootNode_ -> doc_, rootNode_ -> nodeID_);

        // Create document
        newRoot -> createDoc(this);

        // Free the memory
        delete rootNode_;

        // Change the root
        rootNode_ = newRoot;

        // Save the root
        saveNode(rootNode_);
    }

    // Update root ID if necessary
    if (rootID_ != rootNode_ -> nodeID_) {
        rootID_ = rootNode_ -> nodeID_;
        saveTree();
    }
}

// Save a node
void RTree::saveNode(TreeNode* node) {
    bool flag = false;
    if (node -> nodeID_ == -1) {
        flag = true;
        treeSize_++;
        this -> saveTree();
    }
    writeNode(node);
    diskIO_++;
}


// Query
vector<int32_t> RTree::queryMBR(Rectangle MBR) {
    vector<int32_t> a;
    rootNode_ -> query(MBR, a, this);
    return a;
}

// Retrieve a node using ID
TreeNode* RTree::getNode(int32_t id) {
    TreeNode* node = this -> readNode(id);
    node -> nodeID_ = id;
    nodesAccessed_++;
    diskIO_++;
    return node;
}

// Returns node data and size
int32_t RTree::getNodeData(TreeNode* node, char* &a) {
    // Calculate the size of the node
    int32_t size = 0;

    size += sizeof(bool); // Node flag
    size += sizeof(node -> currEntries_);
    size += sizeof(node -> currentMBR_);
    size += sizeof(node -> currentMBR_) * node -> currEntries_;
    size += sizeof(node -> doc_);
    size += sizeof(node -> doc_) * node -> currEntries_;
    size += sizeof(int32_t) * node -> currEntries_;
    size += sizeof(node -> nodeBitmap_[0]) * node -> bitmapSize_;
    size += sizeof(node -> nodeBitmap_[0]) * node -> bitmapSize_ * node -> currEntries_;
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        size += sizeof(size_t);
        for (list<Event>::iterator it = node -> childEvents_[idx].begin(); it != node -> childEvents_[idx].end(); it++) {
            size += sizeof(*it);
        }
    }

    a = new char[size];
    
    char* curr;
    curr = a;

    // Check if it's a leaf node
    bool flag = (typeid(*node) == typeid(LeafNode));

    // Save the type of the node
    memcpy(curr, (char *) &flag, sizeof(flag));
    curr += sizeof(flag);

    // Save the number of entries
    memcpy(curr, (char *) &(node -> currEntries_), sizeof(node -> currEntries_));
    curr += sizeof(node -> currEntries_);

    // Save the MBRs
    memcpy(curr, (char *) &(node -> currentMBR_), sizeof(node -> currentMBR_));
    curr += sizeof(node -> currentMBR_);

    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy(curr, (char *) &(node -> MBR_[idx]), sizeof(node -> MBR_[idx]));
        curr += sizeof(node -> MBR_[idx]);
    }

    // Save the docs
    memcpy(curr, (char *) &(node -> doc_), sizeof(node -> doc_));
    curr += sizeof(node -> doc_);

    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy(curr, (char *) &(node -> docs_[idx]), sizeof(node -> docs_[idx]));
        curr += sizeof(node -> docs_[idx]);
    }

    // Save the child Pointers
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy(curr, (char *) &(node -> childPointers_[idx]), sizeof(node -> childPointers_[idx]));
        curr += sizeof(node -> childPointers_[idx]);
    }

    // Save the bitmaps
    for (int32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
        memcpy(curr, (char *) &(node -> nodeBitmap_[bitmapIdx]), sizeof(node -> nodeBitmap_[bitmapIdx]));
        curr += sizeof(node -> nodeBitmap_[bitmapIdx]);
    }

    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        for (int32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
            memcpy(curr, (char *) &(node -> bitmap_[idx][bitmapIdx]), sizeof(node -> bitmap_[idx][bitmapIdx]));
            curr += sizeof(node -> bitmap_[idx][bitmapIdx]);
        }
    }

    // Save the events
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        // Save list size 
        size_t listSize = node -> childEvents_[idx].size();
        memcpy(curr, (char *) &(listSize), sizeof(listSize));
        curr += sizeof(listSize);

        // Save list
        for (list<Event>::iterator it = node -> childEvents_[idx].begin(); it != node -> childEvents_[idx].end(); it++) {
            memcpy(curr, (char *) &(*it), sizeof(*it));
            curr += sizeof(*it);
        }
    }

    return size;
}


// Write a node to the file
// The initial bytes will be reserved to save the next page for the node
// Initial metadata would also contain the number of useful bytes to read in this page 
void RTree::writeNode(TreeNode* node) {
    
    int32_t pageAvailable = pageSize_ - 2 * sizeof(int32_t);

    char *a; // Node data to be written
    
    int32_t size = getNodeData(node, a);

    deque <int32_t> pages;

    if (node -> nodeID_ == -1) {
        node -> nodeID_ = nextPage_;
        nextPage_++;
        int32_t bytesLeft = size - pageAvailable;
        while (bytesLeft > 0) {
            bytesLeft -= pageAvailable;
            pages.push_back(nextPage_);
            nextPage_++;
        }
    }
    else {
        filePointer_.clear();
        filePointer_.seekg(pageSize_ * node -> nodeID_);
        int32_t nextPage;
        filePointer_.read((char*) &nextPage, sizeof(nextPage));

        int32_t bytesLeft = size - pageAvailable;
        while (nextPage != -1 && bytesLeft > 0) {
            pages.push_back(nextPage);
            filePointer_.clear();
            filePointer_.seekg(nextPage * pageSize_);
            filePointer_.read((char*) &nextPage, sizeof(nextPage));
            bytesLeft -= pageAvailable;
        }

        while (bytesLeft > 0) {
            bytesLeft -= pageAvailable;
            pages.push_back(nextPage_);
            nextPage_++;
        }  
    }

    int32_t currPage = node -> nodeID_;

    int32_t bytesLeft = size;

    char* curr;
    curr = a;



    while (bytesLeft > 0) {
        filePointer_.clear();
        filePointer_.seekp(pageSize_ * currPage);
        int32_t nextPage = (pages.empty()) ? -1 : pages.front();
        if (!pages.empty()) pages.pop_front();
        int32_t bytesUsed = MIN(bytesLeft, pageAvailable);
        bytesLeft -= bytesUsed;

        filePointer_.write((char*) &nextPage, sizeof(nextPage));
        filePointer_.write((char*) &bytesUsed, sizeof(bytesUsed));
        filePointer_.write(curr, bytesUsed);
        curr += bytesUsed;
        currPage = nextPage;
    }

    filePointer_.flush();
}


int32_t RTree::readNodeData(int32_t page, char* &a) {
    int32_t currPage = page;
    int32_t size = 0;

    char* pageData;

    deque <char*> pages;

    while (currPage != -1) {
        filePointer_.clear();
        filePointer_.seekg(currPage * pageSize_);
        pageData = new char[pageSize_];
        filePointer_.read(pageData, pageSize_);
        int32_t bytesUsed;
        memcpy((char*) &currPage, pageData, sizeof(currPage));
        memcpy((char*) &bytesUsed, (pageData + sizeof(currPage)), sizeof(bytesUsed));
        size += bytesUsed;
        pages.push_back(pageData);
    }

    a = new char[size];

    char* curr;
    curr = a;

    currPage = page;
    
    while (currPage != -1) {
        filePointer_.clear();
        pageData = pages.front();
        int32_t bytesUsed;
        memcpy((char*) &currPage, pageData, sizeof(currPage));
        memcpy((char*) &bytesUsed, (pageData + sizeof(currPage)), sizeof(bytesUsed));
        memcpy(curr, (pageData + sizeof(currPage) + sizeof(bytesUsed)), bytesUsed);
        curr += bytesUsed;
        pages.pop_front();
        delete[] pageData;
    }

    return size;
}

// Read the node from the file
TreeNode* RTree::readNode(int32_t page) {

    char *a; // Node data
    int32_t size = readNodeData(page, a);

    char* curr;
    curr = a;

    // Get the type of the node
    bool leaf;
    memcpy((char *) &leaf, curr, sizeof(leaf));
    curr += sizeof(leaf);

    // Create the node
    TreeNode* node;
    if (leaf) {
        node = new LeafNode(maxEntries_, minEntries_, bitmapSize_);
    }
    else {
        node = new TreeNode(maxEntries_, minEntries_, bitmapSize_);
    }

    // Retrieve the relevant info
    memcpy((char *) &(node -> currEntries_), curr, sizeof(node -> currEntries_));
    curr += sizeof(node -> currEntries_);

    // Retrieve MBRs
    memcpy((char *) &(node -> currentMBR_), curr, sizeof(node -> currentMBR_));
    curr += sizeof(node -> currentMBR_);
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy((char *) &(node -> MBR_[idx]), curr, sizeof(node -> MBR_[idx]));
        curr += sizeof(node -> MBR_[idx]);
    }

    // Retrieve Docs
    memcpy((char *) &(node -> doc_), curr, sizeof(node -> doc_));
    curr += sizeof(node -> doc_);
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy((char *) &(node -> docs_[idx]), curr, sizeof(node -> docs_[idx]));
        curr += sizeof(node -> docs_[idx]);
    }   

    // Retrieve pointers
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy((char *) &(node -> childPointers_[idx]), curr, sizeof(node -> childPointers_[idx]));
        curr += sizeof(node -> childPointers_[idx]);
    }


    // Retrieve bitmaps
    for (int32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
        memcpy((char *) &(node -> nodeBitmap_[bitmapIdx]), curr, sizeof(node -> nodeBitmap_[bitmapIdx]));
        curr += sizeof(node -> nodeBitmap_[bitmapIdx]);
    }

    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        for (int32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
            memcpy((char *) &(node -> bitmap_[idx][bitmapIdx]), curr, sizeof(node -> bitmap_[idx][bitmapIdx]));
            curr += sizeof(node -> bitmap_[idx][bitmapIdx]);
        }
    }

    // Retrieve events
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        
        // Read List size
        int32_t size;
        memcpy((char *) &(size), curr, sizeof(size));
        curr += sizeof(size);

        // Read List
        Event value;
        for (int32_t i = 0; i < size; i++) {
            memcpy((char *) &(value), curr, sizeof(value));
            curr += sizeof(value);
            node -> childEvents_[idx].push_back(value);

            // Get iterator to that event
            list<Event>::iterator it = node -> childEvents_[idx].end();
            advance(it, -1);

            // Loop over all time slots
            for (int32_t slotIdx = 0; slotIdx < TIMESLOTS; slotIdx++) {
                
                uint16_t slotDuration = (60 * 24) / TIMESLOTS;
                uint16_t startTime = slotDuration * slotIdx;
                uint16_t endTime = startTime + slotDuration - 1;
                
                // Add to time slot if required
                if (value.existsAtTimeInterval(startTime, endTime)) {
                    node -> childTimeSlots_[idx][slotIdx].push_back(it);
                }
            }
        }

    }

    return node;
}


// Save the tree to the file
void RTree::saveTree() {
    // Place the write pointer in the beginning
    filePointer_.clear();
    filePointer_.seekp(0);

    // Save the relevant info
    filePointer_.write((char *) &rootPageOffset_, sizeof(rootPageOffset_));
    
    filePointer_.write((char *) &treeSize_, sizeof(treeSize_));

    filePointer_.write((char *) &rootID_, sizeof(rootID_));

    filePointer_.write((char *) &pageSize_, sizeof(pageSize_));

    filePointer_.write((char *) &maxEntries_, sizeof(maxEntries_));

    filePointer_.write((char *) &minEntries_, sizeof(minEntries_));

    filePointer_.write((char *) &bitmapSize_, sizeof(bitmapSize_)); 

    filePointer_.write((char *) &nextPage_, sizeof(nextPage_));  

    filePointer_.write((char *) &nextDoc_, sizeof(nextDoc_));  

    filePointer_.flush();
    
}


// Load an existing RTree from the given file
RTree* RTree::LoadIndex(string fileName) {
    // Create a RTree
    RTree* rTree = new RTree();

    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out);

    // Read the relevant info
    rTree -> filePointer_.read((char *) &(rTree -> rootPageOffset_), sizeof(rTree -> rootPageOffset_));
    
    rTree -> filePointer_.read((char *) &(rTree -> treeSize_), sizeof(rTree -> treeSize_));

    rTree -> filePointer_.read((char *) &(rTree -> rootID_), sizeof(rTree -> rootID_));

    rTree -> filePointer_.read((char *) &(rTree -> pageSize_), sizeof(rTree -> pageSize_));

    rTree -> filePointer_.read((char *) &(rTree -> maxEntries_), sizeof(rTree -> maxEntries_));

    rTree -> filePointer_.read((char *) &(rTree -> minEntries_), sizeof(rTree -> minEntries_));

    rTree -> filePointer_.read((char *) &(rTree -> bitmapSize_), sizeof(rTree -> bitmapSize_));

    rTree -> filePointer_.read((char *) &(rTree -> nextPage_), sizeof(rTree -> nextPage_));  

    rTree -> filePointer_.read((char *) &(rTree -> nextDoc_), sizeof(rTree -> nextDoc_));  

    // Get the root node
    rTree -> rootNode_ = nullptr;
    if (rTree -> rootID_ != -1) {
        rTree -> rootNode_ = rTree -> getNode(rTree -> rootID_);
    }

    return rTree;
}

// Print the entire tree
void RTree::printTree() {
    cout << "----------------R Tree---------------" << endl;
    cout << "Expected Root ID : " << rootID_ << endl;
    cout << "Actual Root ID : " << rootNode_ -> nodeID_ << endl;
    cout <<  endl;
    rootNode_ -> printTree(this);
    cout << "-------------------------------------" << endl;
}

// Destructor
RTree::~RTree() {
    delete rootNode_;
    filePointer_.close();
}