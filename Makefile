CC=g++
CFLAGS=-std=c++11 -g -Wall -pthread -I./
LDFLAGS= -lpthread -ltbb -lpmemobj -lpmem
SUBDIRS=core db
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
STOREDSSRC=$(wildcard storeds/array/*.c) $(wildcard storeds/linkedlist/*.c) $(wildcard storeds/hashmap/*.c) $(wildcard storeds/skiplist/*.c) $(wildcard storeds/rbtree/*.c) $(wildcard storeds/btree/*.c)
EXEC=ycsbc
EXECTEST=ycsbc_test
EXECPARALLEL=ycsbc_parallel
EXECPARALLELTEST=ycsbc_parallel_test

all: $(SUBDIRS) $(EXEC) $(EXECTEST) ${EXECPARALLEL} ${EXECPARALLELTEST}

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): ycsbc.cc $(STOREDSSRC) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECTEST): ycsbc_test.cc $(STOREDSSRC) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLEL): ycsbc_parallel.cc $(STOREDSSRC) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLELTEST): ycsbc_parallel_test.cc $(STOREDSSRC) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)
	$(RM) $(EXECTEST)
	$(RM) $(EXECPARALLEL)
	$(RM) $(EXECPARALLELTEST)

.PHONY: $(SUBDIRS) $(EXEC) $(EXECTEST) $(EXECPARALLEL) $(EXECPARALLELTEST)

