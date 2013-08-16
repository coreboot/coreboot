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

bootblock-y += cache.c
bootblock-y += eabi_compat.c
bootblock-y += memset.S
bootblock-y += memcpy.S
bootblock-y += memmove.S

bootblock_lds = $(src)/arch/armv7/bootblock.lds
bootblock_lds += $(chipset_bootblock_lds)

bootblock_inc += $(src)/arch/armv7/bootblock.inc
bootblock_inc += $(src)/arch/armv7/id.inc
bootblock_inc += $(chipset_bootblock_inc)
bootblock_inc += $(objgenerated)/bootblock.inc

bootblock_custom = $(src)/$(call strip_quotes,$(CONFIG_BOOTBLOCK_CPU_INIT))
bootblock_custom += $(src)/$(call strip_quotes,$(CONFIG_BOOTBLOCK_MAINBOARD_INIT))

$(objgenerated)/bootblock.ld: $$(bootblock_lds) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach ldscript,ldoptions $(bootblock_lds),INCLUDE "$(ldscript)"\n)' > $@

$(objgenerated)/bootblock_inc.S: $$(bootblock_inc)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(bootblock_inc),#include "$(crt0)"\n)' > $@

$(objgenerated)/bootblock.o: $(objgenerated)/bootblock.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CPPFLAGS_bootblock) -Wa,-acdlns -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/bootblock.s: $(objgenerated)/bootblock_inc.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CPPFLAGS_bootblock) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/armv7/include -I$(obj) -include $(obj)/build.h -include $(obj)/config.h -I. -I$(src) $< -o $@

$(objgenerated)/bootblock.inc: $(src)/arch/armv7/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(bootblock_custom) $(OPTION_TABLE_H) $(obj)/config.h
	@printf "    CC      $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CFLAGS_bootblock) $(CPPFLAGS_bootblock) -MM \
		-MT$(objgenerated)/bootblock.inc \
		$< > $(objgenerated)/bootblock.inc.d
	$(CC_bootblock) -c -S $(CFLAGS_bootblock) -I. $(CPPFLAGS_bootblock) $< -o $@

$(objcbfs)/bootblock.debug:  $(objgenerated)/bootblock.o $(objgenerated)/bootblock.ld $$(bootblock-objs) $(stages) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_bootblock) -m armelf_linux_eabi -include $(obj)/config.h -static -o $@.tmp -L$(obj) $< -T $(objgenerated)/bootblock.ld
else
	$(CC_bootblock) -nostdlib -nostartfiles -include $(obj)/config.h -static -o $@ -L$(obj) -T $(objgenerated)/bootblock.ld -Wl,--start-group $(objgenerated)/bootblock.o $(bootblock-objs) $(stages) $(LIBGCC_FILE_NAME_bootblock) -Wl,--end-group
endif

endif # CONFIG_ARCH_BOOTBLOCK_ARMV7

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7),y)

romstage-y += cache.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S
romstage-srcs += $(objgenerated)/crt0.s

ldscripts =
ldscripts += $(src)/arch/armv7/romstage.ld

#crt0s += $(src)/cpu/arm/fpu_enable.inc
# FIXME: CONFIG_NEON or something similar for ARM?
#ifeq ($(CONFIG_SSE),y)
#crt0s += $(src)/cpu/arm/sse_enable.inc
#endif

crt0s += $(cpu_incs)
crt0s += $(cpu_incs-y)

$(objcbfs)/romstage.debug: $$(romstage-objs) $(stages_o) $(objgenerated)/romstage.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) $(romstage-objs) -T $(objgenerated)/romstage.ld
else
	$(CC_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/romstage.ld -Wl,--start-group $(romstage-objs) $(stages_o) $(LIBGCC_FILE_NAME_romstage) -Wl,--end-group
endif

$(objgenerated)/romstage.ld: $$(ldscripts) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	printf '$(foreach ldscript,ldoptions $(ldscripts),INCLUDE "$(ldscript:$(obj)/%=%)"\n)' >> $@.tmp
	mv $@.tmp $@

$(objgenerated)/crt0.romstage.S: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(objgenerated)/crt0.romstage.o: $(objgenerated)/crt0.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_romstage) -Wa,-acdlns -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/crt0.s: $(objgenerated)/crt0.romstage.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_romstage) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/armv7/include -I$(obj) -include $(obj)/config.h -include $(obj)/build.h -I. -I$(src) $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	@printf "    CC         romstage.inc\n"
	$(CC_romstage) -MMD $(CFLAGS_romstage) $(CPPFLAGS_romstage) -D__PRE_RAM__ -I$(src) -I. -I$(obj) -c -S $< -o $@

endif # CONFIG_ARCH_ROMSTAGE_ARMV7

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV7),y)

ramstage-y += exception.c
ramstage-y += exception_asm.S
ramstage-y += div0.c
#ramstage-y += interrupts.c
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

$(objcbfs)/ramstage.debug: $(objgenerated)/ramstage.o $(src)/arch/armv7/ramstage.ld
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m armelf_linux_eabi -o $@ -L$(obj) $< -T $(src)/arch/armv7/ramstage.ld
else
	$(CC_ramstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/armv7/ramstage.ld $<
endif

$(objgenerated)/ramstage.o: $(stages_o) $$(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m -m armelf_linux_eabi -r -o $@ --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --wrap __uidiv --start-group $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) --end-group
else
	$(CC_ramstage) $(CFLAGS_ramstage) $(CPPFLAGS_ramstage) -nostdlib -r -o $@ -Wl,--start-group $(stages_o) $(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) -Wl,--end-group
endif

ifeq ($(CONFIG_GENERATE_PIRQ_TABLE),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/irq_tables.c
endif
ifeq ($(CONFIG_BOARD_HAS_HARD_RESET),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/reset.c
endif
ifeq ($(CONFIG_GENERATE_ACPI_TABLES),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/dsdt.asl
# make doesn't have arithmetic operators or greater-than comparisons
ifeq ($(subst 5,4,$(CONFIG_ACPI_SSDTX_NUM)),4)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt2.asl
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt3.asl
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt4.asl
endif
ifeq ($(CONFIG_ACPI_SSDTX_NUM),5)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt5.asl
endif
endif

ifeq ($(CONFIG_HAVE_BUS_CONFIG),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c
endif

endif # CONFIG_ARCH_RAMSTAGE_ARMV7
