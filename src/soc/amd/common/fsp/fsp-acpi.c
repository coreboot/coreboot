/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <console/console.h>
#include <fsp/util.h>
#include <string.h>
#include <types.h>

struct amd_fsp_acpi_hob_info {
	uint32_t table_size_in_bytes;
	uint8_t total_hobs_for_table;
	uint8_t sequence_number;
	uint16_t reserved;
	uint16_t hob_payload[0xffc8]; /* maximum payload size */
} __packed;

uintptr_t add_agesa_fsp_acpi_table(guid_t guid, const char *name, acpi_rsdp_t *rsdp,
				   uintptr_t current)
{
	const struct amd_fsp_acpi_hob_info *data;
	void *table = (void *)current;
	size_t hob_size;

	data = fsp_find_extension_hob_by_guid(guid.b, &hob_size);
	if (data == NULL) {
		printk(BIOS_ERR, "AGESA %s ACPI table was not found.\n", name);
		return current;
	}

	if (data->table_size_in_bytes > sizeof(data->hob_payload)) {
		printk(BIOS_ERR, "AGESA %s ACPI table size larger than maximum HOB payload "
		       "size.\n", name);
		return current;
	}

	printk(BIOS_INFO, "ACPI:    * %s (AGESA).\n", name);

	memcpy(table, data->hob_payload, data->table_size_in_bytes);

	current += data->table_size_in_bytes;
	acpi_add_table(rsdp, table);
	current = acpi_align_current(current);

	return current;
}
