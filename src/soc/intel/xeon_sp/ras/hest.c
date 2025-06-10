/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_apei.h>
#include <acpi/acpi_gnvs.h>
#include <cbmem.h>
#include <console/console.h>
#include <intelblocks/nvs.h>
#include <soc/acpi.h>

#define MCE_ERR_POLL_MS_INTERVAL	1000
#define HEST_PCIE_RP_AER_DESC_TYPE	6
#define HEST_GHES_DESC_TYPE		9
#define GHES_MAX_RAW_DATA_LENGTH	(((CONFIG_ACPI_HEST_ERROR_LOG_BUFFER_SIZE) >> 1) - 8)
#define GHEST_ERROR_STATUS_BLOCK_LENGTH	((CONFIG_ACPI_HEST_ERROR_LOG_BUFFER_SIZE) >> 1)
#define GHEST_ASSIST			(1 << 2)
#define FIRMWARE_FIRST			(1 << 0)
#define MEM_VALID_BITS			0x66ff
#define PCIE_VALID_BITS			0xef
#define QWORD_ACCESS			4
#define NOTIFY_TYPE_SCI			3

static u32 acpi_hest_add_ghes(uintptr_t current, uintptr_t log_mem)
{
	ghes_record_t *rec = (ghes_record_t *)current;
	u32 size = sizeof(ghes_record_t);

	/* Fill GHES error source descriptor  */
	memset(rec, 0, size);
	rec->esd.type = HEST_GHES_DESC_TYPE;
	rec->esd.source_id = 0; /* 0 for MCE check exception source */
	rec->esd.enabled = 1;
	rec->esd.related_src_id = 0xffff;
	rec->esd.prealloc_erecords = 1;
	rec->esd.max_section_per_record = 0xf;
	rec->max_raw_data_length = GHES_MAX_RAW_DATA_LENGTH;

	/* Add error_status_address */
	rec->sts_addr.space_id = 0;
	rec->sts_addr.bit_width = 0x40;
	rec->sts_addr.bit_offset = 0;
	rec->sts_addr.access_size = QWORD_ACCESS;

	/* Add notification structure */
	rec->notify.type = NOTIFY_TYPE_SCI;
	rec->notify.length = sizeof(acpi_hest_hen_t);
	rec->err_sts_blk_len = GHEST_ERROR_STATUS_BLOCK_LENGTH;

	/* error status block entries start address */
	rec->sts_addr.addr = log_mem;

	return size;
}

uintptr_t acpi_soc_fill_hest(acpi_hest_t *hest, uintptr_t current, void *log_mem)
{
	// fill ACPI global non volatile storage with hest elog addr
	struct global_nvs *gnvs = acpi_get_gnvs();
	if (gnvs)
		gnvs->hest_log_addr = (uintptr_t)log_mem;
	else
		printk(BIOS_ERR, "ACPI fill HEST: Unable to get gnvs\n");

	current = acpi_hest_add_ghes(current, (uintptr_t)log_mem);  // add first entry
	hest->error_source_count += 1;

	return current;
}
