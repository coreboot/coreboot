/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>

/*
 * return family number and internal pad number in that community
 * by pad number and which community it is in.
 */



	/* family number in high byte and inner pad number in lowest byte */

void mainboard_pre_console_init(struct romstage_params *params)
{
	uint32_t reg;
	uint32_t *pad_config_reg;

	/* Enable the UART hardware for COM1. */
	reg = 1;
	pci_write_config32(PCI_DEV(0, LPC_DEV, 0), UART_CONT, reg);

	/*
	 * Set up the pads to select the UART function
	 * AD12 SW16(UART1_DATAIN/UART0_DATAIN)   - Setting Mode 2 for UART0_RXD
	 * AD10 SW20(UART1_DATAOUT/UART0_DATAOUT) - Setting Mode 2 for UART0_TXD
	 */
	pad_config_reg = gpio_pad_config_reg(GP_SOUTHWEST, UART1_RXD_PAD);
	write32(pad_config_reg, SET_PAD_MODE_SELECTION(PAD_CONFIG0_DEFAULT0,
		M2));

	pad_config_reg = gpio_pad_config_reg(GP_SOUTHWEST, UART1_TXD_PAD);
	write32(pad_config_reg, SET_PAD_MODE_SELECTION(PAD_CONFIG0_DEFAULT0,
		M2));
}
