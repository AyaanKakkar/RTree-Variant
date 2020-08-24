#ifndef _LEAFNODE
#define _LEAFNODE

#include "treeNode.h"

using namespace std;

class LeafNode : public TreeNode {
    
    /*
        Used to declare a leaf node in the R-Tree
    */
    
    private:
        TreeNode* createSplitNode();
    
    public:
        LeafNode(uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize);
        uint32_t insert(Rectangle MBR, uint32_t* data, uint32_t pointer, RTree* rTree);

    friend class RTree;
};

#endif