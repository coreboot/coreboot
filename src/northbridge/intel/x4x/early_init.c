/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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

#include <stdint.h>
#include <arch/io.h>
#include "iomap.h"
#include <southbridge/intel/i82801gx/i82801gx.h> /* DEFAULT_PMBASE */
#include <pc80/mc146818rtc.h>
#include "x4x.h"

void x4x_early_init(void)
{
	const pci_devfn_t d0f0 = PCI_DEV(0, 0, 0);

	/* Setup MCHBAR. */
	pci_write_config32(d0f0, D0F0_MCHBAR_LO, (uintptr_t)DEFAULT_MCHBAR | 1);

	/* Setup DMIBAR. */
	pci_write_config32(d0f0, D0F0_DMIBAR_LO, (uintptr_t)DEFAULT_DMIBAR | 1);

	/* Setup EPBAR. */
	pci_write_config32(d0f0, D0F0_EPBAR_LO, DEFAULT_EPBAR | 1);

	/* Setup PMBASE */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44 /* ACPI_CNTL */ , 0x80);

	/* Setup HECIBAR */
	pci_write_config32(PCI_DEV(0,3,0), 0x10, DEFAULT_HECIBAR);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(d0f0, D0F0_PAM(0), 0x30);
	pci_write_config8(d0f0, D0F0_PAM(1), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(2), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(3), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(4), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(5), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(6), 0x33);

	/* Enable internal GFX */
	pci_write_config32(d0f0, D0F0_DEVEN, BOARD_DEVEN);
	/* Set preallocated IGD size from cmos */
	u8 gfxsize;

	if (get_option(&gfxsize, "gfx_uma_size") != CB_SUCCESS) {
		/* 6 for 64MB, default if not set in cmos */
		gfxsize = 6;
	}
	pci_write_config16(d0f0, D0F0_GGC, 0x0100 | ((gfxsize + 1) << 4));
}
