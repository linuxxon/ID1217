#!/bin/sh
#
# Homework 1 - Problem 1: Compute Sum, Min and Max of Matrix ELements (20 points)
# 
# Changes done:
#   Uncommenting 'rand()%99' causes overflow with ints so all sums are converted to
#   longs insted
# 
# 1.a) Is an extention of the given code (from matrixSum.c) that also finds the
#      value and position of the minimal and maximal elements in the matrix.
#      This is done with two global structs with respective mutex's
#
# 1.b) Same result as 1.a) but implemented with pthread_join in main thread instead
#      of barrier semantics. The idea of having an array with subsums has also been
#      scrapped
#
# 1.c) Change of the task assignment from 1.b). Instead of static pre-defined areas
#      for each worker the bag semantics is used
#
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

## Build all three versions
make all

echo "Running 1.original)"
./matrixSum

echo "Running 1.a)"
./matrixSum-a

echo "Running 1.b)"
./matrixSum-b

echo "Running 1.c)"
./matrixSum-c

make clean
