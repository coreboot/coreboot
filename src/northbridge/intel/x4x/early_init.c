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
#include "x4x.h"

void x4x_early_init(void)
{
	u16 reg16;

	const device_t d0f0 = PCI_DEV(0, 0, 0);

	/* Setup MCHBAR. */
	pci_write_config32(d0f0, D0F0_MCHBAR_LO, (uintptr_t)DEFAULT_MCHBAR | 1);

	/* Setup DMIBAR. */
	pci_write_config32(d0f0, D0F0_DMIBAR_LO, (uintptr_t)DEFAULT_DMIBAR | 1);

	/* Setup EPBAR. */
	pci_write_config32(d0f0, D0F0_EPBAR_LO, DEFAULT_EPBAR | 1);

	/* Setup PMBASE */
	pci_write_config32(d0f0, D0F0_PMBASE, DEFAULT_PMBASE | 1);

	/* Setup HECIBAR */
	pci_write_config32(PCI_DEV(0,3,0), 0x10, DEFAULT_HECIBAR);
	reg16 = pci_read_config16(PCI_DEV(0,3,0), 0x4);
	pci_write_config16(PCI_DEV(0,3,0), 0x4, reg16 | 0x6);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(d0f0, D0F0_PAM(0), 0x30);
	pci_write_config8(d0f0, D0F0_PAM(1), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(2), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(3), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(4), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(5), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(6), 0x33);

	/* Enable internal GFX */
	pci_write_config16(d0f0, D0F0_GGC, 0x0170);

	reg16 = pci_read_config16(d0f0, D0F0_DEVEN);
	pci_write_config16(d0f0, D0F0_DEVEN, reg16 | 0x8);
}
