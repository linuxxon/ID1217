#!/bin/bash
###############################
# Test executer for performance evaluation of palindromic word finder
#
# Looks through file $1 for palindroms
# using maximum $2 amount of threads
#
# Author: Rasmus Linusson
# Last modified: 12/02-2016
# For course ID1217 at KTH
###############################

## Test parameters

# Default to five iterations with maxiumum of 4 threads
num_iterations=5
max_threads=12
wordfile="words"

# Take input from parameters
if [ $# -gt 0 ]; then
    max_threads=$1

    if [ $# -gt 1 ]; then
        wordfile=$2
    fi
fi

# Test executable to run
executable="./palindrome"

# Ready a list of all test parameters
iterations=$(seq $num_iterations)
threads=$(seq $max_threads)

# Temp file
tempfile="median.tmp"

# Data file
datfile="results.dat"

# Print header for dat file
printf "# Threads\tExecution time\tSpeed up\n" > $datfile

# Init single thread execution speed
single_speed=-1;

for thread in $threads; do
    echo "Working on $wordfile with $thread threads"

    # Init median to -1
    median_speed=-1

    # Clean tempfile
    echo "" > $tempfile

    # Repeat test $iterations times to get median
    for iteration in $iterations; do
        output=$($executable $wordfile $thread)

        # Fetch execution time
        time=$(echo "$output" | grep -o "in [0-9]*.[0-9]* seconds" | awk '{print $2}' )

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
    printf "%d\t%f\t%f\n" $thread $median_speed \
            $(awk "BEGIN{ print $single_speed/$median_speed}") >> $datfile
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
