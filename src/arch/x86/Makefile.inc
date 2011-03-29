##
## This file is part of the coreboot project.
##
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

#######################################################################
# Take care of subdirectories
subdirs-y += boot
# subdirs-y += init
subdirs-y += lib
subdirs-y += smp

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)
cbfs-files-y += cmos_layout.bin
cmos_layout.bin-file = $(obj)/cmos_layout.bin
cmos_layout.bin-type = 0x01aa

OPTION_TABLE_H:=$(obj)/option_table.h
endif

#######################################################################
# Build the final rom image
COREBOOT_ROM_DEPENDENCIES:=
ifeq ($(CONFIG_PAYLOAD_ELF),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_PAYLOAD_FILE)
endif
ifeq ($(CONFIG_PAYLOAD_SEABIOS),y)
COREBOOT_ROM_DEPENDENCIES+=seabios
endif
ifeq ($(CONFIG_AP_CODE_IN_CAR),y)
COREBOOT_ROM_DEPENDENCIES+=$(obj)/coreboot_ap
endif
ifeq ($(CONFIG_GEODE_VSA_FILE),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_VSA_FILENAME)
endif

extract_nth=$(word $(1), $(subst |, ,$(2)))

ifneq ($(CONFIG_UPDATE_IMAGE),y)
prebuild-files = \
	$(foreach file,$(cbfs-files), \
		$(CBFSTOOL) $@ add $(call extract_nth,1,$(file)) $(call extract_nth,2,$(file)) $(call extract_nth,3,$(file)) $(call extract_nth,4,$(file)); )
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))

$(obj)/coreboot.pre1: $(obj)/coreboot.bootblock $$(prebuilt-files) $(CBFSTOOL)
	rm -f $@
	$(CBFSTOOL) $@ create $(CONFIG_COREBOOT_ROMSIZE_KB)K $(obj)/coreboot.bootblock
	$(prebuild-files)
else
$(obj)/coreboot.pre1: $(CBFSTOOL)
	mv $(obj)/coreboot.rom $@
endif

$(obj)/coreboot.rom: $(obj)/coreboot.pre $(obj)/coreboot_ram $(CBFSTOOL) $(call strip_quotes,$(COREBOOT_ROM_DEPENDENCIES))
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
	cp $(obj)/coreboot.pre $@.tmp
	if [ -f $(obj)/coreboot_ap ]; \
	then \
		$(CBFSTOOL) $@.tmp add-stage $(obj)/coreboot_ap $(CONFIG_CBFS_PREFIX)/coreboot_ap $(CBFS_COMPRESS_FLAG); \
	fi
	$(CBFSTOOL) $@.tmp add-stage $(obj)/coreboot_ram $(CONFIG_CBFS_PREFIX)/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	@printf "    PAYLOAD    \e[1;31mnone (as specified by user)\e[0m\n"
endif
ifeq ($(CONFIG_PAYLOAD_ELF),y)
	@printf "    PAYLOAD    $(CONFIG_PAYLOAD_FILE) (compression: $(CBFS_PAYLOAD_COMPRESS_NAME))\n"
	$(CBFSTOOL) $@.tmp add-payload $(CONFIG_PAYLOAD_FILE) $(CONFIG_CBFS_PREFIX)/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_PAYLOAD_SEABIOS),y)
	@printf "    PAYLOAD    SeaBIOS (internal, compression: $(CBFS_PAYLOAD_COMPRESS_NAME))\n"
	$(CBFSTOOL) $@.tmp add-payload $(CONFIG_PAYLOAD_FILE) $(CONFIG_CBFS_PREFIX)/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_GEODE_VSA_FILE),y)
	@printf "    VSA        $(CONFIG_VSA_FILENAME)\n"
	$(OBJCOPY) --set-start 0x20 --adjust-vma 0x60000 -I binary -O elf32-i386 -B i386 $(CONFIG_VSA_FILENAME) $(obj)/vsa.o
	$(LD) -m elf_i386 -e 0x60020 --section-start .data=0x60000 $(obj)/vsa.o -o $(obj)/vsa.elf
	$(CBFSTOOL) $@.tmp add-stage $(obj)/vsa.elf vsa
endif
	mv $@.tmp $@
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ print

stripped_vgabios_id = $(call strip_quotes,$(CONFIG_VGA_BIOS_ID))
cbfs-files-$(CONFIG_VGA_BIOS) += pci$(stripped_vgabios_id).rom
pci$(stripped_vgabios_id).rom-file := $(call strip_quotes,$(CONFIG_VGA_BIOS_FILE))
pci$(stripped_vgabios_id).rom-type := optionrom

cbfs-files-$(CONFIG_INTEL_MBI) += mbi.bin
mbi.bin-file := $(call strip_quotes,$(CONFIG_MBI_FILE))
mbi.bin-type := mbi

cbfs-files-$(CONFIG_BOOTSPLASH) += bootsplash.jpg
bootsplash.jpg-file := $(call strip_quotes,$(CONFIG_BOOTSPLASH_FILE))
bootsplash.jpg-type := bootsplash

#######################################################################
# i386 specific tools

$(OPTION_TABLE_H): $(objutil)/options/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(objutil)/options/build_opt_tbl --config $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout --header $@

$(obj)/cmos_layout.bin: $(objutil)/options/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(objutil)/options/build_opt_tbl --config $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout --binary $@

$(objutil)/options/build_opt_tbl: $(top)/util/options/build_opt_tbl.c $(top)/src/include/pc80/mc146818rtc.h $(top)/src/include/boot/coreboot_tables.h
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) $< -o $@

