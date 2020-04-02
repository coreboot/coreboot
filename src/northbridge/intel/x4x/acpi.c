/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include "x4x.h"

unsigned long acpi_fill_mcfg(unsigned long current)
{
	u32 pciexbar = 0;
	u32 length = 0;

	if (!decode_pciebar(&pciexbar, &length))
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current,
			pciexbar, 0x0, 0x0, (length >> 20) - 1);

	return current;
}

unsigned long northbridge_write_acpi_tables(struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	unsigned long current;
	current = acpi_align_current(start);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
