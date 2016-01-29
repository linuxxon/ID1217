# Makefile for ID1217 Homework 1 - Problem 1
#
# Targets:
# 	all - builld all three sub-assignments
# 	original - build original code given
# 	a - build assignment 1.a
# 	b - build assignment 1.b
# 	c - build assignment 1.c
# 	clean - remove any executables

CC= gcc

CFLAGS=

LDFLAGS= -lpthread

.PHONY: all original a b c clean

all: original a b c

original:
	$(CC) $(CFLAGS) -o matrixSum  matrixSum.c $(LDFLAGS)

a:
	$(CC) $(CFLAGS) -o matrixSum-a  matrixSum-a.c $(LDFLAGS)

b:
	$(CC) $(CFLAGS) -o matrixSum-b  matrixSum-b.c $(LDFLAGS)

c:
	$(CC) $(CFLAGS) -o matrixSum-c  matrixSum-c.c $(LDFLAGS)

clean:
	-rm -f matrixSum matrixSum-a matrixSum-b matrixSum-c
