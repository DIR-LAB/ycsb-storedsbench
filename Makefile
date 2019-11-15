CC=g++
CFLAGS=-std=c++11 -g -Wall -pthread -I./
LDFLAGS= -lpthread -ltbb -lpmemobj -lpmem
SUBDIRS=core db storeds
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc) $(wildcard storeds/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc
EXECTEST=ycsbc_test
EXECPARALLEL=ycsbc_parallel
EXECPARALLELTEST=ycsbc_parallel_test

all: $(SUBDIRS) $(EXEC) $(EXECTEST) ${EXECPARALLEL} ${EXECPARALLELTEST}

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): ycsbc.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECTEST): ycsbc_test.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLEL): ycsbc_parallel.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLELTEST): ycsbc_parallel_test.cc $(OBJECTS)
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

