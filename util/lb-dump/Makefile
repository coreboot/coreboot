TARGETS=dump_lb_table

CC=gcc
CFLAGS=-g -O -Wall

all: $(TARGETS)


dump_lb_table: dump_lb_table.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)


