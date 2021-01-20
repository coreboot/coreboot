/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi.h>
#include <device/device.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
			CONFIG_MMCONF_BASE_ADDRESS, 0, 0, CONFIG_MMCONF_BUS_NUMBER - 1);

	return current;
}
