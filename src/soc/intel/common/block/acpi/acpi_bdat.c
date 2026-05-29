/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <assert.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/debug.h>
#include <fsp/util.h>
#include <intelblocks/acpi.h>
#include <intelblocks/acpi_bdat.h>
#include <string.h>

/*
 * gMrcSchemaListHobGuid {3047C2AC-5E8E-4C55-A1CB-EAAD0A88861B}
 * EFI GUID byte order: first 3 fields little-endian, last 2 byte-by-byte.
 */
static const uint8_t intel_fsp_bdat_schema_list_hob_guid[16] = {
	0xac, 0xc2, 0x47, 0x30, 0x8e, 0x5e, 0x55, 0x4c,
	0xa1, 0xcb, 0xea, 0xad, 0x0a, 0x88, 0x86, 0x1b
};

#define GUID_TAIL_BYTES		8
#define CRC16_MSB			0x8000
#define CRC16_MASK			0x1021
#define CRC16_SHIFT_1BIT	1
#define CRC16_SHIFT_8BITS	8
#define CRC16_ITER			8
#define MAX_HOB_COUNT			256

static const uint8_t bdat_header_sign[] = {'B', 'D', 'A', 'T', 'H', 'E', 'A', 'D'};

static void dump_guid_extension_hobs(void)
{
	const struct hob_header *hob = fsp_get_hob_list();

	if (!hob) {
		printk(BIOS_DEBUG, "BDAT: FSP HOB list is NULL\n");
		return;
	}

	printk(BIOS_DEBUG, "BDAT: FSP GUID extension HOBs:\n");

	for (int hob_count = 0; hob->type != HOB_TYPE_END_OF_HOB_LIST && hob_count < MAX_HOB_COUNT;
	     hob = fsp_next_hob(hob), hob_count++) {
		const uint8_t *guid;

		if (hob->length == 0) {
			printk(BIOS_DEBUG, "  malformed HOB: zero length\n");
			return;
		}

		if (hob->type != HOB_TYPE_GUID_EXTENSION)
			continue;

		guid = (const uint8_t *)fsp_hob_header_to_resource(hob);
		printk(BIOS_DEBUG, "  len=%u guid=", hob->length);
		fsp_print_guid(BIOS_DEBUG, guid);
	}
}

static uint16_t get_crc16(const uint8_t *ptr, size_t n_crc)
{
	int i;
	uint16_t crc = 0;

	while (n_crc > 0) {
		n_crc--;
		crc = crc ^ ((int)*ptr++ << CRC16_SHIFT_8BITS);
		for (i = 0; i < CRC16_ITER; ++i)
			if (crc & CRC16_MSB)
				crc = (crc << CRC16_SHIFT_1BIT) ^ CRC16_MASK;
			else
				crc = crc << CRC16_SHIFT_1BIT;
	}
	return crc;
}

enum cb_err acpi_soc_get_bdat_region(void **region)
{
	size_t hob_size = 0;
	const struct bdat_schema_list_hob *schema_list_hob;
	void *buffer = NULL;
	struct bdat_structure *bdat;
	const uint8_t *guid;
	schema_record schema[MAX_SCHEMA_LIST_LENGTH] = {{NULL, 0}};
	uint32_t *schema_offset_list;
	uint8_t *next_block;
	struct bdat_header_structure *bdat_header;

	schema_list_hob = fsp_find_extension_hob_by_guid(
		intel_fsp_bdat_schema_list_hob_guid, &hob_size);

	if (schema_list_hob == NULL || hob_size == 0) {
		printk(BIOS_DEBUG, "No BDAT data exists; missing BDAT Schema List HOB.\n");
		printk(BIOS_DEBUG, "BDAT: expected Schema List GUID: ");
		fsp_print_guid(BIOS_DEBUG, intel_fsp_bdat_schema_list_hob_guid);
		dump_guid_extension_hobs();
		return CB_ERR;
	}
	printk(BIOS_INFO, "Found Schema List HOB, address=%p, hob_size=%zu\n",
			(void *)schema_list_hob, hob_size);

	if (hob_size < offsetof(struct bdat_schema_list_hob, schema_hob_guids)) {
		printk(BIOS_ERR, "BDAT Schema List HOB too small: size=%zu\n", hob_size);
		return CB_ERR;
	}

	if (schema_list_hob->schema_hob_count > MAX_SCHEMA_LIST_LENGTH) {
		printk(BIOS_ERR, "BDAT Schema List HOB count too large: %u\n",
		       schema_list_hob->schema_hob_count);
		return CB_ERR;
	}

	size_t expected_hob_size = offsetof(struct bdat_schema_list_hob, schema_hob_guids) +
		(schema_list_hob->schema_hob_count * sizeof(schema_list_hob->schema_hob_guids[0]));
	if (hob_size < expected_hob_size) {
		printk(BIOS_ERR, "BDAT Schema List HOB truncated: size=%zu, expected=%zu\n",
		       hob_size, expected_hob_size);
		return CB_ERR;
	}

