#######################################################################
# Take care of subdirectories
subdirs-y += boot
# subdirs-y += init
subdirs-y += lib
subdirs-y += smp

obj-$(CONFIG_HAVE_OPTION_TABLE) += ../../option_table.o

ifdef POST_EVALUATION

#######################################################################
# Build the final rom image
$(obj)/coreboot.rom: $(obj)/coreboot.pre $(obj)/coreboot_ram $(CBFSTOOL)
	cp $(obj)/coreboot.pre $@
	if [ -f fallback/coreboot_apc ]; \
	then \
		$(CBFSTOOL) $@ add-stage fallback/coreboot_apc $(CONFIG_CBFS_PREFIX)/coreboot_apc $(CBFS_COMPRESS_FLAG); \
	fi
	$(CBFSTOOL) $@ add-stage $(obj)/coreboot_ram $(CONFIG_CBFS_PREFIX)/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	@printf "    PAYLOAD    none (as specified by user)\n"
else
	@printf "    PAYLOAD    $(CONFIG_FALLBACK_PAYLOAD_FILE) $(CBFS_PAYLOAD_COMPRESS_FLAG)\n"
	$(CBFSTOOL) $(obj)/coreboot.rom add-payload $(CONFIG_FALLBACK_PAYLOAD_FILE) $(CONFIG_CBFS_PREFIX)/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_VGA_BIOS),y)
	@printf "    VGABIOS    $(CONFIG_FALLBACK_VGA_BIOS_FILE) $(CONFIG_FALLBACK_VGA_BIOS_ID)\n"
	$(CBFSTOOL) $(obj)/coreboot.rom add $(CONFIG_FALLBACK_VGA_BIOS_FILE) "pci$(CONFIG_FALLBACK_VGA_BIOS_ID).rom" optionrom
endif
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $(obj)/coreboot.rom print

#######################################################################
# i386 specific tools

$(obj)/option_table.h $(obj)/option_table.c $(obj)/arch/i386/../../option_table.c: $(obj)/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(obj)/build_opt_tbl --config $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout --header $(obj)/option_table.h --option $(obj)/option_table.c

$(obj)/build_opt_tbl: $(top)/util/options/build_opt_tbl.c $(top)/src/include/pc80/mc146818rtc.h $(top)/src/include/boot/coreboot_tables.h $(obj)/config.h
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -include $(obj)/config.h $< -o $@

#######################################################################
# Build the coreboot_ram (stage 2)

$(obj)/coreboot_ram: $(obj)/coreboot_ram.o $(src)/arch/i386/coreboot_ram.ld #ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/i386/coreboot_ram.ld $(obj)/coreboot_ram.o
	$(NM) -n $(obj)/coreboot_ram | sort > $(obj)/coreboot_ram.map

$(obj)/coreboot_ram.o: $(obj)/arch/i386/lib/c_start.o $(drivers) $(obj)/coreboot.a $(LIBGCC_FILE_NAME)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -r -o $@ $(obj)/arch/i386/lib/c_start.o $(drivers) -Wl,-\( $(obj)/coreboot.a $(LIBGCC_FILE_NAME) -Wl,-\)

$(obj)/coreboot.a: $(objs)
	@printf "    AR         $(subst $(obj)/,,$(@))\n"
	rm -f $(obj)/coreboot.a
	$(AR) cr $(obj)/coreboot.a $(objs)

#######################################################################
# done

crt0s :=
ldscripts :=
ldscripts += $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ldscripts += $(src)/arch/i386/lib/failover.lds
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
ifeq ($(CONFIG_CPU_INTEL_CORE),y)
crt0s += $(src)/cpu/intel/model_6ex/cache_as_ram.inc
endif
# Use Intel Core (not Core 2) code for CAR init, any CPU might be used.
ifeq ($(CONFIG_CPU_INTEL_SOCKET_BGA956),y)
crt0s += $(src)/cpu/intel/model_6ex/cache_as_ram.inc
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

ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
ifeq ($(CONFIG_ROMCC),y)
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/failover.inc
endif
endif
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

ifeq ($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_disable.inc
endif
ifeq ($(CONFIG_MMX),y)
crt0s += $(src)/cpu/x86/mmx_disable.inc
endif

ifeq ($(CONFIG_AP_CODE_IN_CAR),y)
ldscripts += $(src)/arch/i386/init/ldscript_apc.lb
endif

ifeq ($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(chipset_bootblock_inc)
ldscripts += $(chipset_bootblock_lds)
endif

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)
OPTION_TABLE_H:=$(obj)/option_table.h
endif

ifeq ($(CONFIG_ROMCC),y)
ROMCCFLAGS ?= -mcpu=p2 -O2

$(obj)/mainboard/$(MAINBOARDDIR)/failover.inc: $(obj)/romcc $(src)/arch/i386/lib/failover.c
	$(obj)/romcc $(ROMCCFLAGS) --label-prefix=failover $(INCLUDES) $(src)/arch/i386/lib/failover.c -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(obj)/romcc $(OPTION_TABLE_H) $(obj)/build.h
	$(obj)/romcc $(ROMCCFLAGS) -include $(obj)/build.h $(INCLUDES) $< -o $@

else

$(obj)/mainboard/$(MAINBOARDDIR)/ap_romstage.o: $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c $(obj)/option_table.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c -S  $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -include $(obj)/build.h -I$(src) -I. -c -S $< -o $@.tmp1
	sed -e 's/\.rodata/.rom.data/g' -e 's/\.text/.section .rom.text/g' $@.tmp1 > $@.tmp
	mv $@.tmp $@
	rm -f $@.tmp1
endif

else
# Only in first pass

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
ifeq ($(CONFIG_BOARD_HAS_FADT),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/fadt.o
endif
endif

ifeq ($(CONFIG_HAVE_BUS_CONFIG),y)
objs += $(obj)/mainboard/$(MAINBOARDDIR)/get_bus_conf.o
endif

endif

ifeq ($(CONFIG_TINY_BOOTBLOCK),y)
include $(src)/arch/i386/Makefile.bootblock.inc
else
include $(src)/arch/i386/Makefile.bigbootblock.inc
endif
