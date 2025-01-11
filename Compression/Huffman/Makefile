# Makefile usefull for compilation

CC = gcc
CFLAGS = -Wall -g

all: huffman_main

huffman_main: huffman_main.o functions_huffman.o
	$(CC) $(CFLAGS) -o huffman_main huffman_main.o functions_huffman.o

huffman_main.o: huffman_main.c
	$(CC) $(CFLAGS) -c huffman_main.c

functions_huffman.o: functions_huffman.c
	$(CC) $(CFLAGS) -c functions_huffman.c

clean:
	rm -f *.o huffman_main

test_huffman: test_huffman.o functions_huffman.o
	$(CC) $(CFLAGS) -o test_huffman test_huffman.o functions_huffman.o

test_huffman.o: test_huffman.c
	$(CC) $(CFLAGS) -c test_huffman.c