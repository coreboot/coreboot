#
#
#

obj ?= $(shell pwd)

COMMANDS=create.o bootblock.o delete.o extract.o add.o print.o resize.o
OBJ=$(COMMANDS) cbfstool.o util.o fs.o
INC=cbfstool.h cbfs.h

CC=gcc
CFLAGS=-g -Wall -W -Werror

DESTDIR ?= /usr/local/bin

all: $(obj)/cbfstool $(obj)/tools/cbfs-mkpayload $(obj)/tools/cbfs-mkstage

$(obj)/cbfstool: $(patsubst %,$(obj)/%,$(OBJ))
	$(CC) -o $@ $(patsubst %,$(obj)/%,$(OBJ))

tobj = $(obj)/tools
tsrc = $(shell pwd)/tools

include $(tsrc)/Makefile

$(obj)/%.o: %.c $(INC)
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(obj)/cbfstool $(obj)/tools/cbfs-mkpayload $(obj)/tools/cbfs-mkstage
	@ install -d $(DESTDIR)
	@ install -m 0755 $(obj)/cbfstool $(DESTDIR)/cbfstool
	@ install -m 0755 $(obj)/tools/cbfs-mkstage $(DESTDIR)/cbfs-mkstage
	@ install -m 0755 $(obj)/tools/cbfs-mkpayload $(DESTDIR)/cbfs-mkpayload

tags: 
	ctags *.[ch] */*.[ch]

clean: tools-clean
	rm -f $(patsubst %,$(obj)/%,$(OBJ)) $(obj)/cbfstool

