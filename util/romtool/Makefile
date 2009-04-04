#
#
#

obj ?= $(shell pwd)

COMMANDS=create.o bootblock.o delete.o add.o print.o resize.o
OBJ=$(COMMANDS) romtool.o util.o fs.o

CC=gcc
CFLAGS=-g -Wall -W #-Werror

H=romtool.h
DESTDIR ?= /usr/local/bin

all: $(obj)/romtool $(obj)/tools/rom-mkpayload $(obj)/tools/rom-mkstage

$(obj)/romtool: $(patsubst %,$(obj)/%,$(OBJ))
	$(CC) -o $@ $(patsubst %,$(obj)/%,$(OBJ))

$(obj)/tools/rom-mkpayload $(obj)/tools/rom-mkstage:
	$(MAKE) -C tools/ obj=$(obj)/tools $(patsubst tools/%, %, $@)

$(obj)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(obj)/romtool $(obj)/tools/rom-mkpayload $(obj)/tools/rom-mkstage
	@ install -d $(DESTDIR)
	@ install -m 0755 $(obj)/romtool $(DESTDIR)/romtool
	@ install -m 0755 $(obj)/tools/rom-mkstage $(DESTDIR)/rom-mkstage
	@ install -m 0755 $(obj)/tools/rom-mkpayload $(DESTDIR)/rom-mkpayload

tags: 
	ctags *.[ch] */*.[ch]

clean:
	$(MAKE) -C tools/ clean
	rm -f $(patsubst %,$(obj)/%,$(OBJ)) $(obj)/romtool
