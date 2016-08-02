ifeq ($(CONFIG_SOC_INTEL_SKYLAKE),y)

subdirs-y += nhlt
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cache_as_ram.S
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/systemagent.c
bootblock-$(CONFIG_UART_DEBUG) += bootblock/uart.c
bootblock-y += gpio.c
bootblock-y += monotonic_timer.c
bootblock-y += pch.c
bootblock-y += pcr.c
bootblock-y += pmutil.c
bootblock-y += tsc_freq.c
bootblock-$(CONFIG_UART_DEBUG) += uart_debug.c

verstage-$(CONFIG_UART_DEBUG) += uart_debug.c

romstage-y += flash_controller.c
romstage-y += gpio.c
romstage-y += memmap.c
romstage-y += monotonic_timer.c
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
ramstage-y += dsp.c
ramstage-y += elog.c
ramstage-y += finalize.c
ramstage-y += flash_controller.c
ramstage-y += gpio.c
ramstage-y += i2c.c
ramstage-y += igd.c
ramstage-y += lpc.c
ramstage-y += me_status.c
ramstage-y += memmap.c
ramstage-y += monotonic_timer.c
ramstage-y += pch.c
ramstage-y += pcie.c
ramstage-y += pcr.c
ramstage-y += pei_data.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-y += ramstage.c
ramstage-y += sd.c
ramstage-y += smbus.c
ramstage-y += smbus_common.c
ramstage-y += smi.c
ramstage-y += smmrelocate.c
ramstage-y += systemagent.c
ramstage-y += tsc_freq.c
ramstage-y += uart.c
ramstage-$(CONFIG_UART_DEBUG) += uart_debug.c
ramstage-y += vr_config.c
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

# cpu_microcode_bins += ???

CPPFLAGS_common += -I$(src)/soc/intel/skylake
CPPFLAGS_common += -I$(src)/soc/intel/skylake/include
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp1_1/skylake

# Currently used for microcode path.
CPPFLAGS_common += -I3rdparty/blobs/mainboard/$(CONFIG_MAINBOARD_DIR)

ROMCCFLAGS := -mcpu=p4 -fno-simplify-phi -O2

endif
