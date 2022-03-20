/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/compiler.h>
#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>

#define TIMESTAMP_MS(x) ((x) / 1000ull)

static const uint8_t fpdt_guid[16] = {
	0xfd, 0x7b, 0x38, 0x3b, 0xbc, 0x7a, 0xf2, 0x4c,
	0xa0, 0xca, 0xb6, 0xa1, 0x6c, 0x1b, 0x1b, 0x25,
};

enum fpdt_record_type {
	FPDT_GUID_EVENT		= 0x1010,
	FPDT_STRING_EVENT	= 0x1011,
};

struct perf_record_hdr {
	uint16_t	type;
	uint8_t		length;
	uint8_t		revision;
} __packed;

struct generic_event_record {
	struct perf_record_hdr	header;
	uint16_t		progress_id;
	uint32_t		apic_id;
	uint64_t		timestamp;
	uint8_t			guid[16];
	uint8_t			string[0];
} __packed;

/*
 * Performance Hob:
 *   GUID - fpdt_guid;
 *   Data - FPDT_PEI_EXT_PERF_HEADER one or more FPDT records
*/
struct fpdt_pei_ext_perf_header {
	uint32_t	table_size;
	uint32_t	load_image_count;
	uint32_t	hob_is_full;
} __packed;

static void print_guid_record(const struct generic_event_record *rec)
{
	printk(BIOS_INFO, "%5x\t%16llu\t\t", rec->progress_id, TIMESTAMP_MS(rec->timestamp));
	fsp_print_guid(rec->guid);
	printk(BIOS_INFO, "\n");
}

static void print_string_record(const struct generic_event_record *rec)
{
	size_t str_len = rec->header.length - offsetof(struct generic_event_record, string);
	printk(BIOS_INFO, "%5x\t%16llu\t\t%*s/",
	       rec->progress_id, TIMESTAMP_MS(rec->timestamp), (int)str_len, rec->string);
	fsp_print_guid(rec->guid);
	printk(BIOS_INFO, "\n");
}

static void print_fsp_perf_timestamp(const struct generic_event_record *rec)
{
	switch (rec->header.type) {
	case FPDT_GUID_EVENT:
		print_guid_record(rec);
		break;
	case FPDT_STRING_EVENT:
		print_string_record(rec);
		break;
	default:
		printk(BIOS_INFO, "Unhandled Event Type 0x%x\n", rec->header.type);
		break;
	}
}

static void print_fsp_timestamp_header(void)
{
	printk(BIOS_INFO, "+---------------------------------------------------+\n");
	printk(BIOS_INFO, "|------ FSP Performance Timestamp Table Dump -------|\n");
	printk(BIOS_INFO, "+---------------------------------------------------+\n");
	printk(BIOS_INFO, "| Perf-ID\tTimestamp(ms)\t\tString/GUID |\n");
	printk(BIOS_INFO, "+---------------------------------------------------+\n");
}

void fsp_display_timestamp(void)
{
	size_t size;
	const struct fpdt_pei_ext_perf_header *hdr = fsp_find_extension_hob_by_guid(fpdt_guid,
							 &size);

	if (!hdr || !size) {
		printk(BIOS_INFO, "FPDT Extended Firmware Performance HOB Not Found!\n"
		"Check if PcdFspPerformanceEnable is set to `TRUE` inside FSP package\n");
		return;
	}

	const struct generic_event_record *rec = (const struct generic_event_record *)(
				(uint8_t *)hdr + sizeof(struct fpdt_pei_ext_perf_header));

	print_fsp_timestamp_header();
	for (size_t i = 0; i < hdr->table_size;) {
		print_fsp_perf_timestamp(rec);

		i += rec->header.length;
		rec = (const struct generic_event_record *)((uint8_t *)rec +
				 rec->header.length);
	}
}
