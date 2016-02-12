#!/bin/bash
###############################
# Test executer for performance evaluation of matrix assignment
#
# Runs $1 amount of iterations
# With maximum $2 amount of threads
#
# Author: Rasmus Linusson
# Last modified: 10/02-2016
# For course ID1217 at KTH
###############################

## Test parameters

# Default to five iterations with maxiumum of 4 threads
num_iterations=5
max_threads=12

size_start=5000
size_increment=10000
size_nr_iterations=3

# Take input from parameters
if [ $# -gt 0 ]; then
    max_threads=$1
    
    if [ $# -gt 3 ]; then
        size_start=$2
        size_increment=$3
        size_nr_iterations=$4
    elif [ $# -gt 1 ]; then # Not what was expected
        echo "Bad number of arguments!"
        echo "Usage: ./performance_test.sh [max_threads [size_start size_increment size_nr_iterations] ]"
        exit 1
    fi
fi


# Test executable to run
executable="./matrixSum-a"

# Ready a list of test parameters
# Ready a list of all test parameters
sizes=$(awk "BEGIN{for(i=0; i < $size_nr_iterations; i+=1) print $size_start+$size_increment*i}")
#sizes=$(seq 10000 10000)
iterations=$(seq $num_iterations)
threads=$(seq $max_threads)

# Temp file
tempfile="median.tmp"

# Data file
datfile="results.dat"

# Print header for dat file
printf "#Size\tThreads\tExecution time\tSpeed up\n" > $datfile

for size in $sizes; do
    # Init single thread execution speed
    single_speed=-1;

    for thread in $threads; do
        echo "Working on [$size x $size] with $thread threads"

        # Init median to -1
        median_speed=-1

        # Clean tempfile
        echo "" > $tempfile

        # Repeat test $iterations times to get median
        for iteration in $iterations; do
            output=$($executable $size $thread)

            # Fetch execution time
            time=$(echo "$output" | grep -o "it took [0-9]*.[0-9]* seconds" | awk '{print $3}' )

            # print to tempfile to sort
            echo "$time" >> $tempfile
        done;

        # Get median value
        median_speed=$(sort -g $tempfile | awk 'NR==3')

        # If one thread, update single speed
        if [ $thread -eq 1 ]; then
            single_speed=$median_speed
        fi

        # Report speedup ( speedup is 1 for 1 thread )
        printf "%d\t%d\t%f\t%f\n" $size $thread $median_speed \
                $(awk "BEGIN{ print $single_speed/$median_speed}") >> $datfile
    done;
done;

# Clean tempfile
if [ -e $tempfile ]; then
    rm $tempfile
fi

# Make a nice plot
gnuplot speedup.gnuplot

if [ $? -eq 0 ]; then
    echo "Teest ran successfully and plot has been produced"
else
    echo "Plotting failed"
    exit 2
fi
