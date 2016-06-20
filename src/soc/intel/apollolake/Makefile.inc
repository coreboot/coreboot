ifeq ($(CONFIG_SOC_INTEL_APOLLOLAKE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cache_as_ram.S
bootblock-y += bootblock/bootblock.c
bootblock-y += car.c
bootblock-y += gpio.c
bootblock-y += lpc_lib.c
bootblock-y += mmap_boot.c
bootblock-y += spi.c
bootblock-y += tsc_freq.c
bootblock-$(CONFIG_SOC_UART_DEBUG) += uart_early.c

romstage-y += car.c
romstage-$(CONFIG_PLATFORM_USES_FSP2_0) += romstage.c
romstage-y += gpio.c
romstage-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
romstage-y += lpc_lib.c
romstage-y += memmap.c
romstage-y += meminit.c
romstage-y += mmap_boot.c
romstage-y += tsc_freq.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += spi.c

smm-y += mmap_boot.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += spi.c
smm-y += tsc_freq.c
smm-y += uart_early.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += cpu.c
ramstage-y += chip.c
ramstage-y += gpio.c
ramstage-y += graphics.c
ramstage-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
ramstage-y += lpc.c
ramstage-y += lpc_lib.c
ramstage-y += memmap.c
ramstage-y += mmap_boot.c
ramstage-y += uart.c
ramstage-y += northbridge.c
ramstage-y += spi.c
ramstage-y += tsc_freq.c
ramstage-y += pmutil.c
ramstage-y += pmc.c
ramstage-y += smi.c
ramstage-y += spi.c
ramstage-y += reset.c

postcar-y += exit_car.S
postcar-y += memmap.c
postcar-y += mmap_boot.c
postcar-y += spi.c
postcar-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
postcar-y += tsc_freq.c

verstage-y += car.c
verstage-y += memmap.c
verstage-y += mmap_boot.c
verstage-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
verstage-y += tsc_freq.c
verstage-y += pmutil.c
verstage-y += reset.c
verstage-y += spi.c

CPPFLAGS_common += -I$(src)/soc/intel/apollolake/include

# Since FSP-M runs in CAR we need to relocate it to a specific address
$(CONFIG_FSP_M_CBFS)-options := -b $(CONFIG_FSP_M_ADDR)

ifeq ($(CONFIG_NEED_LBP2),y)
files_added::
	$(CBFSTOOL) $(obj)/coreboot.rom write -r $(CONFIG_LBP2_FMAP_NAME) -f $(CONFIG_LBP2_FILE_NAME) --fill-upward
endif

# Bootblock on Apollolake platform lies in the IFWI region. In order to place
# the bootblock at the right location in IFWI image -
# a. Using ifwitool:
#    1. Create IFWI image (ifwi.bin.tmp) from input image
#       (CONFIG_IFWI_FILE_NAME).
#    2. Delete OBBP sub-partition, if present.
#    3. Replace IBBL directory entry in IBBP sub-partition with currently
#       generated bootblock.bin.
# b. Using cbfstool:
#    1. Write ifwi.bin.tmp to coreboot.rom using CONFIG_IFWI_FMAP_NAME.
ifeq ($(CONFIG_NEED_IFWI),y)
files_added:: $(IFWITOOL)
	$(IFWITOOL) $(CONFIG_IFWI_FILE_NAME) create -f $(objcbfs)/ifwi.bin.tmp
	$(IFWITOOL) $(objcbfs)/ifwi.bin.tmp delete -n OBBP
	$(IFWITOOL) $(objcbfs)/ifwi.bin.tmp replace -n IBBP -f $(objcbfs)/bootblock.bin -d -e IBBL
	$(CBFSTOOL) $(obj)/coreboot.rom write -r $(CONFIG_IFWI_FMAP_NAME) -f $(objcbfs)/ifwi.bin.tmp --fill-upward
endif

endif
