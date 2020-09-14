default: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp test.cpp
	g++ -g -o main leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp test.cpp -I .
clean: main
	rm main
