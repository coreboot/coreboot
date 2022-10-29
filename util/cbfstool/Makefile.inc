## SPDX-License-Identifier: GPL-2.0-only
compressionobj :=
compressionobj += compress.o
# LZ4
compressionobj += lz4.o
compressionobj += lz4hc.o
compressionobj += lz4frame.o
compressionobj += xxhash.o
compressionobj += lz4_wrapper.o
# LZMA
compressionobj += lzma.o
compressionobj += LzFind.o
compressionobj += LzmaDec.o
compressionobj += LzmaEnc.o

cbfsobj :=
cbfsobj += cbfstool.o
cbfsobj += common.o
cbfsobj += cbfs_image.o
cbfsobj += cbfs-mkstage.o
cbfsobj += cbfs-mkpayload.o
cbfsobj += elfheaders.o
cbfsobj += rmodule.o
cbfsobj += xdr.o
cbfsobj += partitioned_file.o
cbfsobj += platform_fixups.o
# COMMONLIB
cbfsobj += cbfs_private.o
cbfsobj += fsp_relocate.o
# FMAP
cbfsobj += fmap.o
cbfsobj += kv_pair.o
cbfsobj += valstr.o
# linux as payload
cbfsobj += linux_trampoline.o
cbfsobj += cbfs-payload-linux.o
# compression algorithms
cbfsobj += $(compressionobj)

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

ifwiobj :=
ifwiobj += ifwitool.o
ifwiobj += common.o

ifitobj :=
ifitobj += ifittool.o
ifitobj += common.o
ifitobj += fit.o
ifitobj += cbfs_image.o
# Make it link ....
ifitobj += xdr.o
ifitobj += elfheaders.o
ifitobj += partitioned_file.o
ifitobj += cbfs-mkstage.o
ifitobj += cbfs-mkpayload.o
ifitobj += rmodule.o
# FMAP
ifitobj += fmap.o
ifitobj += kv_pair.o
ifitobj += valstr.o
# compression algorithms
ifitobj += $(compressionobj)


cbfscompobj :=
cbfscompobj += $(compressionobj)
cbfscompobj += cbfscomptool.o

amdcompobj :=
amdcompobj += amdcompress.o
amdcompobj += elfheaders.o
amdcompobj += common.o
amdcompobj += xdr.o

elogobj :=
elogobj := elogtool.o
elogobj += eventlog.o
elogobj += valstr.o
elogobj += elog.o
elogobj += common.o
elogobj += flashrom.o

include $(top)/util/cbfstool/fpt_formats/Makefile.inc
cse_fpt_obj :=
cse_fpt_obj += cse_fpt.o
cse_fpt_obj += common.o
cse_fpt_obj += cse_helpers.o
cse_fpt_obj += $(foreach var, $(fpt_formats_obj), $(var))

include $(top)/util/cbfstool/bpdt_formats/Makefile.inc
cse_serger_obj :=
cse_serger_obj += cse_serger.o
cse_serger_obj += common.o
cse_serger_obj += cse_helpers.o
cse_serger_obj += $(foreach var, $(bpdt_formats_obj), $(var))

TOOLCFLAGS ?= -Werror -Wall -Wextra -Wshadow
TOOLCFLAGS += -Wcast-qual -Wmissing-prototypes -Wredundant-decls -Wshadow
TOOLCFLAGS += -Wstrict-prototypes -Wwrite-strings
TOOLCFLAGS += -O2
TOOLCPPFLAGS ?= -D_DEFAULT_SOURCE # memccpy() from string.h
TOOLCPPFLAGS += -D_BSD_SOURCE -D_SVID_SOURCE # _DEFAULT_SOURCE for older glibc
TOOLCPPFLAGS += -D_GNU_SOURCE # memmem() from string.h
TOOLCPPFLAGS += -I$(top)/util/cbfstool/flashmap
TOOLCPPFLAGS += -I$(top)/util/cbfstool
TOOLCPPFLAGS += -I$(objutil)/cbfstool
TOOLCPPFLAGS += -I$(top)/src/commonlib/include -I$(top)/src/commonlib/bsd/include
TOOLCPPFLAGS += -include $(top)/src/commonlib/bsd/include/commonlib/bsd/compiler.h
TOOLCPPFLAGS += -I$(VBOOT_SOURCE)/firmware/include
TOOLCPPFLAGS += -I$(VBOOT_SOURCE)/firmware/2lib/include
TOOLCPPFLAGS += -I$(VBOOT_SOURCE)/host/include
TOOLCPPFLAGS += -I$(VBOOT_SOURCE)/host/lib/include
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
TOOLCFLAGS+=-std=gnu11
TOOLCPPFLAGS+=-D_GNU_SOURCE
else
TOOLCFLAGS+=-std=c11
endif

