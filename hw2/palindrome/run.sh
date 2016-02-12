#!/bin/sh
#
# Homework 2 - Problem 3: Find Palindromic Words (40 points)
# 
# Things
#
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

## Build with full optimization
make

if [ $? -ne 0 ]; then
    echo "Make failed"
    exit 1
fi

echo "Running with 12 threads"
./palindrome words

echo "Running the performance test"
./performance_test.sh

if [ $? -ne 0 ]; then
    echo "Performance test failed"
    exit 2
fi
