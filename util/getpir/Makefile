#
#
#

CC=gcc
CFLAGS=-O2 -D_GNU_SOURCE -DGETPIR -Wall

all: getpir

getpir: getpir.o checksum.o code_gen.o
	$(CC) $(CFLAGS) -o getpir $^

checkpir: checkpir.c checksum.o irq_tables.o
	$(CC) $(CFLAGS) -o checkpir $^

irq_tables.c: getpir
	./getpir

clean:
	rm -f getpir checkpir *.o *~

distclean: clean
	rm -f irq_tables.o irq_tables.c

