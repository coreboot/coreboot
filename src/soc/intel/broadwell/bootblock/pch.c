/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
#include <broadwell/iomap.h>
#include <broadwell/lpc.h>
#include <broadwell/pci_devs.h>
#include <broadwell/rcba.h>
#include <broadwell/spi.h>

static void store_initial_timestamp(void)
{
	/* Two 32bit scratchpad registers available:
	 * D0:F0  0xdc (SKPAD)
	 * D31:F2 0xd0 (SATA SP)
	 */
	tsc_t tsc = rdtsc();
	pci_write_config32(SA_DEV_ROOT, 0xdc, tsc.lo);
	pci_write_config32(PCH_DEV_SATA, 0xd0, tsc.hi);
}

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	u8 reg8 = pci_read_config8(PCH_DEV_LPC, 0xdc);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(PCH_DEV_LPC, 0xdc, reg8);
}


static void map_rcba(void)
{
	pci_write_config32(PCH_DEV_LPC, RCBA, RCBA_BASE_ADDRESS | 1);
}

static void enable_port80_on_lpc(void)
{
	/* Enable port 80 POST on LPC. The chipset does this by default,
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
	SPIBAR32(SPIBAR_FDOC) = 0x1000;

	/* Extract the Write/Erase SPI Frequency from descriptor */
	fdod = SPIBAR32(SPIBAR_FDOD);
	fdod >>= 24;
	fdod &= 7;

	/* Set Software Sequence frequency to match */
	ssfc = SPIBAR8(SPIBAR_SSFC + 2);
	ssfc &= ~7;
	ssfc |= fdod;
	SPIBAR8(SPIBAR_SSFC + 2) = ssfc;
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
}
