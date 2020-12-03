set boxwidth 0.1
set style fill solid
set autoscale
set xlabel "Inserted MBRs"
set title "EVR Tree Creation | Synthetic Data (Zipf Factor = 0.7)"
set key noautotitle
set terminal png size 800,600
set output'exp0_synthetic_time.png'
set ylabel "Build Time"
plot "exp0_synthetic_time.data" with boxes
set output'exp0_synthetic_IO.png'
set ylabel "Disk IOs"
plot "exp0_synthetic_IO.data" with boxes