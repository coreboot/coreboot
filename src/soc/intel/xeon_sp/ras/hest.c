/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <cbmem.h>
#include <console/console.h>
#include <soc/hest.h>
#include <intelblocks/nvs.h>

static u64 hest_get_elog_addr(void)
{
	/* The elog address comes from reserved memory */
	struct global_nvs *gnvs;
	gnvs = acpi_get_gnvs();
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to get gnvs\n");
		return 0;
	}

	/* Runtime logging address */
	printk(BIOS_DEBUG, "\t status blk start addr = %llx\n", gnvs->hest_log_addr);
	printk(BIOS_DEBUG, "\t size = %x\n", CONFIG_ERROR_LOG_BUFFER_SIZE);
	return gnvs->hest_log_addr;
}

static u32 acpi_hest_add_ghes(void *current)
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
	if (CONFIG(SOC_ACPI_HEST))
		rec->sts_addr.addr = hest_get_elog_addr();

	return size;
}

static unsigned long acpi_fill_hest(acpi_hest_t *hest)
{
	acpi_header_t *header = &(hest->header);
	void *current;
	current = (void *)(hest);
	void *next = current;
	next = hest + 1;
	next += acpi_hest_add_ghes(next);
	hest->error_source_count += 1;
	header->length += next - current;
	return header->length;
}

unsigned long hest_create(unsigned long current, struct acpi_rsdp *rsdp)
{
	struct global_nvs *gnvs;
	acpi_hest_t *hest;

	/* Reserve memory for Enhanced error logging */
	void *mem = cbmem_add(CBMEM_ID_ACPI_HEST, CONFIG_ERROR_LOG_BUFFER_SIZE);
	if (!mem) {
		printk(BIOS_ERR, "Unable to allocate HEST memory\n");
		return current;
	}

	printk(BIOS_DEBUG, "HEST memory created: %p\n", mem);
	gnvs = acpi_get_gnvs();
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to get gnvs\n");
		return current;
	}
	gnvs->hest_log_addr = (uintptr_t)mem;
	printk(BIOS_DEBUG, "elog_addr: %llx, size:%x\n", gnvs->hest_log_addr,
		CONFIG_ERROR_LOG_BUFFER_SIZE);

	current = ALIGN_UP(current, 8);
	hest = (acpi_hest_t *)current;
	acpi_write_hest(hest, acpi_fill_hest);
	acpi_add_table(rsdp, (void *)current);
	current += hest->header.length;
	return current;
}
