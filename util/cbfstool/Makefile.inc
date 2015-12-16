cbfsobj :=
cbfsobj += cbfstool.o
cbfsobj += common.o
cbfsobj += compress.o
cbfsobj += cbfs.o
cbfsobj += cbfs_hash.o
cbfsobj += cbfs_image.o
cbfsobj += cbfs-mkstage.o
cbfsobj += cbfs-mkpayload.o
cbfsobj += elfheaders.o
cbfsobj += mem_pool.o
cbfsobj += region.o
cbfsobj += rmodule.o
cbfsobj += xdr.o
cbfsobj += fit.o
cbfsobj += partitioned_file.o
cbfsobj += fsp1_1_relocate.o
# LZMA
cbfsobj += lzma.o
cbfsobj += LzFind.o
cbfsobj += LzmaDec.o
cbfsobj += LzmaEnc.o
# CRYPTOLIB
cbfsobj += 2sha_utility.o
cbfsobj += 2sha1.o
cbfsobj += 2sha256.o
cbfsobj += 2sha512.o
# FMAP
cbfsobj += fmap.o
cbfsobj += kv_pair.o
cbfsobj += valstr.o
# linux as payload
cbfsobj += linux_trampoline.o
cbfsobj += cbfs-payload-linux.o

fmapobj :=
fmapobj += fmaptool.o
fmapobj += cbfs_sections.o
fmapobj += fmap_from_fmd.o
fmapobj += fmd.o
fmapobj += fmd_parser.o
fmapobj += fmd_scanner.o
# FMAP
fmapobj += fmap.o
fmapobj += kv_pair.o
fmapobj += valstr.o

rmodobj :=
rmodobj += rmodtool.o
rmodobj += rmodule.o
rmodobj += common.o
rmodobj += elfheaders.o
rmodobj += xdr.o

TOOLCFLAGS ?= -Werror -Wall -Wextra
TOOLCFLAGS += -Wcast-qual -Wmissing-prototypes -Wredundant-decls -Wshadow
TOOLCFLAGS += -Wstrict-prototypes -Wwrite-strings
TOOLCPPFLAGS ?= -D_DEFAULT_SOURCE # memccpy() from string.h
TOOLCPPFLAGS += -D_XOPEN_SOURCE=700 # strdup() from string.h
TOOLCPPFLAGS += -I$(top)/util/cbfstool/flashmap
TOOLCPPFLAGS += -I$(top)/util/cbfstool
TOOLCPPFLAGS += -I$(objutil)/cbfstool
TOOLCPPFLAGS += -I$(top)/src/commonlib/include
TOOLCPPFLAGS += -DNEED_VB2_SHA_LIBRARY
TOOLCPPFLAGS += -I$(top)/3rdparty/vboot/firmware/include
TOOLCPPFLAGS += -I$(top)/3rdparty/vboot/firmware/2lib/include
# UEFI header file support. It's not pretty, but that's what we currently
# have right now.
TOOLCPPFLAGS += -I$(top)/src
TOOLCPPFLAGS += -I$(top)/src/vendorcode/intel/edk2/uefi_2.4/MdePkg/Include

TOOLLDFLAGS ?=
HOSTCFLAGS += -fms-extensions

ifeq ($(shell uname -s | cut -c-7 2>/dev/null), MINGW32)
TOOLCFLAGS += -mno-ms-bitfields
endif
ifeq ($(shell uname -o 2>/dev/null), Cygwin)
TOOLCFLAGS+=-std=gnu99
TOOLCPPFLAGS+=-D_GNU_SOURCE
else
TOOLCFLAGS+=-std=c99
endif

$(objutil)/cbfstool/%.o: $(objutil)/cbfstool/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/flashmap/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/3rdparty/vboot/firmware/2lib/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/src/commonlib/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/cbfstool: $(addprefix $(objutil)/cbfstool/,$(cbfsobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cbfsobj))

$(objutil)/cbfstool/fmaptool: $(addprefix $(objutil)/cbfstool/,$(fmapobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(fmapobj))

$(objutil)/cbfstool/rmodtool: $(addprefix $(objutil)/cbfstool/,$(rmodobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(rmodobj))

# Yacc source is superset of header
$(objutil)/cbfstool/fmd.o: TOOLCFLAGS += -Wno-redundant-decls
$(objutil)/cbfstool/fmd_parser.o: TOOLCFLAGS += -Wno-redundant-decls
# Lex generates unneeded functions and declarations
$(objutil)/cbfstool/fmd_scanner.o: TOOLCFLAGS += -Wno-redundant-decls
$(objutil)/cbfstool/fmd_scanner.o: TOOLCFLAGS += -Wno-unused-function
# Tolerate lzma sdk warnings
$(objutil)/cbfstool/LzmaEnc.o: TOOLCFLAGS += -Wno-sign-compare -Wno-cast-qual
# Tolerate vboot warnings
$(objutil)/cbfstool/2sha_utility.o: TOOLCFLAGS += -Wno-sign-compare
$(objutil)/cbfstool/2sha1.o: TOOLCFLAGS += -Wno-cast-qual
# Tolerate commonlib warnings
$(objutil)/cbfstool/region.o: TOOLCFLAGS += -Wno-sign-compare -Wno-cast-qual
$(objutil)/cbfstool/cbfs.o: TOOLCFLAGS += -Wno-sign-compare -Wno-cast-qual
$(objutil)/cbfstool/mem_pool.o: TOOLCFLAGS += -Wno-sign-compare -Wno-cast-qual

$(objutil)/cbfstool/fmd.o: $(objutil)/cbfstool/fmd_parser.h
$(objutil)/cbfstool/fmd.o: $(objutil)/cbfstool/fmd_scanner.h
$(objutil)/cbfstool/fmd_parser.o: $(objutil)/cbfstool/fmd.o
$(objutil)/cbfstool/fmd_scanner.o: $(objutil)/cbfstool/fmd.o

$(objutil)/cbfstool/%: $(top)/util/cbfstool/%_shipped
	mkdir -p $(dir $@)
	cp $< $@

ifeq ($(CONFIG_FMD_GENPARSER),y)
TOOLLEX := lex
TOOLYACC := yacc

# the .c rule also creates .h
$(top)/util/cbfstool/fmd_scanner.h_shipped: $(top)/util/cbfstool/fmd_scanner.c_shipped
$(top)/util/cbfstool/fmd_scanner.c_shipped: $(top)/util/cbfstool/fmd_scanner.l
	echo "    LEX        util/cbfstool/fmd_scanner.[ch]"
	$(TOOLLEX) -t --header-file=$(top)/util/cbfstool/fmd_scanner.h_shipped $< >$(top)/util/cbfstool/fmd_scanner.c_shipped

# the .c rule also creates .h
$(top)/util/cbfstool/fmd_parser.h_shipped: $(top)/util/cbfstool/fmd_parser.c_shipped
$(top)/util/cbfstool/fmd_parser.c_shipped: $(top)/util/cbfstool/fmd_parser.y
	echo "    YACC       util/cbfstool/fmd_parser.[ch]"
	$(TOOLYACC) -d $<
	mv -f y.tab.c $(top)/util/cbfstool/fmd_parser.c_shipped
	mv -f y.tab.h $(top)/util/cbfstool/fmd_parser.h_shipped

endif # CONFIG_FMD_GENPARSER
