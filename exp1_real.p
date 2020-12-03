set boxwidth 0.1
set style fill solid
set autoscale
set xlabel "Query Size"
set title "EVR Tree Querying | Real Data"
set key noautotitle
set terminal png size 800,600
set output'exp1_real_time.png'
set ylabel "Build Time"
plot "exp1_real_time.data" with boxes
set output'exp1_real_IO.png'
set ylabel "Disk IOs"
plot "exp1_real_IO.data" with boxes