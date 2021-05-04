/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <FspGuids.h>
#include <soc/acpi.h>
#include <types.h>

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp)
{
	/* add ALIB SSDT from HOB */
	current = add_agesa_fsp_acpi_table(AMD_FSP_ACPI_ALIB_HOB_GUID, "ALIB", rsdp, current);

	return current;
}