#######################################################################
# Build the coreboot_ram (stage 2)

$(obj)/coreboot_ram: $(obj)/coreboot_ram.o $(src)/arch/x86/coreboot_ram.ld #ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/x86/coreboot_ram.ld $(obj)/coreboot_ram.o
	$(NM) -n $(obj)/coreboot_ram | sort > $(obj)/coreboot_ram.map
	$(OBJCOPY) --only-keep-debug $@ $(obj)/coreboot_ram.debug
	$(OBJCOPY) --strip-debug $@
	$(OBJCOPY) --add-gnu-debuglink=$(obj)/coreboot_ram.debug $@

$(obj)/coreboot_ram.o: $(obj)/arch/x86/lib/c_start.ramstage.o $$(driver-objs) $(obj)/coreboot.a $(LIBGCC_FILE_NAME)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -r -o $@ $(obj)/arch/x86/lib/c_start.ramstage.o $(driver-objs) -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(obj)/coreboot.a $(LIBGCC_FILE_NAME) -Wl,--end-group

$(obj)/coreboot.a: $$(ramstage-objs)
	@printf "    AR         $(subst $(obj)/,,$(@))\n"
	rm -f $(obj)/coreboot.a
	$(AR) cr $(obj)/coreboot.a $^

#######################################################################
# coreboot_ap.rom

ifeq ($(CONFIG_AP_CODE_IN_CAR),y)

$(obj)/coreboot_ap: $(obj)/mainboard/$(MAINBOARDDIR)/ap_romstage.o
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/x86/init/ldscript_apc.lb $^
	$(OBJCOPY) --only-keep-debug $@ $(obj)/coreboot_ap.debug
	$(OBJCOPY) --strip-debug $@
	$(OBJCOPY) --add-gnu-debuglink=$(obj)/coreboot_ap.debug $@
	$(NM) -n $(obj)/coreboot_ap | sort > $(obj)/coreboot_ap.map


endif

#######################################################################
# done

crt0s = $(src)/arch/x86/init/prologue.inc
ldscripts =
ldscripts += $(src)/arch/x86/init/ldscript_fallback_cbfs.lb
ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(src)/cpu/x86/16bit/entry16.inc
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
endif
crt0s += $(src)/cpu/x86/32bit/entry32.inc
ldscripts += $(src)/cpu/x86/32bit/entry32.lds
ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(src)/cpu/x86/16bit/reset16.inc
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
crt0s += $(src)/arch/x86/lib/id.inc
ldscripts += $(src)/arch/x86/lib/id.lds
endif

crt0s += $(src)/cpu/x86/fpu_enable.inc
ifeq ($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_enable.inc
endif

crt0s += $(cpu_incs)

#
# FIXME move to CPU_INTEL_SOCKET_MPGA604
#
ifeq ($(CONFIG_BOARD_TYAN_S2735),y)
crt0s += $(src)/cpu/intel/car/cache_as_ram.inc
endif

ifeq ($(CONFIG_LLSHELL),y)
crt0s += $(src)/arch/x86/llshell/llshell.inc
endif

crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

ifeq ($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_disable.inc
endif
ifeq ($(CONFIG_MMX),y)
crt0s += $(src)/cpu/x86/mmx_disable.inc
endif

ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(chipset_bootblock_inc)
ldscripts += $(chipset_bootblock_lds)
endif

ifeq ($(CONFIG_ROMCC),y)
crt0s += $(src)/arch/x86/init/crt0_romcc_epilogue.inc
endif

ifeq ($(CONFIG_ROMCC),y)
ROMCCFLAGS ?= -mcpu=p2 -O2

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(objutil)/romcc/romcc $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	printf "    ROMCC      romstage.inc\n"
	$(ROMCC) -c -S $(ROMCCFLAGS) -D__PRE_RAM__ -I. $(INCLUDES) $< -o $@
else

$(obj)/mainboard/$(MAINBOARDDIR)/ap_romstage.o: $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD $(CFLAGS) -I$(src) -I. -I$(obj) -c $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	@printf "    CC         romstage.inc\n"
	$(CC) -MMD $(CFLAGS) -D__PRE_RAM__ -I$(src) -I. -I$(obj) -c -S $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc
	@printf "    POST       romstage.inc\n"
	sed -e 's/\.rodata/.rom.data/g' -e 's/\.text/.section .rom.text/g' $^ > $@.tmp
	mv $@.tmp $@
endif

# Things that appear in every board
romstage-srcs += $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mainboard.c
ifeq ($(CONFIG_GENERATE_MP_TABLE),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mptable.c
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
ifeq ($(CONFIG_BOARD_HAS_FADT),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/fadt.c
endif
endif

ifeq ($(CONFIG_HAVE_BUS_CONFIG),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c
endif

ifeq ($(CONFIG_TINY_BOOTBLOCK),y)
include $(src)/arch/x86/Makefile.bootblock.inc
else
include $(src)/arch/x86/Makefile.bigbootblock.inc
endif

seabios:
	$(MAKE) -C payloads/external/SeaBIOS -f Makefile.inc \
			HOSTCC="$(HOSTCC)" \
			CC="$(CC)" LD="$(LD)" OBJDUMP="$(OBJDUMP)" \
			OBJCOPY="$(OBJCOPY)" STRIP="$(STRIP)" \
			CONFIG_SEABIOS_MASTER=$(CONFIG_SEABIOS_MASTER) \
			CONFIG_SEABIOS_STABLE=$(CONFIG_SEABIOS_STABLE)

