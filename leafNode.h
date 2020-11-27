#ifndef _LEAFNODE
#define _LEAFNODE
#include "treeNode.h"

using namespace std;


class LeafNode : public TreeNode {
    
    /*
        Used to declare a leaf node in the R-Tree
    */
    
    private:

        // Override functions
        virtual TreeNode* createSplitNode();
        void addLeafEntry(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, Event* events, uint32_t eventsCount, uint32_t doc);
    
    public:
        // Constructor
        LeafNode(uint32_t maxEntries, uint32_t minEntries, uint32_t bitmapSize);

        // Override functions
        virtual uint32_t insert(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, uint32_t doc, Event* events, uint32_t eventsCount, RTree* rTree);
        virtual void printTree(RTree* rTree);

    friend class RTree;
};

#endif