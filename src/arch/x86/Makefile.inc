################################################################################
##
## This file is part of the coreboot project.
##
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
subdirs-y += boot
# subdirs-y += init
subdirs-y += lib
subdirs-y += smp

ifeq ($(CONFIG_COMPILER_GCC),y)
DISASSEMBLY=-Wa,-acdlns
else
DISASSEMBLY=
endif

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)
cbfs-files-y += cmos_layout.bin
cmos_layout.bin-file = $(obj)/cmos_layout.bin
cmos_layout.bin-type = 0x01aa

OPTION_TABLE_H:=$(obj)/option_table.h
endif

stripped_vgabios_id = $(call strip_quotes,$(CONFIG_VGA_BIOS_ID))
cbfs-files-$(CONFIG_VGA_BIOS) += pci$(stripped_vgabios_id).rom
pci$(stripped_vgabios_id).rom-file := $(call strip_quotes,$(CONFIG_VGA_BIOS_FILE))
pci$(stripped_vgabios_id).rom-type := optionrom

cbfs-files-$(CONFIG_INTEL_MBI) += mbi.bin
mbi.bin-file := $(call strip_quotes,$(CONFIG_MBI_FILE))
mbi.bin-type := mbi

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
CBFSTOOL_PRE1_OPTS = -m x86 -o $$(( $(CONFIG_ROM_SIZE) - $(CONFIG_CBFS_SIZE) ))
CBFSTOOL_PRE_OPTS  = -b $(shell cat $(objcbfs)/base_xip.txt)
endif

################################################################################
# i386 specific tools
NVRAMTOOL:=$(objutil)/nvramtool/nvramtool

$(OPTION_TABLE_H): $(NVRAMTOOL) $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(NVRAMTOOL) -y $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout -H $@

$(obj)/cmos_layout.bin: $(NVRAMTOOL) $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(NVRAMTOOL) -y $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout -L $@


###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32),y)

bootblock_lds = $(src)/arch/x86/init/ldscript_failover.lb
bootblock_lds += $(src)/cpu/x86/16bit/entry16.lds
bootblock_lds += $(src)/cpu/x86/16bit/reset16.lds
bootblock_lds += $(src)/arch/x86/lib/id.lds
bootblock_lds += $(chipset_bootblock_lds)
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
bootblock_lds += $(src)/cpu/intel/fit/fit.lds
endif

bootblock_inc = $(src)/arch/x86/init/prologue.inc
bootblock_inc += $(src)/cpu/x86/16bit/entry16.inc
bootblock_inc += $(src)/cpu/x86/16bit/reset16.inc
bootblock_inc += $(src)/cpu/x86/32bit/entry32.inc
bootblock_inc += $(src)/arch/x86/lib/id.inc
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
bootblock_inc += $(src)/cpu/intel/fit/fit.inc
endif
bootblock_inc += $(chipset_bootblock_inc)

ifeq ($(CONFIG_SSE),y)
bootblock_inc += $(src)/cpu/x86/sse_enable.inc
endif
bootblock_inc += $(objgenerated)/bootblock.inc
bootblock_inc += $(src)/arch/x86/lib/walkcbfs.S

bootblock_romccflags := -mcpu=i386 -O2 -D__PRE_RAM__ -D__BOOT_BLOCK__
ifeq ($(CONFIG_SSE),y)
bootblock_romccflags := -mcpu=k7 -msse -O2 -D__PRE_RAM__ -D__BOOT_BLOCK__
endif

$(objgenerated)/bootblock.ld: $$(bootblock_lds) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach ldscript,ldoptions $(bootblock_lds),INCLUDE "$(ldscript)"\n)' > $@

$(objgenerated)/bootblock_inc.S: $$(bootblock_inc)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(bootblock_inc),#include "$(crt0)"\n)' > $@

$(objgenerated)/bootblock.o: $(objgenerated)/bootblock.s
	@printf "    CC        $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(DISASSEMBLY) -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/bootblock.s: $(objgenerated)/bootblock_inc.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -include $(obj)/build.h -include $(obj)/config.h -I. -I$(src) $< -o $@

$(objgenerated)/bootblock.inc: $(src)/arch/x86/init/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(objutil)/romcc/romcc $(OPTION_TABLE_H)
	@printf "    ROMCC      $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(INCLUDES) $(INCLUDES_bootblock) -MM -MT$(objgenerated)/bootblock.inc \
		$< > $(objgenerated)/bootblock.inc.d
	$(ROMCC) -c -S $(bootblock_romccflags) -I. $(INCLUDES) $(INCLUDES_bootblock) $< -o $@

$(objcbfs)/bootblock.debug: $(objgenerated)/bootblock.o $(objgenerated)/bootblock.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_bootblock) -m elf_i386 -static -o $@ -L$(obj) $< -T $(objgenerated)/bootblock.ld
else
	$(CC_bootblock) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/bootblock.ld $<
