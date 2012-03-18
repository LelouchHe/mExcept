
CC=gcc
CFLAGS=-g -rdynamic

.PHONY: all clean

OBJS=except.o util.o

all: test

test: test.o $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o test
	
