/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <FspGuids.h>
#include <types.h>

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    acpi_rsdp_t *rsdp)
{
	/* TODO: look into adding CRAT */

	/* add ALIB SSDT from HOB */
	current = acpi_align_current(current);
	current = add_agesa_fsp_acpi_table(AMD_FSP_ACPI_ALIB_HOB_GUID, "ALIB", rsdp, current);

	/* IVRS */
	current = acpi_add_ivrs_table(current, rsdp);

	return current;
}
