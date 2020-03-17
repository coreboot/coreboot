/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>

void bootblock_mainboard_early_init(void)
{
	uint32_t reg;
	uint32_t *pad_config_reg;

	/* Enable the UART hardware for COM1. */
	reg = 1;
	pci_write_config32(PCI_DEV(0, LPC_DEV, 0), UART_CONT, reg);

	/*
	 * Set up the pads to select the UART function
	 * AD12 SW16(UART1_DATAIN/UART0_DATAIN)   - Set Mode 2 for UART0_RXD
	 * AD10 SW20(UART1_DATAOUT/UART0_DATAOUT) - Set Mode 2 for UART0_TXD
	 */
	pad_config_reg = gpio_pad_config_reg(GP_SOUTHWEST, UART1_RXD_PAD);
	write32(pad_config_reg, SET_PAD_MODE_SELECTION(PAD_CONFIG0_DEFAULT0,
		M2));

	pad_config_reg = gpio_pad_config_reg(GP_SOUTHWEST, UART1_TXD_PAD);
	write32(pad_config_reg, SET_PAD_MODE_SELECTION(PAD_CONFIG0_DEFAULT0,
		M2));
}
