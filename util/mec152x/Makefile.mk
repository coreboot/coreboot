# SPDX-License-Identifier: BSD-3-Clause

mec152xtoolobj = main.o utils.o rom.o fmap.o kv_pair.o valstr.o
mec152xheader = $(addprefix $(dir)/,utils.h rom.h)

WERROR ?= -Wno-error
MEC152XCFLAGS := -Wno-array-bounds -Wextra -O3 -Wshadow $(WERROR)
MEC152XCFLAGS += -I $(top)/util/cbfstool/flashmap/
MEC152XCFLAGS += -I $(top)/util/mec152x
MEC152XCFLAGS += -I $(top)/src/commonlib/bsd/include

additional-dirs += $(objutil)/mec152x

$(objutil)/mec152x/%.o: $(top)/util/mec152x/%.c $(mec152xheader) | $(objutil)
	printf "    MEC152X   $@\n"
	$(HOSTCC) $(MEC152XCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/mec152x/%.o: $(top)/util/cbfstool/flashmap/%.c $(mec152xheader) | $(objutil)
	$(HOSTCC) $(MEC152XCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/mec152x/mec152xtool: $(addprefix $(objutil)/mec152x/,$(mec152xtoolobj)) $(mec152xheader) | $(objutil)
	printf "    MEC152X    $@\n"
	$(HOSTCC) $(addprefix $(objutil)/mec152x/,$(mec152xtoolobj)) $(LDFLAGS) -o $@
