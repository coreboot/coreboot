#*****************************************************************************
#
# Copyright (c) 2012, Advanced Micro Devices, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Advanced Micro Devices, Inc. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

# AGESA V5 Files
AGESA_ROOT = src/vendorcode/amd/agesa/f16kb

AGESA_AUTOINCLUDES := $(shell find $(AGESA_ROOT)/Proc -type d -exec echo -n "-I"{}" " \;)

AGESA_INC  = -I$(src)/vendorcode/amd/include
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/../common
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(src)/mainboard/$(MAINBOARDDIR)	# OptionsIds.h

BUILDOPTS_INCLUDES = -I$(AGESA_ROOT)/Config $(AGESA_INC) $(AGESA_AUTOINCLUDES)

CFLAGS_x86_32 += -march=k8-sse3 -mtune=k8-sse3 -fno-zero-initialized-in-bss -fno-strict-aliasing
CFLAGS_x86_64 += -march=k8-sse3 -mtune=k8-sse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

# These are invalid, coreboot proper should not require
# use of AGESA internal header files.
CPPFLAGS_x86_ANY =
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/CPU			# heapManager.h
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/CPU/Family

CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/Fch			# FchPlatform.h
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/Fch/Common		# FchCommonCfg.h
CPPFLAGS_x86_ANY += -I$(AGESA_ROOT)/Proc/Common			# AmdFch.h

CPPFLAGS_x86_32 += $(AGESA_INC) $(CPPFLAGS_x86_ANY)
CPPFLAGS_x86_64 += $(AGESA_INC) $(CPPFLAGS_x86_ANY)

#######################################################################

subdirs-y += Legacy/Proc
subdirs-y += $(dir $(shell cd $(dir); find Proc -name Makefile.inc))

$(obj)/libagesa.fam16kb.a: $$(libagesa-objs)
	@printf "    AGESA        $(subst $(obj)/,,$(@))\n"
	$(AR_libagesa) rcsDT $@ $+

romstage-libs  += $(obj)/libagesa.fam16kb.a
ramstage-libs  += $(obj)/libagesa.fam16kb.a
