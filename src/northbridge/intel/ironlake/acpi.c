/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <types.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include "ironlake.h"

static int decode_pcie_bar(u32 *const base, u32 *const len)
{
	*base = 0;
	*len = 0;

	const u32 pciexbar_reg = pci_read_config32(QPI_SAD, SAD_PCIEXBAR);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: /* 256MB */
		*base = pciexbar_reg & (0x0f << 28);
		*len = 256 * MiB;
		return 1;
	case 1: /* 128M */
		*base = pciexbar_reg & (0x1f << 27);
		*len = 128 * MiB;
		return 1;
	case 2: /* 64M */
		*base = pciexbar_reg & (0x3f << 26);
		*len = 64 * MiB;
		return 1;
	}

	return 0;
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	u32 length, pciexbar;

	if (!decode_pcie_bar(&pciexbar, &length))
		return current;

	const int max_buses = length / MiB;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
					     pciexbar, 0x0, 0x0, max_buses - 1);

	return current;
}
