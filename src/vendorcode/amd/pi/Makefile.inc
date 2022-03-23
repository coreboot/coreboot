#*****************************************************************************
#
# Copyright (c) 2012, 2016   Advanced Micro Devices, Inc.
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

subdirs-y += 00670F00

ifeq ($(CONFIG_CPU_AMD_PI_00730F01),y)
# AGESA V5 Files

AGESA_ROOT = $(call strip_quotes,$(CONFIG_AGESA_BINARY_PI_VENDORCODE_PATH))

AGESA_INC = -I$(obj) -I$(src)/vendorcode/amd/include

AGESA_INC += -I$(src)/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)/binaryPI
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(src)/vendorcode/amd/pi
AGESA_INC += -I$(src)/vendorcode/amd/pi/Lib

AGESA_INC += -I$(AGESA_ROOT)/Proc
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/Fch
AGESA_INC += -I$(AGESA_ROOT)/Proc/Fch/Common

AGESA_INC += -I$(src)/northbridge/amd/pi
AGESA_INC += -I$(src)/southbridge/amd/pi/hudson

AGESA_INC += -I$(src)/arch/x86/include
AGESA_INC += -I$(src)/include
AGESA_INC += -I$(src)/commonlib/include
AGESA_INC += -I$(src)/commonlib/bsd/include
AGESA_INC += -I$(VBOOT_SOURCE)/firmware/include

AGESA_CFLAGS += -march=amdfam10 -mno-3dnow
AGESA_CFLAGS += -Wno-pragma-pack
AGESA_CFLAGS += -fno-strict-aliasing -D__LIBAGESA__
CFLAGS_x86_32 += $(AGESA_CFLAGS)
CFLAGS_x86_64 += $(AGESA_CFLAGS)

export AGESA_ROOT := $(AGESA_ROOT)
export AGESA_INC  := $(AGESA_INC)
export AGESA_CFLAGS  := $(AGESA_CFLAGS)

CC_bootblock := $(CC_bootblock) $(AGESA_INC) $(AGESA_CFLAGS)
CC_romstage := $(CC_romstage) $(AGESA_INC) $(AGESA_CFLAGS)
CC_postcar:= $(CC_postcar) -I$(src)/southbridge/amd/pi/hudson -I$(AGESA_ROOT)/binaryPI
CC_ramstage := $(CC_ramstage) $(AGESA_INC) $(AGESA_CFLAGS)

CC_x86_32 := $(CC_x86_32) $(AGESA_INC) $(AGESA_CFLAGS)
CC_x86_64 := $(CC_x86_64) $(AGESA_INC) $(AGESA_CFLAGS)

#######################################################################

define create_agesa_cp_template
# $1 AGESA source file

$(agesa_src_path)/$(notdir $1): $1
	@printf "    AGESA      Copying $$(notdir $1) => $$(@D)\n"
	if [ ! -r $(agesa_src_path)/$(notdir $1) ]; then                   \
		cp -f $1 $$(@D);                                           \
	fi

$(call src-to-obj,libagesa,$1): $(agesa_src_path)/$(notdir $1) $(obj)/config.h $(src)/include/kconfig.h
	@printf "    CC         $$(subst $(obj)/,,$$(@))\n"
	$(CC_libagesa) -c -MMD $(CFLAGS_libagesa) $(AGESA_CFLAGS)                     \
	               $(AGESA_INC)                                                   \
	               -include $(src)/include/kconfig.h                              \
	               -include $(src)/include/rules.h                                \
	               -include $(src)/commonlib/bsd/include/commonlib/bsd/compiler.h \
	               -o $$@                                                         \
	               $(agesa_src_path)/$(notdir $1)

endef

agesa_raw_files += $(wildcard $(src)/vendorcode/amd/pi/Lib/*.[cS])

ifeq ($(CONFIG_HUDSON_IMC_FWM),y)
agesa_raw_files += $(wildcard $(src)/vendorcode/amd/pi/Lib/imc/*.c)
endif

classes-$(CONFIG_CPU_AMD_AGESA_BINARY_PI) += libagesa

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
$(eval $(call create_class_compiler,libagesa,x86_32))
else
$(eval $(call create_class_compiler,libagesa,x86_64))
endif

agesa_src_files := $(strip $(sort $(foreach file,$(strip $(agesa_raw_files)),$(call strip_quotes,$(file)))))
agesa_output_path := $(obj)/libagesa
agesa_src_path := $(agesa_output_path)
agesa_dirs := $(sort $(abspath $(dir $(call src-to-obj,libagesa,$(agesa_src_files)))))

additional-dirs += $(agesa_src_path) $(agesa_dirs)

$(foreach file,$(strip $(agesa_src_files)),$(eval $(call create_agesa_cp_template,$(file))))

$(agesa_output_path)/libagesa.a: $(call src-to-obj,libagesa,$(agesa_src_files))
	@printf "    AGESA      $(subst $(obj)/,,$(@))\n"
	$(AR_libagesa) rcsDT $@ $+

bootblock-libs += $(agesa_output_path)/libagesa.a
romstage-libs += $(agesa_output_path)/libagesa.a
ramstage-libs += $(agesa_output_path)/libagesa.a

#######################################################################

cbfs-files-$(CONFIG_CPU_AMD_AGESA_BINARY_PI) += $(CONFIG_AGESA_CBFS_NAME)
$(CONFIG_AGESA_CBFS_NAME)-file := $(CONFIG_AGESA_BINARY_PI_FILE)
$(CONFIG_AGESA_CBFS_NAME)-type := raw
$(CONFIG_AGESA_CBFS_NAME)-position := $(CONFIG_AGESA_BINARY_PI_LOCATION)

endif
