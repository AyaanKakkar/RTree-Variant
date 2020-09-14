#include <typeinfo>
#include <iostream>
#include "rtree.h"

using namespace std;

// Create a RTree with the given paramters
RTree* RTree::createIndex(string fileName, uint32_t pageSize, uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize) {
    // Create a new RTree
    RTree* rTree = new RTree();
    
    // Set parameters
    rTree -> rootPageOffset_ = 1;
    rTree -> pageSize_ = pageSize;
    rTree -> maxEntries_ = maxEntries;
    rTree -> minEntries_ = minEntries;
    rTree -> dataSize_ = dataSize;
    rTree -> treeSize_ = 0;
    rTree -> rootID_ = -1;
    rTree -> rootNode_ = nullptr;
    
    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out | ios::trunc);

    // Save the tree
    rTree -> saveTree();

    return rTree;
}

// Insert an entry into the RTree
void RTree::insert(Rectangle MBR, uint32_t* data, uint32_t pointer) {
    if (treeSize_ == 0) {
        // If it's an empty tree then create a root node
        this -> rootNode_ = new LeafNode(maxEntries_, minEntries_, dataSize_);
    }
    
    // Insert into the root
    uint32_t newNodeID = this -> rootNode_ -> insert(MBR, data, pointer, this);

    if (newNodeID != -1) {
        // Get the new node
        TreeNode* newNode = this -> getNode(newNodeID);

        // Create a new root
        TreeNode* newRoot = new TreeNode(maxEntries_, minEntries_, dataSize_);

        // Add the new node
        newRoot -> addEntry(newNode -> currentMBR_, newNode -> nodeData_, newNodeID); 

        // Add the current root
        newRoot -> addEntry(this -> rootNode_ -> currentMBR_, this -> rootNode_ -> nodeData_, this -> rootNode_ -> nodeID_);

        // Free the memory
        delete this -> rootNode_;

        // Change the root
        this -> rootNode_ = newRoot;

        // Save the root
        saveNode(this -> rootNode_);
    }

    // Update root ID if necessary
    if (this -> rootID_ != this -> rootNode_ -> nodeID_) {
        this -> rootID_ = this -> rootNode_ -> nodeID_;
        this -> saveTree();
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
}

// Retrieve a node using ID
TreeNode* RTree::getNode(uint32_t id) {
    TreeNode* node = this -> readNode(id + rootPageOffset_);
    node -> nodeID_ = id;
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

    // Save the relevant info
    filePointer_.write((char *) &(node -> currEntries_), sizeof(node -> currEntries_));
    
    filePointer_.write((char *) &(node -> currentMBR_), sizeof(node -> currentMBR_));
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.write((char *) &(node -> MBR_[idx]), sizeof(node -> MBR_[idx]));
    }

    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.write((char *) &(node -> childPointers_[idx]), sizeof(node -> childPointers_[idx]));
    }

    for (uint32_t dataIdx = 0; dataIdx < node -> dataSize_; dataIdx++) {
        filePointer_.write((char *) &(node -> nodeData_[dataIdx]), sizeof(node -> nodeData_[dataIdx]));
    }

    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        for (uint32_t dataIdx = 0; dataIdx < node -> dataSize_; dataIdx++) {
            filePointer_.write((char *) &(node -> data_[idx][dataIdx]), sizeof(node -> data_[idx][dataIdx]));
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
        node = new LeafNode(maxEntries_, minEntries_, dataSize_);
    }
    else {
        node = new TreeNode(maxEntries_, minEntries_, dataSize_);
    }

    // Retrieve the relevant info
    filePointer_.read((char *) &(node -> currEntries_), sizeof(node -> currEntries_));
    
    filePointer_.read((char *) &(node -> currentMBR_), sizeof(node -> currentMBR_));
    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.read((char *) &(node -> MBR_[idx]), sizeof(node -> MBR_[idx]));
    }

    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        filePointer_.read((char *) &(node -> childPointers_[idx]), sizeof(node -> childPointers_[idx]));
    }

    for (uint32_t dataIdx = 0; dataIdx < node -> dataSize_; dataIdx++) {
        filePointer_.read((char *) &(node -> nodeData_[dataIdx]), sizeof(node -> nodeData_[dataIdx]));
    }

    for (uint32_t idx = 0; idx < node -> currEntries_; idx++) {
        for (uint32_t dataIdx = 0; dataIdx < node -> dataSize_; dataIdx++) {
            filePointer_.read((char *) &(node -> data_[idx][dataIdx]), sizeof(node -> data_[idx][dataIdx]));
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

    filePointer_.write((char *) &dataSize_, sizeof(dataSize_));   

    filePointer_.flush();
    
}


// Load an existing RTree from the given file
RTree* RTree::LoadIndex(string fileName) {
    // Create a RTree
    RTree* rTree = new RTree();

    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out);

    // Read the relevant info
    rTree -> filePointer_.write((char *) &(rTree -> rootPageOffset_), sizeof(rTree -> rootPageOffset_));
    
    rTree -> filePointer_.write((char *) &(rTree -> treeSize_), sizeof(rTree -> treeSize_));

    rTree -> filePointer_.write((char *) &(rTree -> rootID_), sizeof(rTree -> rootID_));

    rTree -> filePointer_.write((char *) &(rTree -> pageSize_), sizeof(rTree -> pageSize_));

    rTree -> filePointer_.write((char *) &(rTree -> maxEntries_), sizeof(rTree -> maxEntries_));

    rTree -> filePointer_.write((char *) &(rTree -> minEntries_), sizeof(rTree -> minEntries_));

    rTree -> filePointer_.write((char *) &(rTree -> dataSize_), sizeof(rTree -> dataSize_));  

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