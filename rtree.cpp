#include <typeinfo>
#include <iostream>
#include "rtree.h"

using namespace std;

// Create a RTree with the given paramters
RTree* RTree::createIndex(string fileName, uint32_t pageSize, uint32_t maxEntries, uint32_t minEntries, uint32_t bitmapSize) {
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
    
    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out | ios::trunc);

    // Save the tree
    rTree -> saveTree();

    return rTree;
}

// Insert an entry into the RTree
void RTree::insert(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, uint32_t doc, Event* events, uint32_t eventsCount) {
    if (treeSize_ == 0) {
        // If it's an empty tree then create a root node
        rootNode_ = new LeafNode(maxEntries_, minEntries_, bitmapSize_);
    }
    
    // Insert into the root
    uint32_t newNodeID = this -> rootNode_ -> insert(MBR, bitmap, pointer, doc, events, eventsCount, this);

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
        newRoot -> createDoc();

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
    if (node -> nodeID_ == -1) {
        node -> nodeID_ = treeSize_;
        treeSize_++;
        this -> saveTree();
    }
    writeNode(node, node -> nodeID_ + rootPageOffset_);
    diskIO_++;
}

// Retrieve a node using ID
TreeNode* RTree::getNode(uint32_t id) {
    TreeNode* node = this -> readNode(id + rootPageOffset_);
    node -> nodeID_ = id;
    nodesAccessed_++;
    diskIO_++;
    return node;
}

// Write a node to the file
void RTree::writeNode(TreeNode* node, uint32_t page) {
    // Place the write pointer in the appropriate position
    filePointer_.seekp(page * pageSize_);
    
    // Check if it's a leaf node
    bool flag = (typeid(*node) == typeid(LeafNode));

    // Save the type of the node
    filePointer_.write((char *) &flag, sizeof(flag));

    // Save the number of entries
    filePointer_.write((char *) &(node -> currEntries_), sizeof(node -> currEntries_));
    
    // Save the MBRs
    filePointer_.write((char *) &(node -> currentMBR_), sizeof(node -> currentMBR_));
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.write((char *) &(node -> MBR_[idx]), sizeof(node -> MBR_[idx]));
    }

    // Save the docs
    filePointer_.write((char *) &(node -> doc_), sizeof(node -> doc_));
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.write((char *) &(node -> docs_[idx]), sizeof(node -> docs_[idx]));
    }

    // Save the child Pointers
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.write((char *) &(node -> childPointers_[idx]), sizeof(node -> childPointers_[idx]));
    }

    // Save the bitmaps
    for (uint32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
        filePointer_.write((char *) &(node -> nodeBitmap_[bitmapIdx]), sizeof(node -> nodeBitmap_[bitmapIdx]));
    }

    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        for (uint32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
            filePointer_.write((char *) &(node -> bitmap_[idx][bitmapIdx]), sizeof(node -> bitmap_[idx][bitmapIdx]));
        }
    }

    // Save the events
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        // Save list size 
        size_t listSize = node -> childEvents_[idx].size();
        filePointer_.write((char *) &(listSize), sizeof(listSize));

        // Save list
        for (list<Event>::iterator it = node -> childEvents_[idx].begin(); it != node -> childEvents_[idx].end(); it++) {
            filePointer_.write((char *) &(*it), sizeof(*it));
        }
    }

    filePointer_.flush();
}


// Read the node from the file
TreeNode* RTree::readNode(uint32_t page) {
    // Place the read pointer in the appropriate position
    filePointer_.seekg(page * pageSize_);

    // Get the type of the node
    bool leaf;
    filePointer_.read((char *) &leaf, sizeof(leaf));

    // Create the node
    TreeNode* node;
    if (leaf) {
        node = new LeafNode(maxEntries_, minEntries_, bitmapSize_);
    }
    else {
        node = new TreeNode(maxEntries_, minEntries_, bitmapSize_);
    }

    // Retrieve the relevant info
    filePointer_.read((char *) &(node -> currEntries_), sizeof(node -> currEntries_));
    

    // Retrieve MBRs
    filePointer_.read((char *) &(node -> currentMBR_), sizeof(node -> currentMBR_));
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.read((char *) &(node -> MBR_[idx]), sizeof(node -> MBR_[idx]));
    }

    // Retrieve Docs
    filePointer_.read((char *) &(node -> doc_), sizeof(node -> doc_));
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.read((char *) &(node -> docs_[idx]), sizeof(node -> docs_[idx]));
    }   

    // Retrieve pointers
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.read((char *) &(node -> childPointers_[idx]), sizeof(node -> childPointers_[idx]));
    }


    // Retrieve bitmaps
    for (uint32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
        filePointer_.read((char *) &(node -> nodeBitmap_[bitmapIdx]), sizeof(node -> nodeBitmap_[bitmapIdx]));
    }

    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        for (uint32_t bitmapIdx = 0; bitmapIdx < node -> bitmapSize_; bitmapIdx++) {
            filePointer_.read((char *) &(node -> bitmap_[idx][bitmapIdx]), sizeof(node -> bitmap_[idx][bitmapIdx]));
        }
    }

    // Retrieve events
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        
        // Read List size
        uint32_t size;
        filePointer_.read((char *) &(size), sizeof(size));

        // Read List
        Event value;
        for (uint32_t i = 0; i < size; i++) {
            filePointer_.read((char *) &(value), sizeof(value));
            node -> childEvents_[idx].push_back(value);

            // Get iterator to that event
            list<Event>::iterator it = node -> childEvents_[idx].end();
            advance(it, -1);

            // Loop over all time slots
            for (uint32_t slotIdx = 0; slotIdx < TIMESLOTS; slotIdx++) {
                
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
    filePointer_.seekp(0);

    // Save the relevant info
    filePointer_.write((char *) &rootPageOffset_, sizeof(rootPageOffset_));
    
    filePointer_.write((char *) &treeSize_, sizeof(treeSize_));

    filePointer_.write((char *) &rootID_, sizeof(rootID_));

    filePointer_.write((char *) &pageSize_, sizeof(pageSize_));

    filePointer_.write((char *) &maxEntries_, sizeof(maxEntries_));

    filePointer_.write((char *) &minEntries_, sizeof(minEntries_));

    filePointer_.write((char *) &bitmapSize_, sizeof(bitmapSize_));   

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