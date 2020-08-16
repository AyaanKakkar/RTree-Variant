#include "leafNode.h"

LeafNode::LeafNode(uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize) : TreeNode(maxEntries, minEntries, dataSize) {
    id_ = new uint32_t[maxEntries + 1];
}

// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
uint32_t LeafNode::insert(Rectangle MBR, uint32_t* data, uint32_t pointer, uint32_t id, RTree* rTree) {
    
    addEntry(MBR, data, pointer, id);

    // Perform splitting if necessary
    if (currEntries_ > maxEntries_) {
        return split(rTree);
    }

    // Save changes to the disk
    rTree -> saveNode(this);

    return -1;
}

// Add an entry in the leaf node
void LeafNode::addEntry(Rectangle MBR, uint32_t* data, uint32_t pointer, uint32_t id) {
    MBR_[currEntries_] = MBR;
    
    if (currEntries_ == 0) {
        currentMBR_ = MBR_[currEntries_];
    }
    else {
        currentMBR_ = Rectangle::combine(currentMBR_, MBR_[currEntries_]);
    }
    
    childPointers_[currEntries_] = pointer;
    id_[currEntries_] = id;

    updateChildData(data, currEntries_);

    currEntries_++;
}

// Create a new split node
TreeNode* LeafNode::createSplitNode() {
    return new LeafNode(maxEntries_, minEntries_, dataSize_);
}

// Add entry to a split node
void LeafNode::addEntryToSplitNode(uint32_t entryIdx, TreeNode* splitNode) {
    ((LeafNode*) splitNode) -> addEntry(MBR_[entryIdx], data_[entryIdx], childPointers_[entryIdx], id_[entryIdx]);
}

// Copy content from a node
void LeafNode::copyNodeContent(TreeNode* node) {
    TreeNode::copyNodeContent(node);
    for (uint32_t idx = 0; idx < currEntries_; idx++) {
        id_[idx] = ((LeafNode*) node) -> id_[idx];
    }
}

// Destructor
LeafNode::~LeafNode() {
    delete id_;
}