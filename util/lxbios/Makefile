# $Id$

PROJECT = lxbios
CC = gcc
CFLAGS = -O2 -W -Wall
LDFLAGS =
OBJS = common.o compute_ip_checksum.o hexdump.o cmos_lowlevel.o \
       reg_expr.o layout.o layout_file.o lbtable.o cmos_ops.o input_file.o \
       opts.o lxbios.o
HEADERS = common.h ip_checksum.h coreboot_tables.h hexdump.h \
          cmos_lowlevel.h reg_expr.h layout.h layout_file.h lbtable.h \
          cmos_ops.h input_file.h opts.h

all: lxbios man

lxbios: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

man: lxbios.1.gz

$(OBJS): $(HEADERS)

lxbios.1.gz: lxbios.1
	gzip -c --best lxbios.1 > lxbios.1.gz

clean:
	rm -f *.o lxbios lxbios.1.gz

