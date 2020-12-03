set boxwidth 0.05
set style fill solid
set autoscale
set xrange [280.0:282.0]
set xtics (281)
set xlabel "Inserted MBRs"
set title "EVR Tree Creation | Real Data"
set key noautotitle
set terminal png size 800,600
set output'exp0_real_time.png'
set ylabel "Build Time (ms)"
plot "exp0_real_time.data" with boxes
set output'exp0_real_IO.png'
set ylabel "Disk IOs"
plot "exp0_real_IO.data" with boxes