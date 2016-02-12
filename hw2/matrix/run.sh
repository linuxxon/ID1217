#!/bin/sh
#
# Homework 2 - Problem 1: Compute Sum, Min and Max of Matrix ELements (20 points)
# 
# 1.a) Stuffylistuff
#
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

## Build all three versions
make all

#echo "Running 1.original)"
#./matrixSum

echo "Running 1.a)"
./matrixSum-a

echo "Running 1.c) The performance test"

./performance_test.sh
