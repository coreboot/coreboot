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
#include <device/pci_ops.h>
#include <console/post_codes.h>
#include <spi-generic.h>
#include "me.h"
#include "pch.h"

void intel_pch_finalize_smm(void)
{
	/* Lock down Management Engine */
	intel_me_finalize_smm();

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
	RCBA32_OR(0x0050, (1UL << 31));

	/* BIOS Interface Lockdown */
	RCBA32_OR(0x3410, (1 << 0));

	/* Function Disable SUS Well Lockdown */
	RCBA_AND_OR(8, 0x3420, ~0U, (1 << 7));

	/* Global SMI Lock */
	pci_or_config16(PCH_LPC_DEV, 0xa0, 1 << 4);

	/* GEN_PMCON Lock */
	pci_or_config8(PCH_LPC_DEV, 0xa6, (1 << 1) | (1 << 2));

	/* PMSYNC */
	RCBA32_OR(PMSYNC_CONFIG, (1UL << 31));

	/* R/WO registers */
	RCBA32(0x21a4) = RCBA32(0x21a4);
	pci_write_config32(PCI_DEV(0, 27, 0), 0x74,
		    pci_read_config32(PCI_DEV(0, 27, 0), 0x74));

	/* Indicate finalize step with post code */
	outb(POST_OS_BOOT, 0x80);
}
