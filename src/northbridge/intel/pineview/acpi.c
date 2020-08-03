/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <northbridge/intel/pineview/pineview.h>
#include <types.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	u32 length, pciexbar;

	if (!decode_pcie_bar(&pciexbar, &length))
		return current;

	const int max_buses = length / MiB;
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current, pciexbar, 0, 0,
			max_buses - 1);

	return current;
}
