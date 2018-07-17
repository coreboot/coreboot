/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#include <stdint.h>
#include <string.h>

#include <arch/io.h>
#include <device/pci_def.h>
#include <arch/acpi.h>

#include "gm45.h"

void init_iommu()
{
	/* FIXME: proper test? */
	int me_active = pci_read_config8(PCI_DEV(0, 3, 0), PCI_CLASS_REVISION) != 0xff;
	int stepping = pci_read_config8(PCI_DEV(0, 0, 0), PCI_CLASS_REVISION);

	MCHBAR32(0x28) = IOMMU_BASE1 | 1; /* HDA @ 0:1b.0 */
	if (stepping != STEPPING_B2) {
		/* The official workaround is to run SMM every 64ms.
		   The only winning move is not to play. */
		MCHBAR32(0x18) = IOMMU_BASE2 | 1; /* IGD @ 0:2.0-1 */
	} else {
		/* write-once, so lock it down */
		MCHBAR32(0x18) = 0; /* disable IOMMU for IGD @ 0:2.0-1 */
	}
	if (me_active) {
		MCHBAR32(0x10) = IOMMU_BASE3 | 1; /* ME  @ 0:3.0-3 */
	} else {
		MCHBAR32(0x10) = 0; /* disable IOMMU for ME */
	}
	MCHBAR32(0x20) = IOMMU_BASE4 | 1; /* all other DMA sources */

	/* clear GTT */
	u32 gtt = pci_read_config16(PCI_DEV(0, 0, 0), 0x52);
	if (gtt & 0x400) { /* VT mode */
		pci_devfn_t igd = PCI_DEV(0, 2, 0);

		/* setup somewhere */
		u8 cmd = pci_read_config8(igd, PCI_COMMAND);
		cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
		pci_write_config8(igd, PCI_COMMAND, cmd);
		void *bar = (void *)pci_read_config32(igd, PCI_BASE_ADDRESS_0);

		/* clear GTT, 2MB is enough (and should be safe) */
		memset(bar, 0, 2<<20);

		/* and now disable again */
		cmd &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
		pci_write_config8(igd, PCI_COMMAND, cmd);
		pci_write_config32(igd, PCI_BASE_ADDRESS_0, 0);
	}

	if (stepping == STEPPING_B3) {
		MCHBAR8(0xffc) |= 1 << 4;
		pci_devfn_t peg = PCI_DEV(0, 1, 0);
		/* FIXME: proper test? */
		if (pci_read_config8(peg, PCI_CLASS_REVISION) != 0xff) {
			int val = pci_read_config32(peg, 0xfc) | (1 << 15);
			pci_write_config32(peg, 0xfc, val);
		}
	}

	/* final */
	MCHBAR8(0x94) |= 1 << 3;
}
