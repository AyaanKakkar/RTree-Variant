#ifndef _TREENODE
#define _TREENODE
#define TIMESLOTS 24
#include <cstdint>
#include <list>
#include <vector>
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
        int32_t nodeID_; // ID of the node (Used to access it on disk)
        int32_t maxEntries_; // Max Number of entries in the node
        int32_t minEntries_; // Min number of entries in the node
        int32_t currEntries_; // Number of current entries
        int32_t bitmapSize_; // Size of bitmap array;
        
        // Child Pointers
        int32_t* childPointers_; // Child Pointers (Stores the ID of each child)
        
        // MBRs
        Rectangle currentMBR_; // MBR of the current node
        Rectangle* MBR_; // MBR of each child
        
        // Bitmaps
        int32_t** bitmap_; // Bitmap corresponding to each child
        int32_t* nodeBitmap_; // Bitmap corresponding to the entire node

        // Event Data
        list<Event>* childEvents_; // Child Events Linked List
        list<list<Event>::iterator> ** childTimeSlots_; // Time Slots Hash Buckets for each child

        // Document Data
        int32_t doc_; // Document of the node
        int32_t* docs_; // Document of each child


        // Add Entry to the node
        void addEntry(Rectangle MBR, int32_t* bitmap, list<Event>* events, int32_t eventsListCount, int32_t doc, int32_t pointer);
        
        // Entry addition helper functions
        void addBitmap(int32_t* bitmap);
        void addBitmapToChild(int32_t* bitmap, int32_t childIdx);
        void addEventsToChild(Event* events, int32_t eventsCount, int32_t childIdx);


        // Doc creation
        void createDoc(RTree* rTree);
        
        // Split node
        int32_t split(RTree *rTree);

        // Split helper functions
        TreeNode* chooseSplitNode(int32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, int32_t remainingEntries);
        virtual TreeNode* createSplitNode();
        void addEntryToSplitNode(int32_t entryIdx, TreeNode* splitNode);
        
        // Copy node
        void copyNodeContent(TreeNode* node);
        void copyNodeBitmap(TreeNode* node);
        void copyNodeEvents(TreeNode* node);

        // Child update functions
        void updateChildBitmap(int32_t* bitmap, int32_t childIdx);
        void updateChildEvents(list<Event>* events, int32_t eventsListCount, int32_t childIdx);

        // Destory functions
        void freeBitmap();
        void freeEvents();

    public:

        // Constructor and desctructor
        TreeNode(int32_t maxEntries, int32_t minEntries, int32_t bitmapSize);
        virtual ~TreeNode();
        
        // Insertion function
        virtual int32_t insert(Rectangle MBR, int32_t* bitmap, int32_t pointer, int32_t doc, Event* events, int32_t eventsCount, RTree* rTree);
        
        // Print function
        virtual void printTree(RTree* rTree);

        // Query
        virtual void query(Rectangle MBR, vector<int32_t>& a, RTree* rTree);

    friend class RTree;
};

#endif