##
## Integration of nvramtool into the coreboot build system
##
## (C) 2005-2008 coresystems GmbH
## written by Stefan Reinauer <stepan@coresystems.de>
## (C) 2012 secunet Security Networks AG
##
## SPDX-License-Identifier: GPL-2.0-only

OS_ARCH        = $(shell uname)
NVRAMTOOLFLAGS := -I$(top)/util/nvramtool
ifeq ($(OS_ARCH), NetBSD)
NVRAMTOOLLDLFLAGS = -l$(shell uname -p)
endif
ifeq ($(shell uname -o 2>/dev/null), Cygwin)
NVRAMTOOLFLAGS += -O2 -g -Wall -W -D__GLIBC__
HOSTCFLAGS =
endif

nvramtoolobj :=
nvramtoolobj += cli/nvramtool.o cli/opts.o
nvramtoolobj += cmos_lowlevel.o cmos_ops.o common.o compute_ip_checksum.o
nvramtoolobj += hexdump.o input_file.o layout.o accessors/layout-common.o accessors/layout-text.o accessors/layout-bin.o lbtable.o
nvramtoolobj += reg_expr.o cbfs.o accessors/cmos-mem.o

ifeq ($(shell uname -s 2>/dev/null | cut -c-7), MINGW32)
NVRAMTOOLLDFLAGS += -lregex -lwsock32
nvramtoolobj += win32mmap.o
endif

additional-dirs += $(objutil)/nvramtool/accessors $(objutil)/nvramtool/cli

$(objutil)/nvramtool/%.o: $(top)/util/nvramtool/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(NVRAMTOOLFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/nvramtool/nvramtool: $(objutil)/nvramtool $(objutil)/nvramtool/accessors $(objutil)/nvramtool/cli $(addprefix $(objutil)/nvramtool/,$(nvramtoolobj))
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(NVRAMTOOLFLAGS) -o $@ $(addprefix $(objutil)/nvramtool/,$(nvramtoolobj)) $(NVRAMTOOLLDFLAGS)
