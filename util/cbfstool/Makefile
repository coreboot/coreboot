top ?= $(abspath ../..)
objutil ?= $(top)/util
obj ?= $(objutil)/cbfsutil

CONFIG_FMD_GENPARSER := y

HOSTCC ?= $(CC)

.PHONY: all
all: $(objutil)/cbfstool/cbfstool \
	$(objutil)/cbfstool/fmaptool \
	$(objutil)/cbfstool/rmodtool \

.PHONY: clean
clean:
	$(RM) $(objutil)/cbfstool/cbfstool $(cbfsobj)
	$(RM) $(objutil)/cbfstool/fmaptool $(fmapobj)
	$(RM) $(objutil)/cbfstool/rmodtool $(rmodobj)

include Makefile.inc
