#!/bin/sh
#
# Homework 1 - Problem 5: UNIX diff
#
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

# Make diff, no ugly prints
make 2>&1 > /dev/null

# Run test
./diff file1 file2