endif


endif # CONFIG_ARCH_BOOTBLOCK_X86_32

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)

crt0s = $(src)/arch/x86/init/prologue.inc
ldscripts =
ldscripts += $(src)/arch/x86/init/romstage.ld
crt0s += $(src)/cpu/x86/32bit/entry32.inc
ldscripts += $(src)/cpu/x86/32bit/entry32.lds

crt0s += $(src)/cpu/x86/fpu_enable.inc
ifeq ($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_enable.inc
endif

crt0s += $(cpu_incs)
crt0s += $(cpu_incs-y)

crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

ifeq ($(CONFIG_ROMCC),y)
crt0s += $(src)/arch/x86/init/crt0_romcc_epilogue.inc
endif

ifeq ($(CONFIG_ROMCC),y)
ifeq ($(CONFIG_MMX),y)
	ifeq ($(CONFIG_SSE),y)
		ROMCCFLAGS := -mcpu=p4 -O2 # MMX, SSE
	else
		ROMCCFLAGS := -mcpu=p2 -O2 # MMX, !SSE
	endif
else
	ROMCCFLAGS := -mcpu=i386 -O2 # !MMX, !SSE
endif

$(objcbfs)/romstage_%.bin: $(objcbfs)/romstage_%.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_romstage) -O binary $< $@

$(objcbfs)/romstage_%.elf: $(objcbfs)/romstage_%.debug
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cp $< $@.tmp
	$(OBJCOPY_romstage) --strip-debug $@.tmp
	$(OBJCOPY_romstage) --add-gnu-debuglink=$< $@.tmp
	mv $@.tmp $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(objutil)/romcc/romcc $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	printf "    ROMCC      romstage.inc\n"
	$(ROMCC) -c -S $(ROMCCFLAGS) -D__PRE_RAM__ -I. $(INCLUDES) $(INCLUDES_romstage) $< -o $@
else

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	@printf "    CC         romstage.inc\n"
	$(CC_romstage) -MMD $(CFLAGS_romstage) -D__PRE_RAM__ -I$(src) -I. -I$(obj) -c -S $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc
	@printf "    POST       romstage.inc\n"
	sed -e 's/\.rodata/.rom.data/g' -e 's/\^\.text/.section .rom.text/g' \
		-e 's/\^\.section \.text/.section .rom.text/g' $^ > $@.tmp
	mv $@.tmp $@
endif

romstage-srcs += $(objgenerated)/crt0.s

romstage-libs ?=

$(objcbfs)/romstage_null.debug: $$(romstage-objs) $(objgenerated)/romstage_null.ld $$(romstage-libs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --start-group $(romstage-objs) $(romstage-libs) $(LIBGCC_FILE_NAME_romstage) --end-group -T $(objgenerated)/romstage_null.ld
else
	$(CC_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/romstage_null.ld -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(romstage-objs) $(romstage-libs) $(LIBGCC_FILE_NAME_romstage) -Wl,--end-group
endif
	$(NM_romstage) $@ | grep -q " [DdBb] "; if [ $$? -eq 0 ]; then \
		echo "Forbidden global variables in romstage:"; \
		$(NM_romstage) $@ | grep " [DdBb] "; test "$(CONFIG_CPU_AMD_AGESA)" = y; \
		else true; fi

$(objcbfs)/romstage_xip.debug: $$(romstage-objs) $(objgenerated)/romstage_xip.ld $$(romstage-libs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --start-group $(romstage-objs) $(romstage-libs) $(LIBGCC_FILE_NAME_romstage) --end-group -T $(objgenerated)/romstage_xip.ld
else
	$(CC_romstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/romstage_xip.ld -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(romstage-objs) $(romstage-libs) $(LIBGCC_FILE_NAME_romstage) -Wl,--end-group
endif

$(objgenerated)/romstage_null.ld: $$(ldscripts) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	printf "ROMSTAGE_BASE = 0x0;\n" > $@.tmp
	printf '$(foreach ldscript,ldoptions $(ldscripts),INCLUDE "$(ldscript:$(obj)/%=%)"\n)' >> $@.tmp
	mv $@.tmp $@

$(objgenerated)/romstage_xip.ld: $(objgenerated)/romstage_null.ld $(objcbfs)/base_xip.txt
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	sed -e 's/^/ROMSTAGE_BASE = /g' -e 's/$$/;/g' $(objcbfs)/base_xip.txt > $@.tmp
	sed -e '/ROMSTAGE_BASE/d' $(objgenerated)/romstage_null.ld >> $@.tmp
	mv $@.tmp $@

$(objcbfs)/base_xip.txt: $(obj)/coreboot.pre1 $(objcbfs)/romstage_null.bin
	rm -f $@
	$(CBFSTOOL) $(obj)/coreboot.pre1 locate -T -f $(objcbfs)/romstage_null.bin -n $(CONFIG_CBFS_PREFIX)/romstage -P $(CONFIG_XIP_ROM_SIZE) > $@.tmp \
	 || { echo "The romstage is larger than XIP size. Please expand the CONFIG_XIP_ROM_SIZE" ; exit 1; }
	mv $@.tmp $@

$(objgenerated)/crt0.romstage.S: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(objgenerated)/crt0.romstage.o: $(objgenerated)/crt0.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_romstage) $(DISASSEMBLY) -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/crt0.s: $(objgenerated)/crt0.romstage.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_romstage) $(INCLUDES) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -include $(obj)/config.h -include $(obj)/build.h -I. -I$(src) $< -o $@

endif # CONFIG_ARCH_ROMSTAGE_X86_32

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)

ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)
ifeq ($(CONFIG_GENERATE_MP_TABLE),y)
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/mptable.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mptable.c
endif
endif
ifeq ($(CONFIG_GENERATE_PIRQ_TABLE),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/irq_tables.c
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/reset.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/reset.c
endif
ifeq ($(CONFIG_GENERATE_ACPI_TABLES),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/dsdt.asl
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/ssdt2.asl),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt2.asl
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/ssdt3.asl),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt3.asl
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/ssdt4.asl),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt4.asl
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/ssdt5.asl),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt5.asl
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/fadt.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/fadt.c
endif
endif
ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/smihandler.c),)
smm-srcs += src/mainboard/$(MAINBOARDDIR)/smihandler.c
endif
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c
endif

