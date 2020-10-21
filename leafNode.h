#ifndef _LEAFNODE
#define _LEAFNODE
#include "treeNode.h"

using namespace std;


class LeafNode : public TreeNode {
    
    /*
        Used to declare a leaf node in the R-Tree
    */
    
    private:
        virtual TreeNode* createSplitNode();
        void addLeafEntry(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, Event* events, uint32_t eventsCount);
    
    public:
        LeafNode(uint32_t maxEntries, uint32_t minEntries, uint32_t bitmapSize);
        virtual uint32_t insert(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, Event* events, uint32_t eventsCount, RTree* rTree);
        virtual void printTree(RTree* rTree);

    friend class RTree;
};

#endif