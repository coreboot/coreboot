################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2012 The ChromiumOS Authors
## Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
## Copyright (C) 2009-2010 coresystems GmbH
## Copyright (C) 2009 Ronald G. Minnich
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
###############################################################################
# Take care of subdirectories
###############################################################################
subdirs-y += armv4/ armv7/

###############################################################################
# ARM specific options
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
CBFSTOOL_PRE1_OPTS = -m arm -b $(CONFIG_BOOTBLOCK_ROM_OFFSET) -H $(CONFIG_CBFS_HEADER_ROM_OFFSET) -o $(CONFIG_CBFS_ROM_OFFSET)
CBFSTOOL_PRE_OPTS = -b 0
endif

ifeq ($(CONFIG_ARCH_ARM),y)
stages_c = $(src)/arch/arm/stages.c
stages_o = $(obj)/arch/arm/stages.o

$(stages_o): $(stages_c) $(obj)/config.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_arm) -I. $(CPPFLAGS_arm) -c -o $@ $< -marm

endif # CONFIG_ARCH_ARM

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM),y)

bootblock-y += id.S
$(obj)/arch/arm/id.bootblock.o: $(obj)/build.h

bootblock-y += stages.c
bootblock-y += eabi_compat.c
bootblock-y += memset.S
bootblock-y += memcpy.S
bootblock-y += memmove.S

$(objcbfs)/bootblock.debug: $(src)/arch/arm/bootblock.ld $(obj)/ldoptions $$(bootblock-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_bootblock) -m armelf_linux_eabi --gc-sections -static -o $@ -L$(obj) $< -T $(src)/arch/arm/bootblock.ld
else
	$(CC_bootblock) $(CFLAGS_bootblock) -nostartfiles -Wl,--gc-sections -static -o $@ -L$(obj) -T $(src)/arch/arm/bootblock.ld -Wl,--start-group $(bootblock-objs) $(LIBGCC_FILE_NAME_bootblock) -Wl,--end-group
endif

endif # CONFIG_ARCH_BOOTBLOCK_ARM

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)

romstage-y += stages.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S

VBOOT_STUB_DEPS += $(obj)/arch/arm/eabi_compat.rmodules_arm.o

$(objcbfs)/romstage.debug: $$(romstage-objs) $(src)/arch/arm/romstage.ld $(obj)/ldoptions
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_romstage) -nostdlib -nostartfiles --gc-sections -static -o $@ -L$(obj) $(romstage-objs) -T $(src)/arch/arm/romstage.ld
else
	$(CC_romstage) $(CFLAGS_romstage) -nostartfiles -Wl,--gc-sections -static -o $@ -L$(obj) -T $(src)/arch/arm/romstage.ld -Wl,--start-group $(romstage-objs) $(LIBGCC_FILE_NAME_romstage) -Wl,--end-group
endif

endif # CONFIG_ARCH_ROMSTAGE_ARM

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)

ramstage-y += stages.c
ramstage-y += div0.c
ramstage-y += cpu.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S
ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

$(objcbfs)/ramstage.debug: $$(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) $(src)/arch/arm/ramstage.ld $(obj)/ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m armelf_linux_eabi -o $@ -L$(obj) $< -T $(src)/arch/arm/ramstage.ld
else
	$(CC_ramstage) $(CFLAGS_ramstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -Wl,--start-group $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) -Wl,--end-group -T $(src)/arch/arm/ramstage.ld
endif

$(objgenerated)/ramstage.o: $(stages_o) $$(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m -m armelf_linux_eabi --gc-sections -r -o $@ --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --wrap __uidiv --start-group $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) --end-group
else
	$(CC_ramstage) $(CFLAGS_ramstage) $(CPPFLAGS_ramstage) -nostdlib -Wl,--gc-sections -r -o $@ -Wl,--start-group $(stages_o) $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) -Wl,--end-group
endif

endif # CONFIG_ARCH_RAMSTAGE_ARM
