/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 secunet Security Networks AG
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci_def.h>

#include "sandybridge.h"

void sandybridge_init_iommu(void)
{
	const u32 capid0_a = pci_read_config32(PCI_DEV(0, 0, 0), 0xe4);
	if (capid0_a & (1 << 23))
		return;

	/* setup BARs */
	MCHBAR32(0x5404) = IOMMU_BASE1 >> 32;
	MCHBAR32(0x5400) = IOMMU_BASE1 | 1;
	MCHBAR32(0x5414) = IOMMU_BASE2 >> 32;
	MCHBAR32(0x5410) = IOMMU_BASE2 | 1;

	/* lock policies */
	write32((void *)(IOMMU_BASE1 + 0xff0), 0x80000000);

	const struct device *const azalia =
		dev_find_slot(0x00, PCI_DEVFN(0x1b, 0));
	if (azalia && azalia->enabled) {
		write32((void *)(IOMMU_BASE2 + 0xff0), 0x20000000);
		write32((void *)(IOMMU_BASE2 + 0xff0), 0xa0000000);
	} else {
		write32((void *)(IOMMU_BASE2 + 0xff0), 0x80000000);
	}
}
