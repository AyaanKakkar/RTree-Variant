#ifndef _RTREE
#define _RTREE

#include <string>
#include <fstream>
#include "rectangle.h"
#include "treeNode.h"
#include "leafNode.h"

using namespace std;

class RTree {

    /*
        Used to declare the R-Tree
    */
    
    private:
        
        RTree(){} // Private constructor to restrict access

        uint32_t rootPageOffset_; // Number of initial pages reserved for tree metadata

        uint32_t treeSize_; // Number of nodes in the tree

        uint32_t rootID_; // ID of the root node
        
        uint32_t pageSize_; // Page Size
        
        uint32_t maxEntries_; // Max Entries in the node
        
        uint32_t minEntries_; // Min Entries in the node
        
        uint32_t dataSize_; // Size of the data array
        
        TreeNode* rootNode_; // Pointer to the root node

        fstream filePointer_;

        TreeNode* getNode(uint32_t id); 
        
        void saveNode(TreeNode* node);  // INCREASES TREE SIZE
        
        void writeNode(TreeNode* node, uint32_t page);
        
        TreeNode* readNode(uint32_t page);
        
        void saveTree();
    
    public:
        
        static RTree* LoadIndex(string fileName);
        
        static RTree* createIndex(string fileName, uint32_t pageSize, uint32_t maxEntries, uint32_t minEntries, uint32_t dataSize);
        
        void insert(Rectangle MBR, uint32_t* data, uint32_t pointer);

        ~RTree();

    friend class TreeNode;
    friend class LeafNode;
};

#endif