LZ4CFLAGS ?= -Wno-strict-prototypes

VBOOT_HOSTLIB = $(VBOOT_HOST_BUILD)/libvboot_host.a

$(VBOOT_HOSTLIB):
	printf "    MAKE       $(subst $(objutil)/,,$(@))\n"
	unset CFLAGS LDFLAGS; $(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$(VBOOT_HOST_BUILD) \
		CC="$(HOSTCC)" \
		PKG_CONFIG="true" \
		USE_FLASHROM=0 \
		V=$(V) \
		DEBUG= \
		hostlib

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

$(objutil)/cbfstool/%.o: $(top)/src/commonlib/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/src/commonlib/bsd/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lz4/lib/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) $(LZ4CFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/fpt_formats/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/bpdt_formats/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(TOOLCPPFLAGS) $(TOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/cbfstool: $(addprefix $(objutil)/cbfstool/,$(cbfsobj)) $(VBOOT_HOSTLIB)
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) -v $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cbfsobj)) $(VBOOT_HOSTLIB)

$(objutil)/cbfstool/fmaptool: $(addprefix $(objutil)/cbfstool/,$(fmapobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(fmapobj))

$(objutil)/cbfstool/rmodtool: $(addprefix $(objutil)/cbfstool/,$(rmodobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(rmodobj))

$(objutil)/cbfstool/ifwitool: $(addprefix $(objutil)/cbfstool/,$(ifwiobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(ifwiobj))

$(objutil)/cbfstool/ifittool: $(addprefix $(objutil)/cbfstool/,$(ifitobj)) $(VBOOT_HOSTLIB)
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(ifitobj)) $(VBOOT_HOSTLIB)

$(objutil)/cbfstool/cbfs-compression-tool: $(addprefix $(objutil)/cbfstool/,$(cbfscompobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cbfscompobj))

$(objutil)/cbfstool/amdcompress: $(addprefix $(objutil)/cbfstool/,$(amdcompobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(amdcompobj)) -lz

$(objutil)/cbfstool/elogtool: $(addprefix $(objutil)/cbfstool/,$(elogobj)) $(VBOOT_HOSTLIB)
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(elogobj)) $(VBOOT_HOSTLIB)

$(objutil)/cbfstool/cse_fpt: $(addprefix $(objutil)/cbfstool/,$(cse_fpt_obj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cse_fpt_obj))

$(objutil)/cbfstool/cse_serger: $(addprefix $(objutil)/cbfstool/,$(cse_serger_obj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(TOOLLDFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cse_serger_obj))

# Yacc source is superset of header
$(objutil)/cbfstool/fmd.o: TOOLCFLAGS += -Wno-redundant-decls
$(objutil)/cbfstool/fmd_parser.o: TOOLCFLAGS += -Wno-redundant-decls
# Lex generates unneeded functions and declarations
$(objutil)/cbfstool/fmd_scanner.o: TOOLCFLAGS += -Wno-redundant-decls
$(objutil)/cbfstool/fmd_scanner.o: TOOLCFLAGS += -Wno-unused-function
# Tolerate lzma sdk warnings
$(objutil)/cbfstool/LzmaEnc.o: TOOLCFLAGS += -Wno-sign-compare -Wno-cast-qual
# Tolerate commonlib warnings
$(objutil)/cbfstool/cbfs_private.o: TOOLCFLAGS += -Wno-sign-compare
# Tolerate lz4 warnings
$(objutil)/cbfstool/lz4.o: TOOLCFLAGS += -Wno-missing-prototypes
$(objutil)/cbfstool/lz4_wrapper.o: TOOLCFLAGS += -Wno-attributes

$(objutil)/cbfstool/fmd.o: $(objutil)/cbfstool/fmd_parser.h
$(objutil)/cbfstool/fmd.o: $(objutil)/cbfstool/fmd_scanner.h
$(objutil)/cbfstool/fmd_parser.o: $(objutil)/cbfstool/fmd.o
$(objutil)/cbfstool/fmd_scanner.o: $(objutil)/cbfstool/fmd.o
$(objutil)/cbfstool/fmd_scanner.o: TOOLCFLAGS += -Wno-sign-compare

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
