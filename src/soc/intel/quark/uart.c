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

#include <console/uart.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <rules.h>
#include <soc/pci_devs.h>

unsigned int uart_platform_refclk(void)
{
	return 44236800;
}

uintptr_t uart_platform_base(int idx)
{
	/* HSUART controller #1 (B0:D20:F5). */
	device_t dev = PCI_DEV(0, HSUART1_DEV, HSUART1_FUNC);

	/* UART base address at BAR0(offset 0x10). */
	return (unsigned int) (pci_read_config32(dev,
		PCI_BASE_ADDRESS_0) & ~0xfff);
}
