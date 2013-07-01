/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#include <stdint.h>
#include <arch/io.h>
#include "gm45.h"

void gm45_early_init(void)
{
	const device_t d0f0 = PCI_DEV(0, 0, 0);

	/* Setup MCHBAR. */
	pci_write_config32(d0f0, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);

	/* Setup DMIBAR. */
	pci_write_config32(d0f0, D0F0_DMIBAR_LO, DEFAULT_DMIBAR | 1);

	/* Setup EPBAR. */
	pci_write_config32(d0f0, D0F0_EPBAR_LO, DEFAULT_EPBAR | 1);

	pci_write_config32(d0f0, D0F0_PMBASE, DEFAULT_PMBASE | 1);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(d0f0, D0F0_PAM(0), 0x30);
	pci_write_config8(d0f0, D0F0_PAM(1), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(2), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(3), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(4), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(5), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(6), 0x33);
}

