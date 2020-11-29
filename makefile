default: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment0.cpp event.cpp generate.cpp
	g++ -g -o exp0 leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment0.cpp generate.cpp -I .
exp2: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment2.cpp event.cpp generate.cpp
	g++ -g -o exp2 leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment2.cpp generate.cpp -I .
exp1: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment1.cpp event.cpp generate.cpp
	g++ -g -o exp1 leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment1.cpp generate.cpp -I .
load: leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp testLoad.cpp
	g++ -g -o load leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp testLoad.cpp -I .
clean: exp0 exp2 exp1
	rm exp0 exp2
cleanLoad: load
	rm load
