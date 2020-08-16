#ifndef _RTREE
#define _RTREE

#include "treeNode.h"
#include "leafNode.h"

class RTree {
    private:
        TreeNode* getNode(uint32_t page);
        void saveNode(TreeNode* node);

    friend class TreeNode;
    friend class LeafNode;
};

#endif