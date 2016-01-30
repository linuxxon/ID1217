#!/bin/sh
#
# Homework 1 - Problem 4: UNIX tee
# 
#
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

# Make tee
make

# Run test
./tee test.outfile < text > test.stdout

# Variable to save result: 0 - Passed, 1 - Failed
result=1

# Determine the result
cmp test.outfile text
if [ $? -eq 0 ]; then
    cmp test.stdout text
    if [ $? -eq 0 ]; then
        result=0
    else
        result=1
    fi
else
    result=1
fi

# Clean up the output files
rm -f test.outfile
rm -f test.stdout

# Print and return result
if [ $result -eq 0 ]; then
    echo "Test passed successfully"
    exit 0
else
    echo "Test failed!"
    exit 1
fi
