/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "i82801gx.h"

static void enable_spi_prefetch(void)
{
	u8 reg8;
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);

	reg8 = pci_read_config8(dev, BIOS_CNTL);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(dev, BIOS_CNTL, reg8);
}

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetch();

	i82801gx_setup_bars();

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Disable watchdog timer */
	RCBA32(GCS) = RCBA32(GCS) | 0x20;

	i82801gx_lpc_setup();
}
