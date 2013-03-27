##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

romstage-y += chromeos.c
ramstage-y += chromeos.c
romstage-$(CONFIG_ARCH_X86) += vbnv.c
ramstage-$(CONFIG_ARCH_X86) += vbnv.c
romstage-$(CONFIG_ARCH_X86) += vboot.c
ramstage-y += gnvs.c
romstage-y += fmap.c
ramstage-y += fmap.c
smm-y += fmap.c
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/chromeos.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/chromeos.c
romstage-srcs += src/mainboard/$(MAINBOARDDIR)/chromeos.c
endif

ifeq ($(MOCK_TPM),1)
CFLAGS += -DMOCK_TPM=1
else
CFLAGS += -DMOCK_TPM=0
endif

ifeq ($(CONFIG_VBOOT_VERIFY_FIRMWARE),y)
romstage-y += vboot_loader.c
rmodules-y += vboot_wrapper.c

VB_LIB = $(obj)/external/vboot_reference/vboot_fw.a
VB_FIRMWARE_ARCH := $(ARCHDIR-y)
VB_SOURCE := vboot_reference

# Add the vboot include paths.
VB_INCLUDES += -I$(VB_SOURCE)/firmware/include
INCLUDES += $(VB_INCLUDES)

VBOOT_STUB_ELF = $(obj)/vendorcode/google/chromeos/vbootstub.elf
VBOOT_STUB_DOTO = $(VBOOT_STUB_ELF:.elf=.o)

# Dependency for the vboot rmodules. Ordering matters.
VBOOT_STUB_DEPS += $(obj)/vendorcode/google/chromeos/vboot_wrapper.rmodules.o
VBOOT_STUB_DEPS += $(obj)/lib/memcmp.rmodules.o
VBOOT_STUB_DEPS += $(obj)/arch/x86/lib/memset.rmodules.o
VBOOT_STUB_DEPS += $(obj)/arch/x86/lib/memcpy.rmodules.o
VBOOT_STUB_DEPS += $(VB_LIB)
# Remove the '-include' option since that will break vboot's build and ensure
# vboot_reference can get to coreboot's include files.
VBOOT_CFLAGS += $(patsubst -I%,-I../%,$(filter-out -include $(src)/include/kconfig.h, $(CFLAGS)))
VBOOT_CFLAGS += -DVBOOT_DEBUG

$(VBOOT_STUB_DOTO): $(VBOOT_STUB_DEPS)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ $^

# Link the vbootstub module with a 64KiB-byte heap.
$(eval $(call rmodule_link,$(VBOOT_STUB_ELF), $(VBOOT_STUB_DOTO), 0x10000))

# Build vboot library without the default includes from coreboot proper.
$(VB_LIB):
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	$(Q)FIRMWARE_ARCH=$(VB_FIRMWARE_ARCH) \
		CFLAGS="$(VBOOT_CFLAGS)" \
		make -C $(VB_SOURCE) \
		BUILD=../$(dir $(VB_LIB)) \
		V=$(V) \
		fwlib

endif
