/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"
#include <coreboot_tables.h>

#if CONFIG(MODULE_COREBOOT)

#define MAX_MEMORY_COUNT 5

static struct {
	int mem_count;
	int mem_actual;

	struct cb_memory_range range[MAX_MEMORY_COUNT];

	char vendor[32];
	char part[32];

	char strings[10][64];

	struct cb_serial serial;
	struct cb_console console;
} cb_info;

static int tables_good = 0;

static int coreboot_module_redraw(WINDOW *win)
{
	int row = 2;
	int i;

	print_module_title(win, "coreboot Tables");

	if (tables_good) {
		mvwprintw(win, row++, 1, "No coreboot tables were found");
		return 0;
	}

	mvwprintw(win, row++, 1, "Vendor: %s", cb_info.vendor);
	mvwprintw(win, row++, 1, "Part: %s", cb_info.part);

	mvwprintw(win, row++, 1, "Version: %s%s",
		  cb_info.strings[CB_TAG_VERSION - 0x4],
		  cb_info.strings[CB_TAG_EXTRA_VERSION - 0x4]);

	mvwprintw(win, row++, 1, "Built: %s (%s@%s.%s)",
		  cb_info.strings[CB_TAG_BUILD - 0x4],
		  cb_info.strings[CB_TAG_COMPILE_BY - 0x04],
		  cb_info.strings[CB_TAG_COMPILE_HOST - 0x04],
		  cb_info.strings[CB_TAG_COMPILE_DOMAIN - 0x04]);

	if (cb_info.serial.tag != 0x0) {
		mvwprintw(win, row++, 1, "Serial Port I/O base: 0x%x",
			  cb_info.serial.baseaddr);
	}

	if (cb_info.console.tag != 0x0) {
		mvwprintw(win, row++, 1, "Default Output Console: ");

		switch (cb_info.console.type) {
		case CB_TAG_CONSOLE_SERIAL8250:
			wprintw(win, "Serial Port");
			break;
		case CB_TAG_CONSOLE_VGA:
			wprintw(win, "VGA");
			break;
		case CB_TAG_CONSOLE_BTEXT:
			wprintw(win, "BTEXT");
			break;
		case CB_TAG_CONSOLE_LOGBUF:
			wprintw(win, "Log Buffer");
			break;
		case CB_TAG_CONSOLE_SROM:
			wprintw(win, "Serial ROM");
			break;
		case CB_TAG_CONSOLE_EHCI:
			wprintw(win, "USB Debug");
			break;
		}
	}

	row++;
	mvwprintw(win, row++, 1, "-- Memory Map --");

	for (i = 0; i < cb_info.mem_count; i++) {
		switch (cb_info.range[i].type) {
		case CB_MEM_RAM:
			mvwprintw(win, row++, 3, "     RAM: ");
			break;
		case CB_MEM_RESERVED:
			mvwprintw(win, row++, 3, "Reserved: ");
			break;
		case CB_MEM_TABLE:
			mvwprintw(win, row++, 3, "   Table: ");
		}

		wprintw(win, "%16.16llx - %16.16llx", cb_info.range[i].start,
			cb_info.range[i].start + cb_info.range[i].size - 1);
	}

	return 0;
}

static void parse_memory(unsigned char *ptr)
{
	struct cb_memory *mem = (struct cb_memory *)ptr;
	int max = (MEM_RANGE_COUNT(mem) > MAX_MEMORY_COUNT)
	    ? MAX_MEMORY_COUNT : MEM_RANGE_COUNT(mem);
	int i;

	for (i = 0; i < max; i++) {
		struct cb_memory_range *range =
		    (struct cb_memory_range *)MEM_RANGE_PTR(mem, i);

		memcpy(&cb_info.range[i], range, sizeof(*range));
	}

	cb_info.mem_count = max;
	cb_info.mem_actual = MEM_RANGE_COUNT(mem);
}

static void parse_mainboard(unsigned char *ptr)
{
	struct cb_mainboard *mb = (struct cb_mainboard *)ptr;

	strncpy(cb_info.vendor, cb_mb_vendor_string(mb), sizeof(cb_info.vendor) - 1);
	strncpy(cb_info.part, cb_mb_part_string(mb), sizeof(cb_info.part) - 1);
}

static void parse_strings(unsigned char *ptr)
{
	struct cb_string *string = (struct cb_string *)ptr;
	int index = string->tag - CB_TAG_VERSION;

	strncpy(cb_info.strings[index], (const char *)string->string, 63);
	cb_info.strings[index][63] = 0;
}

static void parse_serial(unsigned char *ptr)
{
	memcpy(&cb_info.serial, (struct cb_serial *)ptr,
	       sizeof(struct cb_serial));
}

static void parse_console(unsigned char *ptr)
{
	memcpy(&cb_info.console, (struct cb_console *)ptr,
	       sizeof(struct cb_console));
}

static int parse_header(void *addr, int len)
{
	struct cb_header *header;
	unsigned char *ptr = (unsigned char *)addr;
	int i;

	for (i = 0; i < len; i += 16, ptr += 16) {
		header = (struct cb_header *)ptr;

		if (!strncmp((const char *)header->signature, "LBIO", 4))
			break;
	}

	/* We walked the entire space and didn't find anything. */
	if (i >= len)
		return -1;

	if (!header->table_bytes)
		return 0;

	/* FIXME: Check the checksum. */

	if (cb_checksum(header, sizeof(*header)))
		return -1;

	if (cb_checksum((ptr + sizeof(*header)), header->table_bytes)
	    != header->table_checksum)
		return -1;

	/* Now, walk the tables. */
	ptr += header->header_bytes;

	for (u32 j = 0; j < header->table_entries; j++) {
		struct cb_record *rec = (struct cb_record *)ptr;

		switch (rec->tag) {
		case CB_TAG_FORWARD:
			return parse_header((void *)(unsigned long)((struct cb_forward *)rec)->forward, 1);
			break;
		case CB_TAG_MEMORY:
			parse_memory(ptr);
			break;
		case CB_TAG_MAINBOARD:
			parse_mainboard(ptr);
			break;
		case CB_TAG_VERSION:
		case CB_TAG_EXTRA_VERSION:
		case CB_TAG_BUILD:
		case CB_TAG_COMPILE_TIME:
		case CB_TAG_COMPILE_BY:
		case CB_TAG_COMPILE_HOST:
		case CB_TAG_COMPILE_DOMAIN:
		case CB_TAG_COMPILER:
		case CB_TAG_LINKER:
		case CB_TAG_ASSEMBLER:
			parse_strings(ptr);
			break;
		case CB_TAG_SERIAL:
			parse_serial(ptr);
			break;
		case CB_TAG_CONSOLE:
			parse_console(ptr);
			break;
		default:
			break;
		}

		ptr += rec->size;
	}

	return 1;
}

static int coreboot_module_init(void)
{
	int ret = parse_header((void *)0x00000, 0x1000);

	if (ret != 1)
		ret = parse_header((void *)0xf0000, 0x1000);

	/* Return error if we couldn't find it at either address. */
	tables_good = (ret == 1) ? 0 : -1;
	return tables_good;
}

struct coreinfo_module coreboot_module = {
	.name = "coreboot",
	.init = coreboot_module_init,
	.redraw = coreboot_module_redraw,
};

#else

struct coreinfo_module coreboot_module = {
};

#endif
