##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 - 2017 Intel Corporation.
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

ifeq ($(CONFIG_SOC_INTEL_DENVERTON_NS),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-$(CONFIG_HAVE_SMI_HANDLER) += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache

bootblock-$(CONFIG_FSP_CAR)+= bootblock/cache_as_ram_fsp.S
bootblock-y += bootblock/bootblock.c
bootblock-y += spi.c
bootblock-y += tsc_freq.c
bootblock-$(CONFIG_CONSOLE_SERIAL) += bootblock/uart.c
bootblock-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

postcar-y += memmap.c
postcar-$(CONFIG_FSP_CAR) += exit_car_fsp.S
postcar-y += spi.c
postcar-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

romstage-y += memmap.c
romstage-y += reset.c
romstage-y += romstage.c
romstage-y += tsc_freq.c
romstage-y += gpio_dnv.c
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
ramstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smm.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += soc_util.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c
smm-$(CONFIG_SPI_FLASH_SMM) += spi.c
smm-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

verstage-y += memmap.c
verstage-y += reset.c
verstage-y += spi.c
verstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart_debug.c

CPPFLAGS_common += -I$(src)/soc/intel/denverton_ns/include
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/denverton_ns

##Set FSP binary blobs memory location

$(CONFIG_FSP_T_CBFS)-options := -b $(CONFIG_FSP_T_ADDR) --xip
$(CONFIG_FSP_M_CBFS)-options := -b $(CONFIG_FSP_M_ADDR) --xip
$(CONFIG_FSP_S_CBFS)-options := -b $(CONFIG_FSP_S_ADDR) --xip

endif ## CONFIG_SOC_INTEL_DENVERTON_NS
