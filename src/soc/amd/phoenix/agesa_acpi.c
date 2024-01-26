/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Make common? */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <commonlib/bsd/helpers.h>
#include <device/device.h>
#include <FspGuids.h>
#include <soc/acpi.h>
#include <types.h>

uintptr_t soc_acpi_write_tables(const struct device *device, uintptr_t current,
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
