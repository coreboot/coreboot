obj ?= $(shell pwd)

HOSTCC   ?= gcc
CFLAGS   ?= -g -Wall -Werror
CFLAGS   += -D_7ZIP_ST

BINARY:=$(obj)/cbfstool

COMMON:=cbfstool.o common.o cbfs_image.o compress.o fit.o
COMMON+=cbfs-mkstage.o cbfs-mkpayload.o
# LZMA
COMMON+=lzma/lzma.o
COMMON+=lzma/C/LzFind.o  lzma/C/LzmaDec.o  lzma/C/LzmaEnc.o

COMMON+=linux_trampoline.o cbfs-payload-linux.o

COMMON:=$(addprefix $(obj)/,$(COMMON))

all: dep $(BINARY)

$(obj)/%.o: %.c
	$(HOSTCC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(COMMON) $(BINARY)

tags:
	ctags *.[ch]

$(obj)/cbfstool:$(COMMON)
	$(HOSTCC) $(CFLAGS) -o $@ $^

dep:
	@$(HOSTCC) $(CFLAGS) -MM *.c > .dependencies
	@$(HOSTCC) $(CFLAGS) -MM lzma/*.c >> .dependencies
	@$(HOSTCC) $(CFLAGS) -MM lzma/C/*.c >> .dependencies

-include .dependencies
