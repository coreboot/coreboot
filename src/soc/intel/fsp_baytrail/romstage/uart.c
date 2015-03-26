/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
#include <baytrail/gpio.h>
#include <baytrail/iomap.h>
#include <baytrail/lpc.h>
#include <baytrail/pci_devs.h>
#include <baytrail/romstage.h>

void byt_config_com1_and_enable(void)
{
	uint32_t reg;

	/* Enable the legacy UART hardware. */
	reg = 1;
	pci_write_config32(PCI_DEV(0, LPC_DEV, 0), UART_CONT, reg);

	/* Set up the pads to select the UART function */
	score_select_func(UART_RXD_PAD, 1);
	score_select_func(UART_TXD_PAD, 1);
}
