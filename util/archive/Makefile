PROGRAM = archive
HOSTCC ?= gcc

SRCS = $(PROGRAM).c

all: $(PROGRAM)

$(PROGRAM): $(SRCS)
	$(HOSTCC) -o $@ $^

clean:
	rm -f $(PROGRAM) *.o

.PHONY: all clean
