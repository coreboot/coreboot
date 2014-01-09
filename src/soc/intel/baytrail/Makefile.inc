subdirs-y += bootblock
subdirs-y += microcode
subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

ramstage-y += memmap.c
romstage-y += memmap.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-y += tsc_freq.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += nvm.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
romstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
ramstage-y += spi.c
smm-y += spi.c
ramstage-y += chip.c
ramstage-y += gfx.c
ramstage-y += iosf.c
romstage-y += iosf.c
ramstage-y += northcluster.c
ramstage-y += ramstage.c
ramstage-y += gpio.c
romstage-y += reset.c
ramstage-y += reset.c
ramstage-y += cpu.c
ramstage-y += pmutil.c
smm-y += pmutil.c
smm-y += smihandler.c
ramstage-y += smm.c
ramstage-y += ehci.c
ramstage-y += xhci.c
ramstage-y += southcluster.c
ramstage-$(CONFIG_HAVE_REFCODE_BLOB) += refcode.c
ramstage-y += sata.c
ramstage-y += acpi.c
ramstage-y += lpe.c
ramstage-y += scc.c
ramstage-y += emmc.c
ramstage-y += lpss.c
ramstage-y += pcie.c
ramstage-y += sd.c
ramstage-y += perf_power.c
ramstage-y += stage_cache.c
romstage-y += stage_cache.c
ramstage-$(CONFIG_ELOG) += elog.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c

INCLUDES += -Isrc/soc/intel/baytrail/

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
INTERMEDIATE:=baytrail_add_me

ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
IFD_BIN_PATH := $(objgenerated)/ifdfake.bin
IFD_SECTIONS := $(addprefix -b ,$(CONFIG_IFD_BIOS_SECTION:"%"=%)) \
		$(addprefix -m ,$(CONFIG_IFD_ME_SECTION:"%"=%)) \
		$(addprefix -p ,$(CONFIG_IFD_PLATFORM_SECTION:"%"=%))
else
IFD_BIN_PATH := $(CONFIG_IFD_BIN_PATH)
endif

baytrail_add_me: $(obj)/coreboot.pre $(IFDTOOL) $(IFDFAKE)
ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
	printf "\n** WARNING **\n"
	printf "Coreboot will be built with a fake Intel Firmware Descriptor (IFD).\n"
	printf "Never write a complete coreboot.rom with a fake IFD to your board's\n"
	printf "flash ROM! Make sure that you only write valid flash regions.\n\n"
	printf "    IFDFAKE    Building a fake Intel Firmware Descriptor\n"
	$(IFDFAKE) $(IFD_SECTIONS) $(IFD_BIN_PATH)
endif
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
ifeq ($(CONFIG_HAVE_ME_BIN),y)
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(CONFIG_ME_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

# Add memory reference code blob.
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := $(CONFIG_MRC_BIN_ADDRESS)
mrc.bin-type := 0xab

PHONY += baytrail_add_me
