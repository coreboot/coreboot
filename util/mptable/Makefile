CC=gcc
CFLAGS=-O2 -Wall -Wextra -Wshadow -Wno-sign-compare

mptable: mptable.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f mptable
