CC=g++
CFLAGS=-Wall -std=c++11
INCLUDES=-I../
HEADERS=$(wildcard *.h) #$(wildcard array/*.h)
LDFLAGS=-lpmem -lpmemobj
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(MAKE) -C ./array
# 	sudo $(MAKE) install -C ./hiredis
#	$(MAKE) -C ../db
#	$(CC) $(CFLAGS) $(INCLUDES) $^ ../db/storeds_db.o array/array_dram.o array/array_pmem.o $(HEADERS) $(LDFLAGS) -o $@

clean:
	$(RM) $(OBJECTS)