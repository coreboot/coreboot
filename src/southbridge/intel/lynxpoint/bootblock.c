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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <cpu/x86/tsc.h>
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


static void map_rcba(void)
{
	device_t dev = PCI_DEV(0, 0x1f, 0);

	pci_write_config32(dev, RCBA, DEFAULT_RCBA | 1);
}

static void enable_port80_on_lpc(void)
{
	/* Enable port 80 POST on LPC. The chipset does this by deafult,
	 * but it doesn't appear to hurt anything. */
	u32 gcs = RCBA32(GCS);
	gcs = gcs & ~0x4;
	RCBA32(GCS) = gcs;
}

static void set_spi_speed(void)
{
	u32 fdod;
	u8 ssfc;

	/* Observe SPI Descriptor Component Section 0 */
	SPIBAR32(FDOC) = 0x1000;

	/* Extract the Write/Erase SPI Frequency from descriptor */
	fdod = SPIBAR32(FDOD);
	fdod >>= 24;
	fdod &= 7;

	/* Set Software Sequence frequency to match */
	ssfc = SPIBAR8(SSFC + 2);
	ssfc &= ~7;
	ssfc |= fdod;
	SPIBAR8(SSFC + 2) = ssfc;
}

static void bootblock_southbridge_init(void)
{
#if CONFIG_COLLECT_TIMESTAMPS
	store_initial_timestamp();
#endif
	map_rcba();
	enable_spi_prefetch();
	enable_port80_on_lpc();
	set_spi_speed();

	/* Enable upper 128bytes of CMOS */
	RCBA32(RC) = (1 << 2);
}
