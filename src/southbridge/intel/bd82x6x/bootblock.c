/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "pch.h"

static void store_initial_timestamp(void)
{
	/* On Cougar Point we have two 32bit scratchpad registers available:
	 * D0:F0  0xdc (SKPAD)
	 * D31:F2 0xd0 (SATA SP)
	 */
	tsc_t tsc = rdtsc();
	pci_write_config32(PCI_DEV(0, 0x00, 0), 0xdc, tsc.lo);
	pci_write_config32(PCI_DEV(0, 0x1f, 2), 0xd0, tsc.hi);
}

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	u8 reg8;
	device_t dev;

	dev = PCI_DEV(0, 0x1f, 0);

	reg8 = pci_read_config8(dev, 0xdc);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(dev, 0xdc, reg8);
}

static void enable_port80_on_lpc(void)
{
	device_t dev = PCI_DEV(0, 0x1f, 0);

	/* Enable port 80 POST on LPC */
	pci_write_config32(dev, RCBA, DEFAULT_RCBA | 1);
#if 0
	RCBA32(GCS) &= (~0x04);
#else
	volatile u32 *gcs = (volatile u32 *)(DEFAULT_RCBA + GCS);
	u32 reg32 = *gcs;
	reg32 = reg32 & ~0x04;
	*gcs = reg32;
	post_code(0x01);
#endif
}

static void bootblock_southbridge_init(void)
{
#if CONFIG_COLLECT_TIMESTAMPS
	store_initial_timestamp();
#endif
	enable_spi_prefetch();
	enable_port80_on_lpc();
}
