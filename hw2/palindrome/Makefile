# Makefile for ID1217 Homework 2 - Problem 3
#
# Targets:
# 	all - builld all three sub-assignments
# 	build - build stuff
# 	clean - remove any executables

CC= gcc

CFLAGS= -fopenmp 
DFLAGS= -g -DDEBUG
OFLAGS= -O4

LDFLAGS=

.PHONY: all build debug clean

all: build

build: CFLAGS+=$(OFLAGS)
build:
	$(CC) $(CFLAGS) -o palindrome  palindrome.c $(LDFLAGS)

debug: CFLAGS+=$(DFLAGS)
debug:
	$(CC) $(CFLAGS) -o palindrome  palindrome.c $(LDFLAGS)

clean:
	-rm -f palindrome palindromes.res
