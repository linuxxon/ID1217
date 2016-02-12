# Makefile for ID1217 Homework 2 - Problem 1
#
# Targets:
# 	all - builld all three sub-assignments
# 	original - build original code given
# 	a - build assignment 1.a
# 	clean - remove any executables

CC= gcc

CFLAGS= -fopenmp -O4

LDFLAGS=

.PHONY: all original a clean

all: original a

original:
	$(CC) $(CFLAGS) -o matrixSum  matrixSum.c $(LDFLAGS)

a:
	$(CC) $(CFLAGS) -o matrixSum-a  matrixSum-a.c $(LDFLAGS)

clean:
	-rm -f matrixSum matrixSum-a
