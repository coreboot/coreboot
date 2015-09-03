ifeq ($(CONFIG_SOC_INTEL_BRASWELL),y)

subdirs-y += microcode
subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

romstage-y += gpio_support.c
romstage-y += iosf.c
romstage-y += lpc_init.c
romstage-y += memmap.c
romstage-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += emmc.c
ramstage-y += gpio.c
ifeq ($(CONFIG_GOP_SUPPORT),n)
ramstage-y += gfx.c
endif
ramstage-y += hda.c
ramstage-y += iosf.c
ramstage-y += lpe.c
ramstage-y += lpss.c
ramstage-y += memmap.c
ramstage-y += northcluster.c
ramstage-y += pcie.c
ramstage-y += pmutil.c
ramstage-y += ramstage.c
ramstage-y += sata.c
ramstage-y += scc.c
ramstage-y += sd.c
ramstage-y += smm.c
ramstage-y += southcluster.c
ramstage-y += spi.c
ramstage-$(CONFIG_ALT_CBFS_LOAD_PAYLOAD) += spi_loading.c
ramstage-y += tsc_freq.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c

smm-y += lpc_init.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += spi.c
smm-y += tsc_freq.c

CPPFLAGS_common += -I$(src)/arch/x86/include/
CPPFLAGS_common += -I$(src)/soc/intel/braswell/
CPPFLAGS_common += -I$(src)/soc/intel/braswell/include

CPPFLAGS_common += -I3rdparty/blobs/mainboard/$(CONFIG_MAINBOARD_DIR)

CPPFLAGS_common += -I$(src)/drivers/intel/fsp1_1
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp1_1
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/uefi_2.4
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/uefi_2.4/MdePkg/Include
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/uefi_2.4/MdePkg/Include/Ia32
CPPFLAGS_common += -I$(CONFIG_FSP_INCLUDE_PATH)

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
INTERMEDIATE := pch_add_me

ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
IFD_BIN_PATH := $(objgenerated)/ifdfake.bin
IFD_SECTIONS := $(addprefix -b ,$(CONFIG_IFD_BIOS_SECTION:"%"=%)) \
		$(addprefix -m ,$(CONFIG_IFD_ME_SECTION:"%"=%)) \
		$(addprefix -p ,$(CONFIG_IFD_PLATFORM_SECTION:"%"=%))
else
IFD_BIN_PATH := $(CONFIG_IFD_BIN_PATH)
endif

pch_add_me: $(obj)/coreboot.pre $(IFDTOOL) $(IFDFAKE)
ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
	printf "\n** WARNING **\n"
	printf "Coreboot will be built with a fake Intel Firmware Descriptor (IFD).\n"
	printf "Never write a complete coreboot.rom with a fake IFD to your board's\n"
	printf "flash ROM! Make sure that you only write valid flash regions.\n\n"
	printf "    IFDFAKE    Building a fake Intel Firmware Descriptor\n"
	$(IFDFAKE) $(IFD_SECTIONS) $(IFD_BIN_PATH)
endif
	printf "    DD         Adding Intel Firmware Descriptor\n"
	printf "CONFIG_IFD_BIN_PATH: $(CONFIG_IFD_BIN_PATH)\n"
	printf "IFD_BIN_PATH: $(IFD_BIN_PATH)\n"
	dd if=$(IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
	printf "CONFIG_HAVE_ME_BIN: $(CONFIG_HAVE_ME_BIN)\n"
ifeq ($(CONFIG_HAVE_ME_BIN),y)
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	printf "CONFIG_ME_BIN_PATH: $(CONFIG_ME_BIN_PATH)\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(CONFIG_ME_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -l $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -u $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

PHONY += pch_add_me

endif
