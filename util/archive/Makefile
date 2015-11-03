PROGRAM = archive
CC ?= gcc

SRCS = $(PROGRAM).c

all: $(PROGRAM)

$(PROGRAM): $(SRCS)
	$(CC) -o $@ $^

clean:
	rm -f $(PROGRAM) *.o

.PHONY: all clean