	/* Determine the buffer size. */
	size_t buffer_size = 0;
	uint8_t schema_count = 0;
	for (int index = 0; index < schema_list_hob->schema_hob_count; index++) {
		guid = (const uint8_t *)&schema_list_hob->schema_hob_guids[index];
		fsp_print_guid(BIOS_DEBUG, guid);
		schema[schema_count].address = fsp_find_extension_hob_by_guid(guid, &hob_size);
		printk(BIOS_DEBUG, "Schema HOB pointer: %p, size = %zu\n",
				(void *)schema[schema_count].address, hob_size);
		if (schema[schema_count].address != NULL && hob_size != 0) {
			schema[schema_count].size = hob_size;
			buffer_size += hob_size;
			schema_count++;
		}
	}
	printk(BIOS_DEBUG, "schema_hob_count = %d, schema_count = %d\n",
						schema_list_hob->schema_hob_count, schema_count);

	/* Check if we did not find any schemas */
	if (schema_count == 0) {
		printk(BIOS_ERR, "No BDAT data found.\n");
		return CB_ERR;
	}

	/*
	 * Request the memory from CBMEM and clear it.
	 * This memory is used to store the BDAT into the ACPI table.
	 *           ---------------------------------
	 *          /| BDAT header structure         |
	 *         / |-------------------------------|
	 *        /  | BDAT schema list structure    |
	 *       /   |-------------------------------|
	 *      /    | Schemas[0]                    |--------
	 *   Header  |-------------------------------|       |
	 *      \    | Schemas[1]                    |-------|------
	 *       \   |-------------------------------|       |     |
	 *        \  | ......                        |       |     |
	 *         \ |-------------------------------|       |     |
	 *          \| Schemas[N]                    |-------|-----|------
	 *           |-------------------------------|       |     |     |
	 *           | Schema[0] header structure    |/______|     |     |
	 *           |-------------------------------|\            |     |
	 *           | SSA result data               |             |     |
	 *           | ................              |             |     |
	 *           |-------------------------------|             |     |
	 *           | Schema[1] header structure    |/____________|     |
	 *           |-------------------------------|\                  |
	 *           | SSA result data               |                   |
	 *           | ................              |                   |
	 *           |-------------------------------|                   |
	 *           | ................              |                   |
	 *           |-------------------------------|                   |
	 *           | Schema[N] header structure    |/__________________|
	 *           |-------------------------------|\
	 *           | SSA result data               |
	 *           | ................              |
	 *           |-------------------------------|
	 */
	size_t header_size = sizeof(struct bdat_structure) + (schema_count * sizeof(uint32_t));
	/* Add header size to the total requested buffer size. */
	buffer_size += header_size;
	printk(BIOS_DEBUG, "BDAT buffer size = %zu\n", buffer_size);
	buffer = cbmem_add(CBMEM_ID_ACPI_BDAT, buffer_size);
	if (buffer == NULL) {
		printk(BIOS_ERR, "CBMEM entry for BDAT info missing.\n");
		return CB_ERR;
	}
	memset(buffer, 0, buffer_size);

	/* Copy the RMT data to BDAT structure. */
	bdat = (struct bdat_structure *)buffer;
	next_block = (uint8_t *)((uintptr_t)buffer + header_size);
	schema_offset_list = (uint32_t *)(((uintptr_t)buffer) + sizeof(struct bdat_structure));

	/* Initialize the BDAT structure header. */
	bdat_header = &bdat->header;
	/* Defined in BDAT spec chapter 4. */
	memcpy(bdat_header->bios_data_sig,
			bdat_header_sign, sizeof(bdat_header->bios_data_sig));
	bdat_header->primary_rev = BDAT_PRIMARY_VERSION;
	bdat_header->secondary_rev = BDAT_SECONDARY_VERSION;
	/* Initialize the schema list. */
	bdat->schemas.schema_list_length = schema_count;

	/* Copy the RMT data from each schema location */
	for (int index = 0; index < schema_count; index++) {
		/*
		 * The collection loop only increments schema_count after storing
		 * a valid schema HOB pointer. If this invariant breaks, stop
		 * before emitting a malformed BDAT buffer.
		 */
		assert(schema[index].address != NULL);
		size_t data_size = schema[index].size;
		memcpy(next_block, schema[index].address, data_size);
		/* Update address for each schema */
		schema_offset_list[index] = (uint32_t)((uintptr_t)next_block - (uintptr_t)bdat);
		next_block = (uint8_t *)((uintptr_t)next_block + data_size);
	}

	/* Fill the size and CRC of the BDAT structure. */
	bdat->header.bios_data_size = buffer_size;
	bdat->header.crc16 = get_crc16((const uint8_t *)bdat, buffer_size);
	*region = (void *)buffer;

	return CB_SUCCESS;
}
