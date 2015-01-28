##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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

verstage-generic-ccopts += -D__PRE_RAM__ -D__VERSTAGE__

bootblock-y += ../vboot_common.c
verstage-y += ../vboot_common.c
romstage-y += ../vboot_common.c
ramstage-y += ../vboot_common.c

bootblock-y += verstub.c
verstage-y += verstub.c
bootblock-y += common.c
verstage-y += verstage.c
verstage-y += antirollback.c common.c
romstage-y += vboot_handoff.c common.c

verstage-y += verstage.ld

VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-VERSTAGE-y))
VB2_LIB = $(obj)/external/vboot_reference/vboot_fw20.a
VBOOT_CFLAGS += $(patsubst -I%,-I$(top)/%,$(filter-out -include $(src)/include/kconfig.h, $(CFLAGS_verstage)))
VBOOT_CFLAGS += $(verstage-c-ccopts)
VBOOT_CFLAGS += -include $(top)/src/include/kconfig.h -Wno-missing-prototypes
VBOOT_CFLAGS += -DVBOOT_DEBUG

$(VB2_LIB): $(obj)/config.h
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	$(Q)FIRMWARE_ARCH=$(VB_FIRMWARE_ARCH) \
	CC="$(CC_verstage)" \
	CFLAGS="$(VBOOT_CFLAGS)" VBOOT2="y" \
	$(MAKE) -C $(VB_SOURCE) \
		BUILD=$(top)/$(dir $(VB2_LIB)) \
		V=$(V) \
		fwlib20

VERSTAGE_ELF = $(objcbfs)/verstage.elf
cbfs-files-y += $(call strip_quotes,$(CONFIG_CBFS_PREFIX))/verstage
fallback/verstage-file = $(VERSTAGE_ELF)
fallback/verstage-type = stage
fallback/verstage-compression = none
