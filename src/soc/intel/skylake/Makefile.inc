ifeq ($(CONFIG_SOC_INTEL_SKYLAKE),y)

subdirs-y += bootblock
subdirs-y += microcode
subdirs-y += romstage
subdirs-y += ../common
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

romstage-y += gpio.c
romstage-y += memmap.c
romstage-y += pch.c
romstage-y += pcr.c
romstage-y += pei_data.c
romstage-y += pmutil.c
romstage-y += smbus_common.c
romstage-y += tsc_freq.c
romstage-$(CONFIG_UART_DEBUG) += uart_debug.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += cpu_info.c
ramstage-y += elog.c
ramstage-y += finalize.c
ramstage-y += flash_controller.c
ramstage-y += gpio.c
ramstage-y += igd.c
ramstage-y += lpc.c
ramstage-y += memmap.c
ramstage-y += monotonic_timer.c
ramstage-y += pch.c
ramstage-y += pcie.c
ramstage-y += pcr.c
ramstage-y += pei_data.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-y += ramstage.c
ramstage-y += smbus.c
ramstage-y += smbus_common.c
ramstage-y += smi.c
ramstage-y += smmrelocate.c
ramstage-y += systemagent.c
ramstage-y += tsc_freq.c
ramstage-y += uart.c
ramstage-$(CONFIG_UART_DEBUG) += uart_debug.c
ramstage-y += xhci.c

smm-y += cpu_info.c
smm-y += gpio.c
smm-y += monotonic_timer.c
smm-y += pcr.c
smm-y += pch.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-$(CONFIG_SPI_FLASH_SMM) += flash_controller.c
smm-y += tsc_freq.c
smm-$(CONFIG_UART_DEBUG) += uart_debug.c

CPPFLAGS_common += -I$(src)/arch/x86/include/
CPPFLAGS_common += -I$(src)/soc/intel/skylake
CPPFLAGS_common += -I$(src)/soc/intel/skylake/include

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
	dd if=$(IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
ifeq ($(CONFIG_HAVE_ME_BIN),y)
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(CONFIG_ME_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

PHONY += pch_add_me

endif
