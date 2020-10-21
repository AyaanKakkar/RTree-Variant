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
        uint32_t nodeID_; // ID of the node (Used to access it on disk)
        uint32_t maxEntries_; // Max Number of entries in the node
        uint32_t minEntries_; // Min number of entries in the node
        uint32_t currEntries_; // Number of current entries
        uint32_t bitmapSize_; // Size of bitmap array;
        
        Rectangle currentMBR_; // MBR of the current node
        
        Rectangle* MBR_; // MBR of each child
        
        uint32_t* childPointers_; // Child Pointers (Stores the ID of each child)

        uint32_t** bitmap_; // Bitmap corresponding to each child

        uint32_t* nodeBitmap_; // Bitmap corresponding to the entire node

        list<Event>* childEvents_; // Child Events Linked List

        list<list<Event>::iterator> ** childTimeSlots_; // Time Slots Hash Buckets for each child

        void addEntry(Rectangle MBR, uint32_t* bitmap, list<Event>* events, uint32_t eventsListCount, uint32_t pointer);
        
        uint32_t split(RTree *rTree);
        
        TreeNode* chooseSplitNode(uint32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, uint32_t remainingEntries);
        
        virtual TreeNode* createSplitNode();
        
        void addEntryToSplitNode(uint32_t entryIdx, TreeNode* splitNode);
        
        void copyNodeContent(TreeNode* node);

        void addBitmap(uint32_t* bitmap);

        void addBitmapToChild(uint32_t* bitmap, uint32_t childIdx);

        void addEventsToChild(Event* events, uint32_t eventsCount, uint32_t childIdx);

        void updateChildBitmap(uint32_t* bitmap, uint32_t childIdx);

        void updateChildEvents(list<Event>* events, uint32_t eventsListCount, uint32_t childIdx);

        void copyNodeBitmap(TreeNode* node);

        void copyNodeEvents(TreeNode* node);

        void freeBitmap();

        void freeEvents();


    public:
        TreeNode(uint32_t maxEntries, uint32_t minEntries, uint32_t bitmapSize);
        virtual ~TreeNode();
        virtual uint32_t insert(Rectangle MBR, uint32_t* bitmap, uint32_t pointer, Event* events, uint32_t eventsCount, RTree* rTree);
        virtual void printTree(RTree* rTree);

    friend class RTree;

};

#endif