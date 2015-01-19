#*****************************************************************************
#
# Copyright (c) 2012,        Advanced Micro Devices, Inc.
#               2013 - 2014, Sage Electronic Engineering, LLC
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
AGESA_ROOT = $(call strip_quotes,$(src)/../$(CONFIG_AGESA_BINARY_PI_PATH))

AGESA_INC = -I$(obj)

AGESA_INC += -I$(src)/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)/binaryPI
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(src)/vendorcode/amd/pi/00730F01
AGESA_INC += -I$(src)/vendorcode/amd/pi/00730F01/Lib
AGESA_INC += -I$(AGESA_ROOT)/Proc
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/Fch
AGESA_INC += -I$(AGESA_ROOT)/Proc/Fch/Common

AGESA_INC += -I$(src)/southbridge/amd/pi/hudson

AGESA_INC += -I$(src)/arch/x86/include
AGESA_INC += -I$(src)/include

AGESA_CFLAGS += -march=amdfam10 -mno-3dnow -fno-zero-initialized-in-bss -fno-strict-aliasing
CFLAGS_x86_32 += $(AGESA_CFLAGS)

export AGESA_ROOT := $(AGESA_ROOT)
export AGESA_INC  := $(AGESA_INC)
export AGESA_CFLAGS  := $(AGESA_CFLAGS)

CC_bootblock := $(CC_bootblock) $(AGESA_INC) $(AGESA_CFLAGS)
CC_romstage := $(CC_romstage) $(AGESA_INC) $(AGESA_CFLAGS)
CC_ramstage := $(CC_ramstage) $(AGESA_INC) $(AGESA_CFLAGS)
CC_x86_32 := $(CC_x86_32) $(AGESA_INC) $(AGESA_CFLAGS)

#######################################################################

define create_agesa_cp_template

# $1 AGESA source file
# $2 AGESA copy-to location
$(agesa_src_path)/$(notdir $2): $2 $(agesa_src_path)
	@printf "    AGESA      Copying $$(notdir $2) => $$(@D)\n"
	if [ ! -r $(agesa_src_path)/$(notdir $2) ]; then                   \
		cp -uf $2 $$(@D);                                                     \
	fi

$(agesa_obj_path)/$1.libagesa.o: $(agesa_src_path)/$(notdir $2) $(obj)/config.h $(src)/include/kconfig.h $(agesa_obj_path)
	@printf "    CC         $(subst $(agesa_obj_path)/,,$$(@))\n"
	$(CC_libagesa) -c -MMD $(CFLAGS_libagesa) $(AGESA_CFLAGS)                 \
	               $(AGESA_INC)                                               \
	               -include $(obj)/config.h -include $(src)/include/kconfig.h \
	               -o $$@                                                     \
	               $(agesa_src_path)/$(notdir $2)

endef

agesa_raw_files += $(wildcard $(src)/vendorcode/amd/pi/00730F01/Lib/*.[cS])
agesa_raw_files += $(wildcard $(AGESA_ROOT)/binaryPI/*.[cS])

classes-$(CONFIG_CPU_AMD_AGESA_BINARY_PI) += libagesa
$(eval $(call create_class_compiler,libagesa,x86_32))

agesa_src_files := $(strip $(sort $(foreach file,$(strip $(agesa_raw_files)),$(call strip_quotes,$(file)))))
agesa_obj_path := $(strip $(obj)/vendorcode/amd)
agesa_src_path := $(strip $(obj)/agesa)
agesa_src_copies := $(strip $(foreach file,$(agesa_src_files),$(agesa_obj_path)/$(notdir $(file))))
agesa_obj_copies := $(strip $(agesa_src_copies:.c=.libagesa.o))

$(agesa_src_path):
	mkdir -p $@

$(agesa_obj_path):
	mkdir -p $@

$(foreach file,$(strip $(agesa_src_files)),$(eval $(call create_agesa_cp_template,$(basename $(notdir $(file))),$(file))))

$(obj)/agesa/libagesa.00730F01.a: $(agesa_obj_copies)
	@printf "    AGESA      $(subst $(agesa_obj_path)/,,$(@))\n"
	ar rcs $@ $+

romstage-libs += $(obj)/agesa/libagesa.00730F01.a
ramstage-libs += $(obj)/agesa/libagesa.00730F01.a

#######################################################################

cbfs-files-$(CONFIG_CPU_AMD_AGESA_BINARY_PI) += AGESA
AGESA-file := $(CONFIG_AGESA_BINARY_PI_PATH)/$(CONFIG_AGESA_BINARY_PI_FILE)
AGESA-type := raw
AGESA-position := $(CONFIG_AGESA_BINARY_PI_LOCATION)
