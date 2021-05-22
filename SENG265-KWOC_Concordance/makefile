#
# "makefile" for the penultimate implementation of KWOC (kwoc3.c)
# Spring 2020, assignment #3.
#

CC=gcc

# The line with -DDEBUG can be used for development. When
# building your code for evaluation, however, the line *without*
# the -DDEBUG will be used.
#

CFLAGS=-c -Wall -g -DDEBUG -std=c99
#CFLAGS=-c -Wall -g -std=c99


all: kwoc3

kwoc3: kwoc3.o listy.o emalloc.o
	$(CC) kwoc3.o listy.o emalloc.o -o kwoc3

kwoc3.o: kwoc3.c listy.h emalloc.h
	$(CC) $(CFLAGS) kwoc3.c

listy.o: listy.c listy.h emalloc.h
	$(CC) $(CFLAGS) listy.c

emalloc.o: emalloc.c emalloc.h
	$(CC) $(CFLAGS) emalloc.c

clean:
	rm -rf *.o kwoc3 
