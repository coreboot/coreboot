#
#
#

obj ?= $(shell pwd)

COMMANDS=create.o bootblock.o delete.o add.o print.o resize.o
OBJ=$(COMMANDS) romtool.o util.o fs.o
INC=romtool.h romfs.h

CC=gcc
CFLAGS=-g -Wall # -W -Werror

DESTDIR ?= /usr/local/bin

all: $(obj)/romtool $(obj)/tools/rom-mkpayload $(obj)/tools/rom-mkstage

$(obj)/romtool: $(patsubst %,$(obj)/%,$(OBJ))
	$(CC) -o $@ $(patsubst %,$(obj)/%,$(OBJ))

tobj = $(obj)/tools
tsrc = $(shell pwd)/tools

include tools/Makefile

$(obj)/%.o: %.c $(INC)
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(obj)/romtool $(obj)/tools/rom-mkpayload $(obj)/tools/rom-mkstage
	@ install -d $(DESTDIR)
	@ install -m 0755 $(obj)/romtool $(DESTDIR)/romtool
	@ install -m 0755 $(obj)/tools/rom-mkstage $(DESTDIR)/rom-mkstage
	@ install -m 0755 $(obj)/tools/rom-mkpayload $(DESTDIR)/rom-mkpayload

tags: 
	ctags *.[ch] */*.[ch]

clean: tools-clean
	rm -f $(patsubst %,$(obj)/%,$(OBJ)) $(obj)/romtool

