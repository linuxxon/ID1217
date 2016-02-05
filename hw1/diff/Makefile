# Makefile for ID1217 Homework 1 - Problem 5
#
# Targets:
# 	build - build program
# 	debug - build with debuging enabled (prints and -g)
# 	clean - remove any executables

CC= gcc

CFLAGS= -O4
DEBUGFLAGS= -g -DDEBUG

LDFLAGS= -lpthread

.PHONY: build debug clean

build:
	$(CC) $(CFLAGS) -o diff diff.c $(LDFLAGS)

debug: CFLAGS+=$(DEBUGFLAGS)
debug:
	$(CC) $(CFLAGS) -o diff diff.c $(LDFLAGS)

clean:
	-rm -f tee
