# SPDX-License-Identifier: BSD-3-Clause

ifdtoolobj = ifdtool.o fmap.o kv_pair.o valstr.o

IFDTOOLCFLAGS  = -O2 -g -Wall -Wextra -Wmissing-prototypes -Werror
IFDTOOLCFLAGS += -I$(top)/src/commonlib/include -I$(top)/src/commonlib/bsd/include
IFDTOOLCFLAGS += -I$(top)/util/cbfstool/flashmap
IFDTOOLCFLAGS += -include $(top)/src/commonlib/bsd/include/commonlib/bsd/compiler.h
IFDTOOLCFLAGS += -D_DEFAULT_SOURCE # for endianness converting functions

$(objutil)/ifdtool/%.o: $(top)/util/ifdtool/%.c
	$(HOSTCC) $(IFDTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/ifdtool/%.o: $(top)/util/cbfstool/flashmap/%.c
	$(HOSTCC) $(IFDTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/ifdtool/ifdtool: $(addprefix $(objutil)/ifdtool/,$(ifdtoolobj))
	printf "   IFDTOOL\n"
	$(HOSTCC) ${LDFLAGS} $(addprefix $(objutil)/ifdtool/,$(ifdtoolobj)) -o $@
