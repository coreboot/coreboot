/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbmem.h>
#include <nhlt.h>
#include <soc/nvs.h>

uintptr_t nhlt_soc_serialize(struct nhlt *nhlt, uintptr_t acpi_addr)
{
	return nhlt_soc_serialize_oem_overrides(nhlt, acpi_addr, NULL, NULL, 0);
}

uintptr_t nhlt_soc_serialize_oem_overrides(struct nhlt *nhlt,
	uintptr_t acpi_addr, const char *oem_id, const char *oem_table_id,
	uint32_t oem_revision)
{
	global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs == NULL)
		return acpi_addr;

	/* Update NHLT GNVS Data */
	gnvs->nhla = (uintptr_t)acpi_addr;
	gnvs->nhll = nhlt_current_size(nhlt);

	return nhlt_serialize_oem_overrides(nhlt, acpi_addr,
					oem_id, oem_table_id, oem_revision);
}
