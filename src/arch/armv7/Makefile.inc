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
subdirs-y += boot/
subdirs-y += lib/

###############################################################################
# ARM specific options
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7),y)
CBFSTOOL_PRE1_OPTS = -m armv7 -b $(CONFIG_BOOTBLOCK_ROM_OFFSET) -H $(CONFIG_CBFS_HEADER_ROM_OFFSET) -o $(CONFIG_CBFS_ROM_OFFSET)
CBFSTOOL_PRE_OPTS = -b 0
endif

ifeq ($(CONFIG_ARCH_ARMV7),y)
stages_c = $(src)/arch/armv7/stages.c
stages_o = $(obj)/arch/armv7/stages.o

$(stages_o): $(stages_c) $(obj)/config.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_armv7) -I. $(CPPFLAGS_armv7) -c -o $@ $< -marm

endif # CONFIG_ARCH_ARMV7

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV7),y)

ifneq ($(CONFIG_ARM_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
bootblock-$(CONFIG_BOOTBLOCK_SIMPLE) += bootblock_simple.c
endif
bootblock-y += id.S
$(obj)/arch/arm/id.bootblock.o: $(obj)/build.h

bootblock-y += stages.c
bootblock-y += cache.c
bootblock-y += eabi_compat.c
bootblock-y += memset.S
bootblock-y += memcpy.S
bootblock-y += memmove.S
bootblock-y += mmu.c

$(objcbfs)/bootblock.debug: $(src)/arch/armv7/bootblock.ld $(obj)/ldoptions $$(bootblock-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_bootblock) -m armelf_linux_eabi -static -o $@ -L$(obj) $< -T $(src)/arch/armv7/bootblock.ld
else
	$(CC_bootblock) $(CFLAGS_bootblock) -nostartfiles -include $(obj)/config.h -static -o $@ -L$(obj) -T $(src)/arch/armv7/bootblock.ld -Wl,--start-group $(bootblock-objs) $(LIBGCC_FILE_NAME_bootblock) -Wl,--end-group
endif

endif # CONFIG_ARCH_BOOTBLOCK_ARMV7

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7),y)

romstage-y += stages.c
romstage-y += cache.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S

VBOOT_STUB_DEPS += $(obj)/arch/armv7/eabi_compat.rmodules_arm.o

$(objcbfs)/romstage.debug: $$(romstage-objs) $(src)/arch/armv7/romstage.ld $(obj)/ldoptions
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) $(romstage-objs) -T $(src)/arch/armv7/romstage.ld
else
	$(CC_romstage) $(CFLAGS_romstage) -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/armv7/romstage.ld -Wl,--start-group $(romstage-objs) $(LIBGCC_FILE_NAME_romstage) -Wl,--end-group
endif

endif # CONFIG_ARCH_ROMSTAGE_ARMV7

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV7),y)

ramstage-y += stages.c
ramstage-y += exception.c
ramstage-y += exception_asm.S
ramstage-y += div0.c
ramstage-y += cache.c
ramstage-y += cpu.c
ramstage-y += mmu.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S
ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

$(objcbfs)/ramstage.debug: $$(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) $(src)/arch/armv7/ramstage.ld $(obj)/ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m armelf_linux_eabi -o $@ -L$(obj) $< -T $(src)/arch/armv7/ramstage.ld
else
	$(CC_ramstage) $(CFLAGS_ramstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -Wl,--start-group $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) -Wl,--end-group -T $(src)/arch/armv7/ramstage.ld
endif

$(objgenerated)/ramstage.o: $(stages_o) $$(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m -m armelf_linux_eabi -r -o $@ --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --wrap __uidiv --start-group $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) --end-group
else
	$(CC_ramstage) $(CFLAGS_ramstage) $(CPPFLAGS_ramstage) -nostdlib -r -o $@ -Wl,--start-group $(stages_o) $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) -Wl,--end-group
endif

endif # CONFIG_ARCH_RAMSTAGE_ARMV7
