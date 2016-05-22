/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2006-2010 coresystems GmbH
 * Copyright (C) 2015-2016 Intel Corporation.
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <device/pci.h>
#include <device/pci_def.h>
#include <rules.h>
#include <soc/romstage.h>

int set_base_address_and_enable_uart(u8 bus, u8 dev, u8 func, u32 mmio_base)
{
	uint16_t reg16;

	/* HSUART controller #1 (B0:D20:F5). */
	pci_devfn_t uart_bdf = PCI_DEV(bus, dev, func);

	/* Decode BAR0(offset 0x10). */
	pci_write_config32(uart_bdf, PCI_BASE_ADDRESS_0, mmio_base);

	/* Enable MEMBASE at CMD(offset 0x04). */
	reg16 = pci_read_config16(uart_bdf, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(uart_bdf, PCI_COMMAND, reg16);

	return 0;
}
