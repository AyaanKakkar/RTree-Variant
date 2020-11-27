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
        void addLeafEntry(Rectangle MBR, int32_t* bitmap, int32_t pointer, Event* events, int32_t eventsCount, int32_t doc);
    
    public:
        // Constructor
        LeafNode(int32_t maxEntries, int32_t minEntries, int32_t bitmapSize);

        // Override functions
        virtual int32_t insert(Rectangle MBR, int32_t* bitmap, int32_t pointer, int32_t doc, Event* events, int32_t eventsCount, RTree* rTree);
        virtual void printTree(RTree* rTree);

    friend class RTree;
};

#endif