subdirs-y += bootblock
subdirs-y += microcode
subdirs-y += romstage
subdirs-y += ../common
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

ramstage-y += acpi.c
ramstage-y += adsp.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += cpu_info.c
smm-y += cpu_info.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += finalize.c
ramstage-y += gpio.c
romstage-y += gpio.c
smm-y      += gpio.c
ramstage-y += hda.c
ramstage-y += igd.c
ramstage-y += iobp.c
romstage-y += iobp.c
ramstage-y += lpc.c
ramstage-y += me.c
ramstage-y += me_status.c
romstage-y += me_status.c
ramstage-y += memmap.c
romstage-y += memmap.c
ramstage-y += minihd.c
ramstage-y += monotonic_timer.c
ramstage-y += pch.c
romstage-y += pch.c
ramstage-y += pcie.c
ramstage-y += pei_data.c
romstage-y += pei_data.c
ramstage-y += pmutil.c
romstage-y += pmutil.c
smm-y      += pmutil.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_HAVE_REFCODE_BLOB) += refcode.c
ramstage-y += reset.c
romstage-y += reset.c
ramstage-y += sata.c
ramstage-y += serialio.c
ramstage-y += smbus.c
ramstage-y += smbus_common.c
romstage-y += smbus_common.c
ramstage-y += smi.c
smm-y      += smihandler.c
ramstage-y += smmrelocate.c
ramstage-y += spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += spi.c
ramstage-y += stage_cache.c
romstage-y += stage_cache.c
ramstage-y += systemagent.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-y      += tsc_freq.c
ramstage-y += ehci.c
ramstage-y += xhci.c
smm-y      += xhci.c

ifeq ($(CONFIG_USBDEBUG),y)
ramstage-y += usbdebug.c
romstage-y += usbdebug.c
smm-y      += usbdebug.c
endif

CPPFLAGS_common += -Isrc/soc/intel/broadwell/

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
INTERMEDIATE := broadwell_add_me

ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
IFD_BIN_PATH := $(objgenerated)/ifdfake.bin
IFD_SECTIONS := $(addprefix -b ,$(CONFIG_IFD_BIOS_SECTION:"%"=%)) \
		$(addprefix -m ,$(CONFIG_IFD_ME_SECTION:"%"=%)) \
		$(addprefix -p ,$(CONFIG_IFD_PLATFORM_SECTION:"%"=%))
else
IFD_BIN_PATH := $(CONFIG_IFD_BIN_PATH)
endif

broadwell_add_me: $(obj)/coreboot.pre $(IFDTOOL) $(IFDFAKE)
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
ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -l $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -u $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif
endif

PHONY += broadwell_add_me

# If an MRC file is an ELF file determine the entry address and first loadable
# section offset in the file. Subtract the offset from the entry address to
# determine the final location.
mrcelfoffset = $(shell $(READELF_x86_32) -S -W $(CONFIG_MRC_FILE) | sed -e 's/\[ /[0/' | awk '$$3 ~ /PROGBITS/ { print "0x"$$5; exit }' )
mrcelfentry = $(shell $(READELF_x86_32) -h -W $(CONFIG_MRC_FILE) | grep 'Entry point address' | awk '{print $$NF }')

# Add memory reference code blob.
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := $(if $(findstring elf,$(CONFIG_MRC_FILE)),$(shell printf "0x%x" $$(( $(mrcelfentry) - $(mrcelfoffset) )) ),$(CONFIG_MRC_BIN_ADDRESS))
mrc.bin-type := mrc
