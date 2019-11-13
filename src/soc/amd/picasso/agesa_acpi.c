/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <fsp/util.h>
#include <FspGuids.h>
#include <soc/acpi.h>
#include <stdint.h>

struct amd_fsp_acpi_hob_info {
	uint32_t table_size_in_bytes;
	uint8_t total_hobs_for_table;
	uint8_t sequence_number;
	uint16_t reserved;
	uint16_t hob_payload[0xffc8];
} __packed;

static uintptr_t add_agesa_acpi_table(guid_t guid, const char *name, acpi_rsdp_t *rsdp,
				      uintptr_t current)
{
	const struct amd_fsp_acpi_hob_info *data;
	void *table = (void *)current;
	size_t hob_size;

	data = fsp_find_extension_hob_by_guid(guid.b, &hob_size);
	if (!data) {
		printk(BIOS_ERR, "AGESA %s ACPI table was not found.\n", name);
		return current;
	}

	printk(BIOS_INFO, "ACPI:    * %s (AGESA).\n", name);

	memcpy(table, data->hob_payload, data->table_size_in_bytes);

	current += data->table_size_in_bytes;
	acpi_add_table(rsdp, table);
	current = acpi_align_current(current);

	return current;
}

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp)
{
	printk(BIOS_DEBUG, "Searching for AGESA FSP ACPI Tables\n");

	current = add_agesa_acpi_table(AMD_FSP_ACPI_SSDT_HOB_GUID, "SSDT", rsdp, current);
	current = add_agesa_acpi_table(AMD_FSP_ACPI_CRAT_HOB_GUID, "CRAT", rsdp, current);
	current = add_agesa_acpi_table(AMD_FSP_ACPI_ALIB_HOB_GUID, "ALIB", rsdp, current);
	current = add_agesa_acpi_table(AMD_FSP_ACPI_IVRS_HOB_GUID, "IVRS", rsdp, current);

	/* Add SRAT, MSCT, SLIT if needed in the future */

	return current;
}
