#ifndef _TREENODE
#define _TREENODE

#include <cstdint>
#include "rectangle.h"

using namespace std;

class RTree;

class TreeNode {

    /*
        Used to declare a node in the R-Tree
    */

    protected: 
        uint32_t nodeID_; // ID of the node (Used to access it on disk)
        uint32_t maxEntries_; // Max Number of entries in the node
        uint32_t minEntries_; // Min number of entries in the node
        uint32_t currEntries_; // Number of current entries
        uint32_t dataSize_; // Size of data array;
        
        Rectangle currentMBR_; // MBR of the current node
        
        Rectangle* MBR_; // MBR of each child
        
        uint32_t* childPointers_; // Child Pointers (Stores the ID of each child)

        uint32_t** data_; // Data corresponding to each child

        uint32_t* nodeData_;

        void addEntry(Rectangle MBR, uint32_t* data, uint32_t pointer);
        
        uint32_t split(RTree *rTree);
        
        TreeNode* chooseSplitNode(uint32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, uint32_t remainingEntries);
        
        virtual TreeNode* createSplitNode();
        
        void addEntryToSplitNode(uint32_t entryIdx, TreeNode* splitNode);
        
        void copyNodeContent(TreeNode* node);

        void addData(uint32_t* data);

        void updateChildData(uint32_t* data, uint32_t childIdx);

        void copyNodeData(TreeNode* node);

        void freeData();


    public:
        TreeNode(uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize);
        virtual ~TreeNode();
        virtual uint32_t insert(Rectangle MBR, uint32_t* data, uint32_t pointer, RTree* rTree);
        virtual void printTree(RTree* rTree);

    friend class RTree;

};

#endif