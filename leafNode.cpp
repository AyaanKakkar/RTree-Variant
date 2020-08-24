#include "leafNode.h"

using namespace std;

LeafNode::LeafNode(uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize) : TreeNode(maxEntries, minEntries, dataSize) {

}

// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
uint32_t LeafNode::insert(Rectangle MBR, uint32_t* data, uint32_t pointer, RTree* rTree) {
    
    addEntry(MBR, data, pointer);

    // Perform splitting if necessary
    if (currEntries_ > maxEntries_) {
        return split(rTree);
    }

    // Save changes to the disk
    rTree -> saveNode(this);

    return -1;
}


// Create a new split node
TreeNode* LeafNode::createSplitNode() {
    return new LeafNode(maxEntries_, minEntries_, dataSize_);
}

