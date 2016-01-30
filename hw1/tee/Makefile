# Makefile for ID1217 Homework 1 - Problem 4
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
	$(CC) $(CFLAGS) -o tee  tee.c $(LDFLAGS)

debug: CFLAGS+=$(DEBUGFLAGS)
debug:
	$(CC) $(CFLAGS) -o tee  tee.c $(LDFLAGS)

clean:
	-rm -f tee
