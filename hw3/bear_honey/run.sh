#!/bin/sh
#
# Homework 3 - Problem 2: The Bear and Honeybees Problem (20 points)
# 
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

## Build with full optimization
make

if [ $? -ne 0 ]; then
    echo "Make failed"
    exit 1
fi

./hungry
