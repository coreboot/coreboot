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
 */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "pch.h"

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	u8 reg8;
	pci_devfn_t dev = PCH_LPC_DEV;

	reg8 = pci_read_config8(dev, BIOS_CNTL);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(dev, BIOS_CNTL, reg8);
}

static void enable_port80_on_lpc(void)
{
	/* Enable port 80 POST on LPC */
	RCBA32(GCS) &= (~0x04);
}

static void set_spi_speed(void)
{
	u32 fdod;
	u8 ssfc;

	/* Observe SPI Descriptor Component Section 0 */
	RCBA32(0x38b0) = 0x1000;

	/* Extract the Write/Erase SPI Frequency from descriptor */
	fdod = RCBA32(0x38b4);
	fdod >>= 24;
	fdod &= 7;

	/* Set Software Sequence frequency to match */
	ssfc = RCBA8(0x3893);
	ssfc &= ~7;
	ssfc |= fdod;
	RCBA8(0x3893) = ssfc;
}

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetch();

	early_pch_init();

	enable_port80_on_lpc();
	set_spi_speed();

	/* Enable upper 128bytes of CMOS */
	RCBA32(RC) = (1 << 2);
}
