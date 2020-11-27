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

        // Metadata
        uint32_t rootPageOffset_; // Number of initial pages reserved for tree metadata
        uint32_t treeSize_; // Number of nodes in the tree
        uint32_t rootID_; // ID of the root node
        uint32_t pageSize_; // Page Size
        uint32_t maxEntries_; // Max Entries in the node
        uint32_t minEntries_; // Min Entries in the node
        uint32_t bitmapSize_; // Size of the bitmap

        // Root node
        TreeNode* rootNode_; // Pointer to the root node

        // File Pointer
        fstream filePointer_;
        
        // Get node from disk
        TreeNode* getNode(uint32_t id); 
        TreeNode* readNode(uint32_t page);
        
        // Save node to disk
        void saveNode(TreeNode* node);  // INCREASES TREE SIZE
        void writeNode(TreeNode* node, uint32_t page);
        
        // Save tree
        void saveTree();
    
    public:
        // Create and retrieve index
        static RTree* LoadIndex(string fileName);
        static RTree* createIndex(string fileName, uint32_t pageSize, uint32_t maxEntries, uint32_t minEntries, uint32_t bitmapSize);
        
        // Insertion function
        void insert(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, uint32_t doc, Event* events = nullptr, uint32_t eventsCount = 0);

        // Print tree for debugging
        void printTree();

        // Destructor
        ~RTree();

        // Maintain count of nodes accessed
        uint32_t nodesAccessed_;

        // Maintain a count of disk IOs
        uint32_t diskIO_;

    friend class TreeNode;
    friend class LeafNode;
};

#endif