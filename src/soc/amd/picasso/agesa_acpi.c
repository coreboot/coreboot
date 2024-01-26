/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <FspGuids.h>
#include <types.h>

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    acpi_rsdp_t *rsdp)
{
	/* CRAT */
	current = acpi_add_crat_table(current, rsdp);

	/* add ALIB SSDT from HOB */
	current = acpi_align_current(current);
	current = add_agesa_fsp_acpi_table(AMD_FSP_ACPI_ALIB_HOB_GUID, "ALIB", rsdp, current);

	/* IVRS */
	current = acpi_add_ivrs_table(current, rsdp);

	/* Add SRAT, MSCT, SLIT if needed in the future */

	return current;
}
