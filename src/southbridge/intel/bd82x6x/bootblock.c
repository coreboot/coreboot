/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "pch.h"

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	pci_update_config8(PCH_LPC_DEV, BIOS_CNTL, ~(3 << 2), 2 << 2);
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
