CC = gcc
CFLAGS = -Wall --std=c99 -g

build: idiff.c
	$(CC) $(CFLAGS) -o idiff idiff.c

clean:
	rm -f *.o idiff

rebuild: clean build

