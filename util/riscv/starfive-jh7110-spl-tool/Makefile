# SPDX-License-Identifier: GPL-2.0+
override CFLAGS=-Wall -Wno-unused-result -Wno-format-truncation -O2

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: spl_tool

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

spl_tool: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

clean:
	rm -f *.o spl_tool
