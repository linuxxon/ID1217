## Gnuplot scriptfile for plotting speedup per #threads
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
set title   "Execution speedup for file: words"
plot    "results.dat" using 1:3 with linespoints
