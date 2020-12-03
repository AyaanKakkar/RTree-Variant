set boxwidth 0.0013
set style fill solid
set autoscale
set xrange [0:0.049]
set xtics 0.007,0.007,0.042
set xlabel "Query Size"
set title "EVR Tree Querying | Real Data"
set key noautotitle
set terminal png size 800,600
set output'exp1_real_time.png'
set ylabel "Execution Time (ns)"
plot "exp1_real_time.data" with boxes
set yrange [0:30]
set output'exp1_real_IO.png'
set ylabel "Disk IOs"
plot "exp1_real_IO.data" with boxes