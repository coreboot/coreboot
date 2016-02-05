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
#include <device/pci.h>
#include <soc/bootblock.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/uart.h>

static const struct pad_config tpm_spi_configs[] = {
	PAD_CFG_NF(GPIO_106, NATIVE, DEEP, NF3),	/* FST_SPI_CS2_N */
};

static const struct pad_config lpc_gpio_configs[] = {
	PAD_CFG_NF(LPC_AD0, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_AD1, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_AD2, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_AD3, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_FRAMEB, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT1, UP_20K, DEEP, NF1)
};

static void tpm_enable(void)
{
	/* Configure gpios */
	gpio_configure_pads(tpm_spi_configs, ARRAY_SIZE(tpm_spi_configs));
}

static void early_lpc_enable(void)
{
	/* Enable requested fixed IO decode ranges */
	pci_write_config16(LPC_DEV, LPC_EN, LPC_EN_MC1 | LPC_EN_KB | LPC_EN_LGAME);

	/* Enable generic IO decode ranges for 0x800-0x9ff */
	/* FIXME: remove range hardcoding and/or calculate based on EC definitions */
	pci_write_config32(LPC_DEV, LPC_GEN1_DEC, ((0xff & ~3 ) << 8) | 0x800 | 1);
	pci_write_config32(LPC_DEV, LPC_GEN2_DEC, ((0xff & ~3 ) << 8) | 0x900 | 1);

	/* GPIO pins need to be configured to specific native function */
	gpio_configure_pads(lpc_gpio_configs, ARRAY_SIZE(lpc_gpio_configs));
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

void bootblock_soc_early_init(void)
{
	/* Prepare UART for serial console. */
	if (IS_ENABLED(CONFIG_SOC_UART_DEBUG))
		soc_console_uart_init();

	if (IS_ENABLED(CONFIG_LPC_TPM))
		tpm_enable();

	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC_LPC))
		early_lpc_enable();

}
