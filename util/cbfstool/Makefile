obj ?= .

HOSTCC ?= $(CC)

CFLAGS += -g3
CFLAGS += -std=c99 -Werror -Wall -Wextra
CFLAGS += -Wcast-qual -Wmissing-prototypes -Wredundant-decls -Wshadow
CFLAGS += -Wstrict-prototypes -Wwrite-strings
CPPFLAGS += -D_DEFAULT_SOURCE # memccpy() from string.h
CPPFLAGS += -D_POSIX_C_SOURCE=200809L # strdup() from string.h
CPPFLAGS += -Iflashmap
LDFLAGS += -g3

CBFSTOOL_BINARY:=$(obj)/cbfstool
CBFSTOOL_COMMON:=common.o cbfs_image.o compress.o fit.o
CBFSTOOL_COMMON+=elfheaders.o cbfs-mkstage.o cbfs-mkpayload.o xdr.o
CBFSTOOL_COMMON+=partitioned_file.o linux_trampoline.o cbfs-payload-linux.o
CBFSTOOL_COMMON+=rmodule.o
# LZMA
CBFSTOOL_COMMON+=lzma/lzma.o
CBFSTOOL_COMMON+=lzma/C/LzFind.o  lzma/C/LzmaDec.o  lzma/C/LzmaEnc.o
# FMAP
CBFSTOOL_COMMON+=flashmap/fmap.o
CBFSTOOL_COMMON+=flashmap/kv_pair.o flashmap/valstr.o

CBFSTOOL_COMMON:=$(addprefix $(obj)/,$(CBFSTOOL_COMMON))

FMAPTOOL_BINARY:=$(obj)/fmaptool
FMAPTOOL_COMMON:=cbfs_sections.o fmap_from_fmd.o
FMAPTOOL_COMMON+=fmd.o fmd_parser.o fmd_scanner.o
# FMAP
FMAPTOOL_COMMON+=flashmap/fmap.o
FMAPTOOL_COMMON+=flashmap/kv_pair.o flashmap/valstr.o

FMAPTOOL_COMMON:=$(addprefix $(obj)/,$(FMAPTOOL_COMMON))

RMODTOOL_BINARY:=$(obj)/rmodtool
RMODTOOL_COMMON:=rmodule.o common.o elfheaders.o xdr.o

RMODTOOL_COMMON:=$(addprefix $(obj)/,$(RMODTOOL_COMMON))

FMAPTESTS_BINARY:=$(obj)/flashmap_tests
FMAPTESTS_COMMON:=flashmap/fmap.o flashmap/valstr.o flashmap/kv_pair.o

FMAPTESTS_COMMON:=$(addprefix $(obj)/,$(FMAPTESTS_COMMON))

GENERATED:=fmd_parser.c fmd_parser.h fmd_scanner.c fmd_scanner.h

.PHONY: all
all: .dependencies $(CBFSTOOL_BINARY) $(FMAPTOOL_BINARY) $(RMODTOOL_BINARY)

$(obj)/%: $(obj)/%.o
	mkdir -p $(dir $@)
	$(HOSTCC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
$(obj)/%.o: %.c
	mkdir -p $(dir $@)
	$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

fmd_scanner.h: fmd_scanner.c
fmd_scanner.c: fmd_scanner.l
	$(LEX) $(LFLAGS) -t --header-file=$*.h $< >$*.c

fmd_parser.h: fmd_parser.c
fmd_parser.c: fmd_parser.y
	$(YACC) $(YFLAGS) -d $<
	mv -f y.tab.c $*.c
	mv -f y.tab.h $*.h

.PHONY: clean
clean:
	$(RM) $(CBFSTOOL_COMMON) $(CBFSTOOL_BINARY).o $(CBFSTOOL_BINARY)
	$(RM) $(FMAPTOOL_COMMON) $(FMAPTOOL_BINARY).o $(FMAPTOOL_BINARY)
	$(RM) $(RMODTOOL_COMMON) $(RMODTOOL_BINARY).o $(RMODTOOL_BINARY)
	$(RM) $(FMAPTESTS_COMMON) $(FMAPTESTS_BINARY).o $(FMAPTESTS_BINARY)
.PHONY: distclean
distclean: clean
	$(RM) .dependencies
.PHONY: maintainer-clean
maintainer-clean: distclean
	$(RM) $(GENERATED)

tags: $(GENERATED)
	ctags *.[ch]

.dependencies: $(GENERATED)
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM -MG *.c > $@
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM flashmap/*.c >> $@
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM lzma/*.c >> $@
	@$(HOSTCC) $(CPPFLAGS) $(CFLAGS) -MM lzma/C/*.c >> $@
	@sed -i 's|.*:.*|$$(obj)/&|' $@

$(CBFSTOOL_BINARY): $(CBFSTOOL_COMMON)
$(FMAPTOOL_BINARY): $(FMAPTOOL_COMMON)
$(RMODTOOL_BINARY): $(RMODTOOL_COMMON)

# This target must be built manually
$(FMAPTESTS_BINARY): $(FMAPTESTS_COMMON)

# Yacc source is superset of header
$(obj)/fmd_parser.o: CFLAGS += -Wno-redundant-decls
# Lex generates unneeded functions and declarations
$(obj)/fmd_scanner.o: CFLAGS += -Wno-redundant-decls
$(obj)/fmd_scanner.o: CFLAGS += -Wno-unused-function
# Tolerate lzma sdk warnings
$(obj)/lzma/C/LzmaEnc.o: CFLAGS += -Wno-sign-compare -Wno-cast-qual

-include .dependencies
