## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_DENVERTON_NS),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache

bootblock-y += bootblock/bootblock.c
bootblock-y += spi.c
bootblock-y += tsc_freq.c
bootblock-$(CONFIG_CONSOLE_SERIAL) += bootblock/uart.c
bootblock-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

postcar-y += memmap.c
postcar-y += spi.c
postcar-y += tsc_freq.c
postcar-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

romstage-y += memmap.c
romstage-y += reset.c
romstage-y += ../../../cpu/intel/car/romstage.c
romstage-y += romstage.c
romstage-y += tsc_freq.c
romstage-y += gpio_dnv.c
romstage-y += gpio.c
romstage-y += soc_util.c
romstage-y += spi.c
romstage-y += fiamux.c
romstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
romstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c

ramstage-y += memmap.c
ramstage-y += systemagent.c
ramstage-y += reset.c
ramstage-y += chip.c
ramstage-y += soc_util.c
ramstage-y += uart.c
ramstage-y += xhci.c
ramstage-y += csme_ie_kt.c
ramstage-y += lpc.c
ramstage-y += pmc.c
ramstage-y += npk.c
ramstage-y += sata.c
ramstage-y += cpu.c
ramstage-y += tsc_freq.c
ramstage-y += spi.c
ramstage-y += fiamux.c
ramstage-y += hob_mem.c
ramstage-y += gpio.c
ramstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smm.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c

smm-y += pmutil.c
smm-y += soc_util.c
smm-y += smihandler.c
smm-y += tsc_freq.c
smm-$(CONFIG_SPI_FLASH_SMM) += spi.c
smm-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

verstage-y += memmap.c
verstage-y += reset.c
verstage-y += spi.c
verstage-y += tsc_freq.c
verstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

CPPFLAGS_common += -I$(src)/soc/intel/denverton_ns/include

##Set FSP binary blobs memory location
$(call strip_quotes,$(CONFIG_FSP_T_CBFS))-options := -b $(CONFIG_FSP_T_ADDR) --xip
$(call strip_quotes,$(CONFIG_FSP_M_CBFS))-options := -b $(CONFIG_FSP_M_ADDR) --xip
$(call strip_quotes,$(CONFIG_FSP_S_CBFS))-options := -b $(CONFIG_FSP_S_ADDR) --xip

cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-5f-01

endif ## CONFIG_SOC_INTEL_DENVERTON_NS
