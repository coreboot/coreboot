/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include "x4x.h"

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
			CONFIG_MMCONF_BASE_ADDRESS, 0, 0, CONFIG_MMCONF_BUS_NUMBER - 1);

	return current;
}

unsigned long northbridge_write_acpi_tables(const struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	unsigned long current;
	current = acpi_align_current(start);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}
