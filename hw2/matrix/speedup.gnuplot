## Gnuplot scriptfile for plotting speedup per #threads with differente
# matrix sizes
#
# Used for evaluation of course work for ID1217 at KTH
# Author: Rasmus Linusson
# Last modified: 12/2-2016
#####

set autoscale
unset log
unset label
set xtic auto
set ytic auto
set grid
set terminal png

## Plot memory
set xlabel  "Number of threads"
set ylabel  "Speed relative to single thread performance"

set output "speedup.png"
set title   "Execution speedup for different matrix sizes"
plot    "results.dat" every ::0::11 using 2:4 with linespoints title "5000x5000",\
        "results.dat" every ::12::23 using 2:4 with linespoints title "15000x15000",\
        "results.dat" every ::24::35 using 2:4 with linespoints title "25000x25000"
