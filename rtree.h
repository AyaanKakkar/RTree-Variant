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
        int32_t rootPageOffset_; // Number of initial pages reserved for tree metadata
        int32_t treeSize_; // Number of nodes in the tree
        int32_t rootID_; // ID of the root node
        int32_t pageSize_; // Page Size
        int32_t maxEntries_; // Max Entries in the node
        int32_t minEntries_; // Min Entries in the node
        int32_t bitmapSize_; // Size of the bitmap
        int32_t nextPage_; // Next available page

        // Root node
        TreeNode* rootNode_; // Pointer to the root node

        // File Pointer
        fstream filePointer_;
        
        // Get node from disk
        TreeNode* getNode(int32_t id); 
        TreeNode* readNode(int32_t page);
        
        // Save node to disk
        void saveNode(TreeNode* node);  // INCREASES TREE SIZE
        void writeNode(TreeNode* node);
        int32_t getNodeData(TreeNode* node, char* &a); // Helper function
        int32_t readNodeData(int32_t page, char* &a); // Helper function
        
        // Save tree
        void saveTree();
    
    public:
        // Create and retrieve index
        static RTree* LoadIndex(string fileName);
        static RTree* CreateIndex(string fileName, int32_t pageSize, int32_t maxEntries, int32_t minEntries, int32_t bitmapSize);
        
        // Insertion function
        void insert(Rectangle MBR, int32_t* bitmap, int32_t pointer, int32_t doc, Event* events = nullptr, int32_t eventsCount = 0);

        // Print tree for debugging
        void printTree();

        // Destructor
        ~RTree();

        // Maintain count of nodes accessed
        int32_t nodesAccessed_;

        // Maintain a count of disk IOs
        int32_t diskIO_;

    friend class TreeNode;
    friend class LeafNode;
};

#endif