ramstage-libs ?=

$(eval $(call create_class_compiler,rmodules,x86_32))
ifeq ($(CONFIG_RELOCATABLE_RAMSTAGE),y)

$(eval $(call rmodule_link,$(objcbfs)/ramstage.debug, $(objgenerated)/ramstage.o, $(CONFIG_HEAP_SIZE)))

# The rmodule_link defintion creates an elf file with .rmod extension.
$(objcbfs)/ramstage.elf: $(objcbfs)/ramstage.debug.rmod
	cp $< $@

else

$(objcbfs)/ramstage.debug: $(objgenerated)/ramstage.o $(src)/arch/x86/ramstage.ld
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m elf_i386 -o $@ -L$(obj) $< -T $(src)/arch/x86/ramstage.ld
else
	$(CC_ramstage) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/x86/ramstage.ld $<
endif

endif

$(objgenerated)/ramstage.o: $$(ramstage-objs) $(LIBGCC_FILE_NAME_ramstage) $$(ramstage-libs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD_ramstage) -m elf_i386 -r -o $@ --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --start-group $(ramstage-objs) $(ramstage-libs) $(LIBGCC_FILE_NAME_ramstage) --end-group
else
	$(CC_ramstage) $(CFLAGS_ramstage) -nostdlib -r -o $@ -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(ramstage-objs) $(ramstage-libs) $(LIBGCC_FILE_NAME_ramstage) -Wl,--end-group
endif

endif # CONFIG_ARCH_RAMSTAGE_X86_32

################################################################################

seabios:
	$(MAKE) -C payloads/external/SeaBIOS -f Makefile.inc \
			HOSTCC="$(HOSTCC)" \
			CC="$(CC_x86_32)" LD="$(LD_x86_32)" OBJDUMP="$(OBJDUMP_x86_32)" \
			OBJCOPY="$(OBJCOPY_x86_32)" STRIP="$(STRIP_x86_32)" \
			AS="$(AS_x86_32)" \
			CONFIG_SEABIOS_MASTER=$(CONFIG_SEABIOS_MASTER) \
			CONFIG_SEABIOS_STABLE=$(CONFIG_SEABIOS_STABLE) \
			CONFIG_SEABIOS_THREAD_OPTIONROMS=$(CONFIG_SEABIOS_THREAD_OPTIONROMS) \
			OUT=$(abspath $(obj)) IASL="$(IASL)"

filo:
	$(MAKE) -C payloads/external/FILO -f Makefile.inc \
			HOSTCC="$(HOSTCC)" \
			CC="$(CC_x86_32)" LD="$(LD_x86_32)" OBJDUMP="$(OBJDUMP_x86_32)" \
			OBJCOPY="$(OBJCOPY_x86_32)" STRIP="$(STRIP_x86_32)" \
			CONFIG_FILO_MASTER=$(CONFIG_FILO_MASTER) \
			CONFIG_FILO_STABLE=$(CONFIG_FILO_STABLE)


grub2:
	$(MAKE) -C payloads/external/GRUB2 -f Makefile.inc \
			HOSTCC="$(HOSTCC)" \
			CC="$(CC_x86_32)" LD="$(LD_x86_32)" OBJDUMP="$(OBJDUMP_x86_32)" \
			OBJCOPY="$(OBJCOPY_x86_32)" STRIP="$(STRIP_x86_32)" \
			CONFIG_GRUB2_MASTER=$(CONFIG_GRUB2_MASTER)
