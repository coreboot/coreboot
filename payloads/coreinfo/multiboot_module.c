/* SPDX-License-Identifier: GPL-2.0-only */

#include <multiboot_tables.h>
#include "coreinfo.h"

#if CONFIG(MODULE_MULTIBOOT)

#define MAX_MEMORY_COUNT  10

static struct {
	int mem_count;

	struct {
		u64 start;
		u64 size;
		int type;
	} range[MAX_MEMORY_COUNT];
} cb_info;

static int tables_good = 0;

static int multiboot_module_redraw(WINDOW *win)
{
	int row = 2;
	int i;

	print_module_title(win, "Multiboot Tables");

	if (tables_good == 0) {
		mvwprintw(win, row++, 1, "No multiboot tables were found");
		return 0;
	}

	row++;
	mvwprintw(win, row++, 1, "-- Memory Map --");

	for (i = 0; i < cb_info.mem_count; i++) {

		if (cb_info.range[i].type == 1)
			mvwprintw(win, row++, 3, "     RAM: ");
		else
			mvwprintw(win, row++, 3, "Reserved: ");

		wprintw(win, "%16.16llx - %16.16llx",
			cb_info.range[i].start,
			cb_info.range[i].start + cb_info.range[i].size - 1);
	}

	return 0;
}

static void parse_memory(struct multiboot_header *table)
{
	u8 *start = (u8 *) phys_to_virt(table->mmap_addr);
	u8 *ptr = start;
	int i = 0;

	cb_info.mem_count = 0;

	while(ptr < (start + table->mmap_length)) {
		struct multiboot_mmap *mmap = (struct multiboot_mmap *) ptr;

		cb_info.range[i].start = mmap->addr;
		cb_info.range[i].size = mmap->length;
		cb_info.range[i].type = mmap->type;

		if (++cb_info.mem_count == MAX_MEMORY_COUNT)
			return;

		ptr += (mmap->size + sizeof(mmap->size));
		i++;
	}
}

static void parse_header(unsigned long addr)
{
	struct multiboot_header *table = (struct multiboot_header *) addr;

	if (table->flags & MULTIBOOT_FLAGS_MMAP)
		parse_memory(table);
}

static int multiboot_module_init(void)
{
	unsigned long mbaddr;
	tables_good = sysinfo_have_multiboot(&mbaddr);

	parse_header(mbaddr);

	return tables_good ? 0 : -1;
}

struct coreinfo_module multiboot_module = {
	.name = "Multiboot",
	.init = multiboot_module_init,
	.redraw = multiboot_module_redraw,
};

#else

struct coreinfo_module multiboot_module = {
};

#endif /* CONFIG_MODULE_MULTIBOOT */
