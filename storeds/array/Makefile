CC=gcc
CFLAGS=-c -g -Wall -fgnu-tm
INCLUDES=-I../
#todo: will update the makefile when we can make the array_pmem.c workable
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
LDFLAGS=-lpmem -lpmemobj

all: $(SOURCES) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJECTS)

#	$(CC) $(CFLAGS) array_dram.c -o array_dram.o $(LDFLAGS)
#	$(CC) $(CFLAGS) array_pmem.c -o array_pmem.o $(LDFLAGS)