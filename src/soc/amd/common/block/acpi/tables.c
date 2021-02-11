/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <types.h>

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
					     CONFIG_MMCONF_BASE_ADDRESS,
					     0,
					     0,
					     CONFIG_MMCONF_BUS_NUMBER - 1);

	return current;
}
