/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpigen.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <northbridge/intel/pineview/pineview.h>
#include <types.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	u32 length = 0;
	u32 pciexbar = 0;
	int max_buses;

	if (!decode_pciebar(&pciexbar, &length))
		return current;

	max_buses = length >> 20;
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current, pciexbar, 0, 0,
			max_buses - 1);

	return current;
}
