#
# This file is part of the coreboot project.
#
# Copyright (C) 2012 Advanced Micro Devices, Inc.
# Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#


## ABSOLUTE AGESA V5 ROOT PATH ##
AGESA_ROOT ?= $(PWD)
AGESA_ROOT = src/vendorcode/amd/agesa/f15

AGESA_AUTOINCLUDES := $(shell find $(AGESA_ROOT)/Proc -type d -exec echo -n "-I"{}" " \;)

AGESA_INC  = -I$(src)/vendorcode/amd/include
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/../common
AGESA_INC += -I$(AGESA_ROOT)/Include

BUILDOPTS_INCLUDES = -I$(AGESA_ROOT)/Config $(AGESA_INC) $(AGESA_AUTOINCLUDES)

## AGESA need sse feature ##
CFLAGS_x86_32 += -msse3 -fno-zero-initialized-in-bss -fno-strict-aliasing
CFLAGS_x86_64 += -msse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

# These are invalid, coreboot proper should not require
# use of AGESA internal header files.
CPPFLAGS_x86_ANY =
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/CPU			# heapManager.h
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/CPU/Family
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/Common			# CreateStruct.h

CPPFLAGS_x86_32 += $(AGESA_INC) $(CPPFLAGS_x86_ANY)
CPPFLAGS_x86_64 += $(AGESA_INC) $(CPPFLAGS_x86_ANY)

#######################################################################

subdirs-y += Legacy/Proc
subdirs-y += $(dir $(shell cd $(dir); find Proc -name Makefile.inc))

$(obj)/libagesa.fam15.a: $$(libagesa-objs)
	@printf "    AGESA        $(subst $(obj)/,,$(@))\n"
	ar rcs $@ $+

romstage-libs  += $(obj)/libagesa.fam15.a
ramstage-libs  += $(obj)/libagesa.fam15.a
