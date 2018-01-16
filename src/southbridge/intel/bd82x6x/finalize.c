/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/post_codes.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/common/rcba.h>
#include "pch.h"
#include <spi-generic.h>

void intel_pch_finalize_smm(void)
{
	u16 tco1_cnt;
	u16 pmbase;

	if (IS_ENABLED(CONFIG_LOCK_SPI_FLASH_RO) ||
	    IS_ENABLED(CONFIG_LOCK_SPI_FLASH_NO_ACCESS)) {
		/* Copy flash regions from FREG0-4 to PR0-4
		   and enable write protection bit31 */
		int i;
		u32 lockmask = (1 << 31);
		if (IS_ENABLED(CONFIG_LOCK_SPI_FLASH_NO_ACCESS))
			lockmask |= (1 << 15);
		for (i = 0; i < 20; i += 4)
			RCBA32(0x3874 + i) = RCBA32(0x3854 + i) | lockmask;
	}

	/* Set SPI opcode menu */
	RCBA16(0x3894) = SPI_OPPREFIX;
	RCBA16(0x3896) = SPI_OPTYPE;
	RCBA32(0x3898) = SPI_OPMENU_LOWER;
	RCBA32(0x389c) = SPI_OPMENU_UPPER;

	/* Lock SPIBAR */
	RCBA32_OR(0x3804, (1 << 15));

#if IS_ENABLED(CONFIG_SPI_FLASH_SMM)
	/* Re-init SPI driver to handle locked BAR */
	spi_init();
#endif

	/* TCLOCKDN: TC Lockdown */
	RCBA32_OR(0x0050, (1 << 31));

	/* BIOS Interface Lockdown */
	RCBA32_OR(0x3410, (1 << 0));

	/* Function Disable SUS Well Lockdown */
	RCBA_AND_OR(8, 0x3420, ~0U, (1 << 7));

	/* Global SMI Lock */
	pci_or_config16(PCH_LPC_DEV, GEN_PMCON_1, 1 << 4);

	/* GEN_PMCON Lock */
	pci_or_config8(PCH_LPC_DEV, GEN_PMCON_LOCK, (1 << 1) | (1 << 2));

	/* ETR3: CF9GR Lockdown */
	pci_update_config32(PCH_LPC_DEV, ETR3, ~ETR3_CF9GR, ETR3_CF9LOCK);

	/* R/WO registers */
	RCBA32(0x21a4) = RCBA32(0x21a4);
	pci_write_config32(PCI_DEV(0, 27, 0), 0x74,
		    pci_read_config32(PCI_DEV(0, 27, 0), 0x74));

	/* TCO_Lock */
	pmbase = smm_get_pmbase();
	tco1_cnt = inw(pmbase + TCO1_CNT);
	tco1_cnt |= TCO_LOCK;
	outw(tco1_cnt, pmbase + TCO1_CNT);

	/* Indicate finalize step with post code */
	outb(POST_OS_BOOT, 0x80);
}
