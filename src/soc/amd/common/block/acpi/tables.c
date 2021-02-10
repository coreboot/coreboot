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
