#######################################################################
# Take care of subdirectories
subdirs-y += boot
# subdirs-y += init
subdirs-y += lib
subdirs-y += smp

obj-$(CONFIG_HAVE_OPTION_TABLE) += ../../option_table.o

#######################################################################
# Build the final rom image
COREBOOT_ROM_DEPENDENCIES:=
ifneq ($(CONFIG_PAYLOAD_NONE),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_FALLBACK_PAYLOAD_FILE)
endif
ifeq ($(CONFIG_VGA_BIOS),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_FALLBACK_VGA_BIOS_FILE)
endif
ifeq ($(CONFIG_INTEL_MBI),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_FALLBACK_MBI_FILE)
endif
ifeq ($(CONFIG_BOOTSPLASH),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_FALLBACK_BOOTSPLASH_FILE)
endif
ifeq ($(CONFIG_AP_CODE_IN_CAR),y)
COREBOOT_ROM_DEPENDENCIES+=$(obj)/coreboot_ap
endif

$(obj)/coreboot.rom: $(obj)/coreboot.pre $(obj)/coreboot_ram $(CBFSTOOL) $(call strip_quotes,$(COREBOOT_ROM_DEPENDENCIES))
	printf "    CBFS       $(subst $(obj)/,,$(@))\n"
	cp $(obj)/coreboot.pre $@.tmp
	if [ -f $(obj)/coreboot_ap ]; \
	then \
		$(CBFSTOOL) $@.tmp add-stage $(obj)/coreboot_ap $(CONFIG_CBFS_PREFIX)/coreboot_ap $(CBFS_COMPRESS_FLAG); \
	fi
	$(CBFSTOOL) $@.tmp add-stage $(obj)/coreboot_ram $(CONFIG_CBFS_PREFIX)/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	printf "    PAYLOAD    none (as specified by user)\n"
else
	printf "    PAYLOAD    $(CONFIG_FALLBACK_PAYLOAD_FILE) $(CBFS_PAYLOAD_COMPRESS_FLAG)\n"
	$(CBFSTOOL) $@.tmp add-payload $(CONFIG_FALLBACK_PAYLOAD_FILE) $(CONFIG_CBFS_PREFIX)/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_VGA_BIOS),y)
	printf "    VGABIOS    $(CONFIG_FALLBACK_VGA_BIOS_FILE) $(CONFIG_FALLBACK_VGA_BIOS_ID)\n"
	$(CBFSTOOL) $@.tmp add $(CONFIG_FALLBACK_VGA_BIOS_FILE) "pci$(CONFIG_FALLBACK_VGA_BIOS_ID).rom" optionrom
endif
ifeq ($(CONFIG_INTEL_MBI),y)
	printf "    MBI        $(CONFIG_FALLBACK_MBI_FILE)\n"
	$(CBFSTOOL) $@.tmp add $(CONFIG_FALLBACK_MBI_FILE) mbi.bin mbi
endif
ifeq ($(CONFIG_BOOTSPLASH),y)
	printf "    BOOTSPLASH $(CONFIG_FALLBACK_BOOTSPLASH_FILE)\n"
	$(CBFSTOOL) $@.tmp add $(CONFIG_FALLBACK_BOOTSPLASH_FILE) bootsplash.jpg bootsplash
endif
	mv $@.tmp $@
	printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ print

#######################################################################
# i386 specific tools

$(obj)/option_table.h $(obj)/option_table.c: $(obj)/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(obj)/build_opt_tbl --config $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout --header $(obj)/option_table.h --option $(obj)/option_table.c

$(obj)/build_opt_tbl: $(top)/util/options/build_opt_tbl.c $(top)/src/include/pc80/mc146818rtc.h $(top)/src/include/boot/coreboot_tables.h
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) $< -o $@

#######################################################################
# Build the coreboot_ram (stage 2)

$(obj)/coreboot_ram: $(obj)/coreboot_ram.o $(src)/arch/i386/coreboot_ram.ld #ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/i386/coreboot_ram.ld $(obj)/coreboot_ram.o
	$(NM) -n $(obj)/coreboot_ram | sort > $(obj)/coreboot_ram.map

$(obj)/coreboot_ram.o: $(obj)/arch/i386/lib/c_start.o $$(drivers) $(obj)/coreboot.a $(LIBGCC_FILE_NAME)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -r -o $@ $(obj)/arch/i386/lib/c_start.o $(drivers) -Wl,--start-group $(obj)/coreboot.a $(LIBGCC_FILE_NAME) -Wl,--end-group

$(obj)/coreboot.a: $$(objs)
	@printf "    AR         $(subst $(obj)/,,$(@))\n"
	rm -f $(obj)/coreboot.a
	$(AR) cr $(obj)/coreboot.a $^

#######################################################################
# coreboot_ap.rom

ifeq ($(CONFIG_AP_CODE_IN_CAR),y)

$(obj)/coreboot_ap: $(obj)/mainboard/$(MAINBOARDDIR)/ap_romstage.o
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/i386/init/ldscript_apc.lb $^
	$(NM) -n $(obj)/coreboot_ap | sort > $(obj)/coreboot_ap.map


endif

#######################################################################
# done

