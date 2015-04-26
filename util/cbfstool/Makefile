obj ?= .

HOSTCC ?= $(CC)

CFLAGS += -g3
CFLAGS += -std=c99 -Werror -Wall -Wextra
CFLAGS += -Wcast-qual -Wmissing-prototypes -Wredundant-decls -Wshadow
CFLAGS += -Wstrict-prototypes -Wwrite-strings
CPPFLAGS += -D_POSIX_C_SOURCE=200809L # strdup() from string.h
LINKFLAGS += -g3

CBFSTOOL_BINARY:=$(obj)/cbfstool
CBFSTOOL_COMMON:=common.o cbfs_image.o compress.o fit.o
CBFSTOOL_COMMON+=elfheaders.o cbfs-mkstage.o cbfs-mkpayload.o xdr.o
CBFSTOOL_COMMON+=linux_trampoline.o cbfs-payload-linux.o
# LZMA
CBFSTOOL_COMMON+=lzma/lzma.o
CBFSTOOL_COMMON+=lzma/C/LzFind.o  lzma/C/LzmaDec.o  lzma/C/LzmaEnc.o

CBFSTOOL_COMMON:=$(addprefix $(obj)/,$(CBFSTOOL_COMMON))


RMODTOOL_BINARY:=$(obj)/rmodtool
RMODTOOL_COMMON:=rmodule.o common.o elfheaders.o xdr.o

RMODTOOL_COMMON:=$(addprefix $(obj)/,$(RMODTOOL_COMMON))

.PHONY: all
all: .dependencies $(CBFSTOOL_BINARY) $(RMODTOOL_BINARY)

$(obj)/%: $(obj)/%.o
	mkdir -p $(dir $@)
	$(HOSTCC) $(LINKFLAGS) -o $@ $^ $(LDLIBS)
$(obj)/%.o: %.c
	mkdir -p $(dir $@)
	$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) $(CBFSTOOL_COMMON) $(CBFSTOOL_BINARY).o $(CBFSTOOL_BINARY)
	$(RM) $(RMODTOOL_COMMON) $(RMODTOOL_BINARY).o $(RMODTOOL_BINARY)
.PHONY: distclean
distclean: clean
	$(RM) .dependencies

tags:
	ctags *.[ch]

.dependencies:
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM -MG *.c > $@
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM lzma/*.c >> $@
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM lzma/C/*.c >> $@
	@sed -i 's|.*:.*|$$(obj)/&|' $@

$(CBFSTOOL_BINARY): $(CBFSTOOL_COMMON)
$(RMODTOOL_BINARY): $(RMODTOOL_COMMON)

# Tolerate lzma sdk warnings
$(obj)/lzma/C/LzmaEnc.o: CFLAGS += -Wno-sign-compare -Wno-cast-qual

-include .dependencies
