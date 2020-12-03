set boxwidth 0.1
set style fill solid
set autoscale
set xlabel "Zipf Factor"
set title "EVR Tree Querying | Synthetic Data"
set key noautotitle
set terminal png size 800,600
set output'exp2_time.png'
set ylabel "Build Time"
plot "exp2_time.data" with boxes
set output'exp2_IO.png'
set ylabel "Disk IOs"
plot "exp2_IO.data" with boxes