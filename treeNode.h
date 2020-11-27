#ifndef _TREENODE
#define _TREENODE
#define TIMESLOTS 24
#include <cstdint>
#include <list>
#include "rectangle.h"
#include "event.h"

using namespace std;

class RTree;

class TreeNode {

    /*
        Used to declare a node in the R-Tree
    */

    protected: 

        // Basic Node Data
        uint32_t nodeID_; // ID of the node (Used to access it on disk)
        uint32_t maxEntries_; // Max Number of entries in the node
        uint32_t minEntries_; // Min number of entries in the node
        uint32_t currEntries_; // Number of current entries
        uint32_t bitmapSize_; // Size of bitmap array;
        
        // Child Pointers
        uint32_t* childPointers_; // Child Pointers (Stores the ID of each child)
        
        // MBRs
        Rectangle currentMBR_; // MBR of the current node
        Rectangle* MBR_; // MBR of each child
        
        // Bitmaps
        uint32_t** bitmap_; // Bitmap corresponding to each child
        uint32_t* nodeBitmap_; // Bitmap corresponding to the entire node

        // Event Data
        list<Event>* childEvents_; // Child Events Linked List
        list<list<Event>::iterator> ** childTimeSlots_; // Time Slots Hash Buckets for each child

        // Document Data
        uint32_t doc_; // Document of the node
        uint32_t* docs_; // Document of each child


        // Add Entry to the node
        void addEntry(Rectangle MBR, uint32_t* bitmap, list<Event>* events, uint32_t eventsListCount, uint32_t doc, uint32_t pointer);
        
        // Entry addition helper functions
        void addBitmap(uint32_t* bitmap);
        void addBitmapToChild(uint32_t* bitmap, uint32_t childIdx);
        void addEventsToChild(Event* events, uint32_t eventsCount, uint32_t childIdx);


        // Doc creation
        void createDoc();
        
        // Split node
        uint32_t split(RTree *rTree);

        // Split helper functions
        TreeNode* chooseSplitNode(uint32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, uint32_t remainingEntries);
        virtual TreeNode* createSplitNode();
        void addEntryToSplitNode(uint32_t entryIdx, TreeNode* splitNode);
        
        // Copy node
        void copyNodeContent(TreeNode* node);
        void copyNodeBitmap(TreeNode* node);
        void copyNodeEvents(TreeNode* node);

        // Child update functions
        void updateChildBitmap(uint32_t* bitmap, uint32_t childIdx);
        void updateChildEvents(list<Event>* events, uint32_t eventsListCount, uint32_t childIdx);

        // Destory functions
        void freeBitmap();
        void freeEvents();

    public:

        // Constructor and desctructor
        TreeNode(uint32_t maxEntries, uint32_t minEntries, uint32_t bitmapSize);
        virtual ~TreeNode();
        
        // Insertion function
        virtual uint32_t insert(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, uint32_t doc, Event* events, uint32_t eventsCount, RTree* rTree);
        
        // Print function
        virtual void printTree(RTree* rTree);

    friend class RTree;
};

#endif