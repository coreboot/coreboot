CC=gcc
CFLAGS=-O2 -Wall

all: genprof

genprof: genprof.o
	$(CC) $(CFLAGS) -o genprof $^

clean:
	rm -f genprof  *.o *~

distclean: clean
