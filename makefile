build0: leafNode.cpp saveGraph.cpp load.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment0.cpp event.cpp generate.cpp
	g++ -g -o exp0 saveGraph.cpp load.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment0.cpp generate.cpp -I .
build2: leafNode.cpp load.cpp saveGraph.cpp rectangle.cpp rtree.cpp treeNode.cpp experiment2.cpp event.cpp generate.cpp
	g++ -g -o exp2 load.cpp saveGraph.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment2.cpp generate.cpp -I .
build1: leafNode.cpp rectangle.cpp load.cpp saveGraph.cpp rtree.cpp treeNode.cpp experiment1.cpp event.cpp generate.cpp
	g++ -g -o exp1 load.cpp saveGraph.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp event.cpp experiment1.cpp generate.cpp -I .
run0: build0
	./exp0
	gnuplot -c exp0_synthetic.p
run1: build1
	./exp1
	gnuplot -c exp1_synthetic.p
run2: build2
	./exp2
	gnuplot -c exp2.p
run0_real: build0
	./exp0 real
	gnuplot -c exp0_real.p
run1_real: build1
	./exp1 real
	gnuplot -c exp1_real.p
