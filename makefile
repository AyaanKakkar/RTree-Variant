default: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp test.cpp event.cpp
	g++ -g -o save leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp test.cpp -I .
load: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp testLoad.cpp
	g++ -g -o load leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp testLoad.cpp -I .
clean: save
	rm save
cleanLoad: load
	rm load
