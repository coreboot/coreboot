top ?= $(abspath ../..)
objutil ?= $(top)/util

CONFIG_FMD_GENPARSER := y

HOSTCC ?= $(CC)

.PHONY: all
all: $(objutil)/cbfstool/cbfstool \
	$(objutil)/cbfstool/fmaptool \
	$(objutil)/cbfstool/rmodtool \

.PHONY: clean
clean:
	$(RM) fmd_parser.c fmd_parser.h fmd_scanner.c fmd_scanner.h
	$(RM) $(objutil)/cbfstool/cbfstool $(cbfsobj)
	$(RM) $(objutil)/cbfstool/fmaptool $(fmapobj)
	$(RM) $(objutil)/cbfstool/rmodtool $(rmodobj)

include Makefile.inc
