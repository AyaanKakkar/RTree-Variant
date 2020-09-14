#include "treeNode.h"
#include "rtree.h"
#include <iostream>

using namespace std;

// Constructor
TreeNode::TreeNode(uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize) {
    
    nodeID_ = -1;
    maxEntries_ = maxEntries;
    minEntries_ = minEntries;
    dataSize_ = dataSize_;

    currEntries_ = 0;
    
    currentMBR_ = Rectangle();
    
    MBR_ = new Rectangle[maxEntries_ + 1];
    
    childPointers_ = new uint32_t[maxEntries_ + 1];
    

    nodeData_ = new uint32_t[dataSize_]; 
    for (uint32_t idx = 0; idx < dataSize_; idx++) {
        nodeData_[idx] = 0;
    }

    data_ = new uint32_t*[maxEntries_ + 1];
    for (uint32_t idx = 0; idx < maxEntries_ + 1; idx++) {
        data_[idx] = new uint32_t[dataSize_];
        for (uint32_t dataIdx = 0; dataIdx < dataSize_; dataIdx++) {
            data_[idx][dataIdx] = 0;
        }
    }
}


// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
uint32_t TreeNode::insert(Rectangle MBR, uint32_t* data, uint32_t pointer, RTree* rTree) {
    
    // Update the MBR
    currentMBR_ = Rectangle::combine(currentMBR_, MBR);

    // Add the data
    addData(data);

    // Calculate the index of the child to traverse to
    uint32_t childIdx = -1;
    long double minAreaEnlargement = 0;
    long double childArea = 0;

    for (uint32_t index = 0; index < currEntries_; index++) {

        long double areaEnlargment = MBR_[index].getAreaEnlargement(MBR);
        long double area = MBR_[index].getArea();
        
        if (childIdx == -1 || areaEnlargment < minAreaEnlargement 
        || areaEnlargment == minAreaEnlargement && childArea > area) {
            childIdx = index;
            minAreaEnlargement = areaEnlargment;
            childArea = area;
        }
    } 

    // Get that child node
    TreeNode* childNode = rTree -> getNode(childPointers_[childIdx]);

    // Insert into that node
    uint32_t newNodeID = childNode -> insert(MBR, data, pointer, rTree);

    // Update that node's MBR
    MBR_[childIdx] = childNode -> currentMBR_;
    
    // Update that node's data
    updateChildData(childNode -> nodeData_, childIdx);

    // Free the memory
    delete childNode;

    // Perform addition if necessary
    if (newNodeID != -1) {
        // Get the new node
        TreeNode* newNode = rTree -> getNode(newNodeID);
        addEntry(newNode -> currentMBR_, newNode -> nodeData_, newNodeID);   
        
        // Free the memory
        delete newNode;

        // Perform splitting if necessary and return the new noe formed
        if (currEntries_ > maxEntries_) {
            return split(rTree);
        }
    }

    // Save changes to the disk
    rTree -> saveNode(this);

    return -1;
}

// Add an entry in the node
void TreeNode::addEntry(Rectangle MBR, uint32_t* data, uint32_t pointer) {

    MBR_[currEntries_] = MBR;
    
    if (currEntries_ == 0) {
        currentMBR_ = MBR_[currEntries_];
    }
    else {
        currentMBR_ = Rectangle::combine(currentMBR_, MBR_[currEntries_]);
    }

    updateChildData(data, currEntries_);

    childPointers_[currEntries_] = pointer;
    currEntries_++;
}

// Split the node and return the ID of the new node
uint32_t TreeNode::split(RTree* rTree) {
    
    // Using linear splitting;
    
    uint32_t index1 = 0;
    uint32_t index2 = 1;
    long double maxDistance = Rectangle::distance(MBR_[index1], MBR_[index2]);
    
    // Loop over all possilbe pairs and find the pair with the largest distance 
    for (uint32_t i = 0; i < currEntries_; i++) {
        for (uint32_t j = i + 1; j < currEntries_; j++) {
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

    uint32_t remainingEntries = currEntries_ - 2;

    // Iterate over all remaining entries
    for (uint32_t idx = 0; idx < currEntries_; idx++) {
        if (idx != index1 && idx != index2) {
            // Choose a node, and add the entry to that node
            addEntryToSplitNode(idx, chooseSplitNode(idx, splitNode1, splitNode2, remainingEntries));
            remainingEntries--;
        }
    }

    // Transfer the contents of one node to the current node
    copyNodeContent(splitNode1);

    // Save the changes to the disk
    rTree -> saveNode(splitNode2);  
    rTree -> saveNode(this);

    // Return ID
    uint32_t returnID = splitNode2 -> nodeID_;

    // Free the memory
    delete splitNode1;
    delete splitNode2;

    // Return the newly creater node's ID
    return returnID;

}

// Helper function to choose node to add entry to
TreeNode* TreeNode::chooseSplitNode(uint32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, uint32_t remainingEntries) {
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
    return new TreeNode(maxEntries_, minEntries_, dataSize_);
}

// Add entry to a split node
void TreeNode::addEntryToSplitNode(uint32_t entryIdx, TreeNode* splitNode) {
    splitNode->addEntry(MBR_[entryIdx], data_[entryIdx], childPointers_[entryIdx]);
}

// Copy content from a node
void TreeNode::copyNodeContent(TreeNode* node) {
    currEntries_ = node -> currEntries_;
    currentMBR_ = node -> currentMBR_;
    for (uint32_t idx = 0; idx < currEntries_; idx++) {
        MBR_[idx] = node -> MBR_[idx];
        childPointers_[idx] = node -> childPointers_[idx];
    }
    copyNodeData(node);
}

// Destructor
TreeNode::~TreeNode() {
    delete MBR_;
    delete childPointers_;
    freeData();
}

// Add Data to the current node
void TreeNode::addData(uint32_t* data) {
    for (uint32_t idx = 0; idx < dataSize_; idx++) {
        nodeData_[idx] += data[idx];
    }
}

// Change the data of a child
void TreeNode::updateChildData(uint32_t* data, uint32_t childIdx) {
    // Update node data
    for (uint32_t idx = 0; idx < dataSize_; idx++) {
        nodeData_[idx] += (data[idx] - data_[childIdx][idx]);
    }

    // Update child data 
    for (uint32_t idx = 0; idx < dataSize_; idx++) {
        data_[childIdx][idx] = data[idx];
    }
}

// Copy the data of a node
void TreeNode::copyNodeData(TreeNode* node) {
    // Update node data
    for (uint32_t idx = 0; idx < dataSize_; idx++) {
        nodeData_[idx] = node -> nodeData_[idx];
    }

    // Update child data 
    for (uint32_t entryIdx = 0; entryIdx < node -> currEntries_; entryIdx++) {
        for (uint32_t idx = 0; idx < dataSize_; idx++) {
            data_[entryIdx][idx] = node -> data_[entryIdx][idx];
        }
    }
}

// Print the tree
void TreeNode::printTree(RTree* rTree) {
    cout << "Node ID : " << nodeID_ << endl;
    cout << "Current Entries : " << currEntries_ << endl;
    cout << "Child Pointers : ";
    for (int idx = 0; idx < currEntries_; idx++) {
        cout << childPointers_[idx];
        if (idx < currEntries_ - 1) {
            cout << ", ";
        }
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

// Free the data
void TreeNode::freeData() {
    
    delete nodeData_;
    
    for (uint32_t idx = 0; idx < maxEntries_ + 1; idx++) {
        delete data_[idx];
    }

    delete data_;
}
