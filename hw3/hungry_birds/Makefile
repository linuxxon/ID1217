# Makefile for ID1217 Homework 3 - Problem 1
#
# Targets:
# 	all - builld all three sub-assignments
# 	build - build stuff
# 	clean - remove any executables

CC= gcc

CFLAGS= 
DFLAGS= -g -DDEBUG
OFLAGS= -O4

LDFLAGS= -pthread

.PHONY: all build debug clean

all: build

build: CFLAGS+=$(OFLAGS)
build:
	$(CC) $(CFLAGS) -o hungry hungry.c $(LDFLAGS)

debug: CFLAGS+=$(DFLAGS)
debug:
	$(CC) $(CFLAGS) -o hungry hungry.c $(LDFLAGS)

clean:
	-rm -f hungry hungry.res
