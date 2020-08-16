#ifndef _LEAFNODE
#define _LEAFNODE

#include "treeNode.h"

using namespace std;

class LeafNode : public TreeNode {
    
    /*
        Used to declare a leaf node in the R-Tree
    */
    
    private:
        uint32_t* id_; // Store ID of each entry. Can be used to store other data
        
        void addEntry(Rectangle MBR, uint32_t* data, uint32_t pointer, uint32_t id);
        
        TreeNode* createSplitNode();
        
        void addEntryToSplitNode(uint32_t entryIdx, TreeNode* splitNode);
        
        void copyNodeContent(TreeNode* node);
    
    public:
        LeafNode(uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize);
        ~LeafNode();
        uint32_t insert(Rectangle MBR, uint32_t* data, uint32_t pointer, uint32_t id, RTree* rTree);
};

#endif