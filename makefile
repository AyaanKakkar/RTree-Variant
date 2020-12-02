default: leafNode.cpp saveGraph.cpp load.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment0.cpp event.cpp generate.cpp
	g++ -g -o exp0 saveGraph.cpp load.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment0.cpp generate.cpp -I .
exp2: leafNode.cpp load.cpp saveGraph.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment2.cpp event.cpp generate.cpp
	g++ -g -o exp2 load.cpp saveGraph.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment2.cpp generate.cpp -I .
exp1: leafNode.cpp rectangle.cpp load.cpp saveGraph.cpp rtree.cpp treeNode.cpp experiment1.cpp event.cpp generate.cpp
	g++ -g -o exp1 load.cpp saveGraph.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment1.cpp generate.cpp -I .
load: leafNode.cpp rectangle.cpp rtree.cpp saveGraph.cpp treeNode.cpp testLoad.cpp
	g++ -g -o load leafNode.cpp saveGraph.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp testLoad.cpp -I .
clean: exp0 exp2 exp1
	rm exp0 exp2
cleanLoad: load
	rm load
