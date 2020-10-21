#include "rtree.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    

    RTree* rtree = RTree::LoadIndex("testFile.bin");

    rtree -> printTree();

    return 0;
}