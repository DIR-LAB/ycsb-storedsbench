CC=gcc
CFLAGS=-g -Wall -pthread -I./
LDFLAGS= -lpmemobj -lpmem
SUBDIRS=array
SUBSRCS=$(wildcard array/*.c)
OBJECTS=$(SUBSRCS:.c=.o)
EXEC=storeds

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): $(wildcard *.c) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)
