/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include "i945.h"

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
