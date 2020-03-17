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

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "i82801jx.h"

static void enable_spi_prefetch(void)
{
	u8 reg8;
	pci_devfn_t dev;

	dev = PCI_DEV(0, 0x1f, 0);

	reg8 = pci_read_config8(dev, 0xdc);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(dev, 0xdc, reg8);
}

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetch();

	i82801jx_setup_bars();

	/* Enable upper 128bytes of CMOS. */
	RCBA32(0x3400) = (1 << 2);

	i82801jx_lpc_setup();
}
