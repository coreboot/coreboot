/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <arch/cpu.h>
#include <bootblock_common.h>
#include <cpu/x86/mtrr.h>
#include <device/pci.h>
#include <lib.h>
#include <soc/bootblock.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/uart.h>

static const struct pad_config tpm_spi_configs[] = {
	PAD_CFG_NF(GPIO_106, NATIVE, DEEP, NF3),	/* FST_SPI_CS2_N */
};

static void tpm_enable(void)
{
	/* Configure gpios */
	gpio_configure_pads(tpm_spi_configs, ARRAY_SIZE(tpm_spi_configs));
}

void asmlinkage bootblock_c_entry(void)
{
	device_t dev = NB_DEV_ROOT;

	/* Set PCI Express BAR */
	pci_io_write_config32(dev, PCIEXBAR, CONFIG_MMCONF_BASE_ADDRESS | 1);

	dev = P2SB_DEV;
	/* BAR and MMIO enable for IOSF, so that GPIOs can be configured */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, CONFIG_IOSF_BASE_ADDRESS);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0);
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Call lib/bootblock.c main */
	main();
}

static void cache_bios_region(void)
{
	int mtrr;
	uint32_t rom_size, alignment;

	mtrr = get_free_var_mtrr();

	if (mtrr==-1)
		return;

	/* Only the IFD BIOS region is memory mapped (at top of 4G) */
	rom_size =  CONFIG_IFD_BIOS_END - CONFIG_IFD_BIOS_START;
	/* Round to power of two */
	alignment = 1 << (log2_ceil(rom_size));
	rom_size = ALIGN_UP(rom_size, alignment);
	set_var_mtrr(mtrr, 4ULL*GiB - rom_size, rom_size, MTRR_TYPE_WRPROT);
}

void bootblock_soc_early_init(void)
{
	/* Prepare UART for serial console. */
	if (IS_ENABLED(CONFIG_SOC_UART_DEBUG))
		soc_console_uart_init();

	if (IS_ENABLED(CONFIG_TPM_ON_FAST_SPI))
		tpm_enable();

	cache_bios_region();
}
