top ?= $(abspath ../..)
objutil ?= $(top)/util

HOSTCC ?= $(CC)
VB_SOURCE ?= $(top)/3rdparty/vboot

.PHONY: all
all: $(objutil)/futility/futility

.PHONY: clean
clean:
	$(RM) $(objutil)/futility/futility
	$(RM) -r $(objutil)/futility/build

ifneq ($(V),1)
ifneq ($(Q),)
.SILENT:
endif
endif

include Makefile.inc
