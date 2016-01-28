# Makefile for ID1217 Homework 1 - Problem 1
#
# Targets:
# 	all - builld all three sub-assignments
# 	a - build only assignment 1.a
# 	b - build only assignment 1.b
# 	c - build only assignment 1.c

CC= gcc

CFLAGS=

LDFLAGS= -lpthread

.PHONY: all a b c clean

all: a b c

a:
	$(CC) $(CFLAGS) -o matrixSum-a  matrixSum-a.c $(LDFLAGS)

b:
	$(CC) $(CFLAGS) -o matrixSum-b  matrixSum-b.c $(LDFLAGS)

c:
	$(CC) $(CFLAGS) -o matrixSum-c  matrixSum-c.c $(LDFLAGS)

clean:
	-rm -f matrixSum-a matrixSum-b matrixSum-c
