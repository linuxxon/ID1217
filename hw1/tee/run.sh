#!/bin/sh
#
# Homework 1 - Problem 4: UNIX tee
# 
# Script will run several tests to weed out some suspicions of race conditions
#
# The basis for the test is described in README
#
# As per requirements paragraph 2, section 4 a script to run the
# homework must be included.

# Make tee, no ugly prints
make 2>&1 > /dev/null

# Number of times to run the test
iterations=100

echo "Running test for $iterations iterations..."

for i in $(seq $iterations); do
    # Run test
    ./tee "test.outfile" < text > "test.stdout"

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
    if [ $result -ne 0 ]; then
        echo "Test failed!"
        exit 1
    fi
done

# If script got here everything is great!
echo "Test passed successfully!"
