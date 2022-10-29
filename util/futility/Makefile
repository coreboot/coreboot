## SPDX-License-Identifier: GPL-2.0-only
top ?= $(abspath ../..)
objutil ?= $(top)/util
RM ?= rm

HOSTCC ?= $(CC)
VBOOT_SOURCE ?= $(top)/3rdparty/vboot
VBOOT_HOST_BUILD ?= $(abspath $(objutil)/vboot_lib)

.PHONY: all
all: $(objutil)/futility/futility

.PHONY: clean
clean:
	$(RM) -f $(objutil)/futility/futility
	$(RM) -rf $(objutil)/futility/build

distclean: clean

ifneq ($(V),1)
ifneq ($(Q),)
.SILENT:
endif
endif

include Makefile.inc
