/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/early_spi.h>
#include "pch.h"

static void map_rcba(void)
{
	pci_write_config32(PCH_LPC_DEV, RCBA, CONFIG_FIXED_RCBA_MMIO_BASE | 1);
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

void bootblock_early_southbridge_init(void)
{
	map_rcba();
	enable_spi_prefetching_and_caching();
	enable_port80_on_lpc();
	set_spi_speed();

	/* Enable upper 128bytes of CMOS */
	RCBA32(RC) = (1 << 2);

	pch_enable_lpc();
	mainboard_config_superio();
}