crt0s = $(src)/arch/i386/init/crt0_prologue.inc
ldscripts =
ldscripts += $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(src)/cpu/x86/16bit/entry16.inc
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
endif
crt0s += $(src)/cpu/x86/32bit/entry32.inc
ldscripts += $(src)/cpu/x86/32bit/entry32.lds
ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(src)/cpu/x86/16bit/reset16.inc
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
ifeq ($(CONFIG_ROMCC),y)
crt0s += $(src)/arch/i386/lib/cpu_reset.inc
endif
crt0s += $(src)/arch/i386/lib/id.inc
ldscripts += $(src)/arch/i386/lib/id.lds
endif

crt0s += $(src)/cpu/x86/fpu_enable.inc
ifeq ($(CONFIG_CPU_AMD_GX1),y)
crt0s += $(src)/cpu/amd/model_gx1/cpu_setup.inc
crt0s += $(src)/cpu/amd/model_gx1/gx_setup.inc
endif
ifeq ($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_enable.inc
endif

ifeq ($(CONFIG_CPU_AMD_LX),y)
crt0s += $(src)/cpu/amd/model_lx/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_F),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_F_1207),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_AM2R2),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_AM2),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_S1G1),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_754),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_939),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_AMD_SOCKET_940),y)
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_INTEL_ATOM_230),y)
crt0s += $(src)/cpu/intel/model_106cx/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_INTEL_CORE),y)
crt0s += $(src)/cpu/intel/model_6ex/cache_as_ram.inc
endif
# Use Intel Core (not Core 2) code for CAR init, any CPU might be used.
ifeq ($(CONFIG_CPU_INTEL_SOCKET_BGA956),y)
crt0s += $(src)/cpu/intel/model_6ex/cache_as_ram.inc
endif
ifeq ($(CONFIG_CPU_INTEL_SOCKET_MFCBGA479),y)
crt0s += $(src)/cpu/x86/car/cache_as_ram.inc
endif
# should be CONFIG_CPU_VIA_C7, but bcom/winnetp680, jetway/j7f24, via/epia-cn, via/pc2500e don't use CAR yet
ifeq ($(CONFIG_BOARD_VIA_VT8454C),y)
crt0s += $(src)/cpu/via/car/cache_as_ram.inc
endif
ifeq ($(CONFIG_BOARD_VIA_EPIA_M700),y)
crt0s += $(src)/cpu/via/car/cache_as_ram.inc
endif
# who else could use this?
ifeq ($(CONFIG_BOARD_TYAN_S2735),y)
crt0s += $(src)/cpu/x86/car/cache_as_ram.inc
ldscripts += $(src)/cpu/x86/car/cache_as_ram.lds
endif

ifeq ($(CONFIG_LLSHELL),y)
crt0s += $(src)/arch/i386/llshell/llshell.inc
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
crt0s += $(src)/arch/i386/init/crt0_romcc_epilogue.inc
endif

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)
OPTION_TABLE_H:=$(obj)/option_table.h
endif

ifeq ($(CONFIG_ROMCC),y)
ROMCCFLAGS ?= -mcpu=p2 -O2

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(obj)/romcc $(OPTION_TABLE_H) $(obj)/build.h
	printf "    ROMCC      romstage.inc\n"
	$(ROMCC) -c -S $(ROMCCFLAGS) -D__PRE_RAM__ -I. $(INCLUDES) $< -o $@
else

$(obj)/mainboard/$(MAINBOARDDIR)/ap_romstage.o: $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD $(CFLAGS) -I$(src) -I. -c $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h
	printf "    CC         romstage.inc\n"
	$(CC) -MMD $(CFLAGS) -D__PRE_RAM__ -I$(src) -I. -c -S $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc
	printf "    POST       romstage.inc\n"
	sed -e 's/\.rodata/.rom.data/g' -e 's/\.text/.section .rom.text/g' $^ > $@.tmp
	mv $@.tmp $@
endif

# Things that appear in every board
initobjs += $(obj)/mainboard/$(MAINBOARDDIR)/crt0.o
objs += $(obj)/mainboard/$(MAINBOARDDIR)/mainboard.o
ifeq ($(CONFIG_GENERATE_MP_TABLE),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/mptable.o
endif
ifeq ($(CONFIG_GENERATE_PIRQ_TABLE),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/irq_tables.o
endif
ifeq ($(CONFIG_BOARD_HAS_HARD_RESET),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/reset.o
endif
ifeq ($(CONFIG_GENERATE_ACPI_TABLES),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/acpi_tables.o
objs += $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.o
# make doesn't have arithmetic operators or greater-than comparisons
ifeq ($(subst 5,4,$(CONFIG_ACPI_SSDTX_NUM)),4)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/ssdt2.o
objs += $(obj)/mainboard/$(MAINBOARDDIR)/ssdt3.o
objs += $(obj)/mainboard/$(MAINBOARDDIR)/ssdt4.o
endif
ifeq ($(CONFIG_ACPI_SSDTX_NUM),5)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/ssdt5.o
endif
ifeq ($(CONFIG_BOARD_HAS_FADT),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/fadt.o
endif
endif

ifeq ($(CONFIG_HAVE_BUS_CONFIG),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/get_bus_conf.o
endif

ifeq ($(CONFIG_TINY_BOOTBLOCK),y)
include $(src)/arch/i386/Makefile.bootblock.inc
else
include $(src)/arch/i386/Makefile.bigbootblock.inc
endif
