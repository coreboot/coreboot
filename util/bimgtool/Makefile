obj ?= $(shell pwd)

HOSTCC ?= gcc
CFLAGS ?= -g
CFLAGS += -D_7ZIP_ST
CFLAGS += -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Wwrite-strings -Wredundant-decls -Wno-trigraphs
CFLAGS += -Wstrict-aliasing -Wshadow -Werror
LDFLAGS += -g

all: dep $(obj)/bimgtool

clean:
	rm -f $(obj)/bimgtool

$(obj)/bimgtool: bimgtool.c
	$(HOSTCC) $(CFLAGS) -o $@ $^
