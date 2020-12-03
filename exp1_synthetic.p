set boxwidth 0.1
set style fill solid
set autoscale
set xrange [0:3.5]
set xtics 0.5,0.5,3
set xlabel "Query Size"
set title "EVR Tree Querying | Synthetic Data (Zipf Factor = 0.7)"
set key noautotitle
set terminal png size 800,600
set output'exp1_synthetic_time.png'
set ylabel "Execution Time (ns)"
plot "exp1_synthetic_time.data" with boxes
set output'exp1_synthetic_IO.png'
set ylabel "Disk IOs"
plot "exp1_synthetic_IO.data" with boxes