# $Id$

PROJECT = nvramtool
CC = gcc
CFLAGS = -O2 -W -Wall
LDFLAGS =
OBJS = common.o compute_ip_checksum.o hexdump.o cmos_lowlevel.o \
       reg_expr.o layout.o layout_file.o lbtable.o cmos_ops.o input_file.o \
       opts.o nvramtool.o
HEADERS = common.h ip_checksum.h coreboot_tables.h hexdump.h \
          cmos_lowlevel.h reg_expr.h layout.h layout_file.h lbtable.h \
          cmos_ops.h input_file.h opts.h

all: nvramtool man

nvramtool: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

man: nvramtool.1.gz

$(OBJS): $(HEADERS)

nvramtool.1.gz: nvramtool.1
	gzip -c --best nvramtool.1 > nvramtool.1.gz

clean:
	rm -f *.o nvramtool nvramtool.1.gz

