/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Eric Biederman
 * Copyright (C) 2005-2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <ip_checksum.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <arch/coreboot_tables.h>
#include <string.h>
#include <version.h>
#include <device/device.h>
#include <stdlib.h>
#if (CONFIG_USE_OPTION_TABLE == 1)
#include <option_table.h>
#include <cbfs.h>
#endif

static struct lb_header *lb_table_init(unsigned long addr)
{
	struct lb_header *header;

	/* 16 byte align the address */
	addr += 15;
	addr &= ~15;

	header = (void *)addr;
	header->signature[0] = 'L';
	header->signature[1] = 'B';
	header->signature[2] = 'I';
	header->signature[3] = 'O';
	header->header_bytes = sizeof(*header);
	header->header_checksum = 0;
	header->table_bytes = 0;
	header->table_checksum = 0;
	header->table_entries = 0;
	return header;
}

static struct lb_record *lb_first_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = (void *)(((char *)header) + sizeof(*header));
	return rec;
}

static struct lb_record *lb_last_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = (void *)(((char *)header) + sizeof(*header) + header->table_bytes);
	return rec;
}

#if 0
static struct lb_record *lb_next_record(struct lb_record *rec)
{
	rec = (void *)(((char *)rec) + rec->size);
	return rec;
}
#endif

static struct lb_record *lb_new_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = lb_last_record(header);
	if (header->table_entries) {
		header->table_bytes += rec->size;
	}
	rec = lb_last_record(header);
	header->table_entries++;
	rec->tag = LB_TAG_UNUSED;
	rec->size = sizeof(*rec);
	return rec;
}


static struct lb_memory *lb_memory(struct lb_header *header)
{
	struct lb_record *rec;
	struct lb_memory *mem;
	rec = lb_new_record(header);
	mem = (struct lb_memory *)rec;
	mem->tag = LB_TAG_MEMORY;
	mem->size = sizeof(*mem);
	return mem;
}

static struct lb_serial *lb_serial(struct lb_header *header)
{
#if CONFIG_CONSOLE_SERIAL8250
	struct lb_record *rec;
	struct lb_serial *serial;
	rec = lb_new_record(header);
	serial = (struct lb_serial *)rec;
	serial->tag = LB_TAG_SERIAL;
	serial->size = sizeof(*serial);
	serial->ioport = CONFIG_TTYS0_BASE;
	serial->baud = CONFIG_TTYS0_BAUD;
	return serial;
#else
	return header;
#endif
}

static void add_console(struct lb_header *header, u16 consoletype)
{
	struct lb_console *console;

	console = (struct lb_console *)lb_new_record(header);
	console->tag = LB_TAG_CONSOLE;
	console->size = sizeof(*console);
	console->type = consoletype;
}

static void lb_console(struct lb_header *header)
{
#if CONFIG_CONSOLE_SERIAL8250
	add_console(header, LB_TAG_CONSOLE_SERIAL8250);
#endif
#if CONFIG_CONSOLE_LOGBUF
	add_console(header, LB_TAG_CONSOLE_LOGBUF);
#endif
#if CONFIG_USBDEBUG
	add_console(header, LB_TAG_CONSOLE_EHCI);
#endif
}

static void lb_framebuffer(struct lb_header *header)
{
#if defined(CONFIG_BOOTSPLASH) && CONFIG_BOOTSPLASH && CONFIG_COREBOOT_KEEP_FRAMEBUFFER
	void fill_lb_framebuffer(struct lb_framebuffer *framebuffer);

	struct lb_framebuffer *framebuffer;
	framebuffer = (struct lb_framebuffer *)lb_new_record(header);
	framebuffer->tag = LB_TAG_FRAMEBUFFER;
	framebuffer->size = sizeof(*framebuffer);
	fill_lb_framebuffer(framebuffer);
#endif
}

static struct lb_mainboard *lb_mainboard(struct lb_header *header)
{
	struct lb_record *rec;
	struct lb_mainboard *mainboard;
	rec = lb_new_record(header);
	mainboard = (struct lb_mainboard *)rec;
	mainboard->tag = LB_TAG_MAINBOARD;

	mainboard->size = (sizeof(*mainboard) +
		strlen(mainboard_vendor) + 1 +
		strlen(mainboard_part_number) + 1 +
		3) & ~3;

	mainboard->vendor_idx = 0;
	mainboard->part_number_idx = strlen(mainboard_vendor) + 1;

	memcpy(mainboard->strings + mainboard->vendor_idx,
		mainboard_vendor,      strlen(mainboard_vendor) + 1);
	memcpy(mainboard->strings + mainboard->part_number_idx,
		mainboard_part_number, strlen(mainboard_part_number) + 1);

	return mainboard;
}

#if (CONFIG_USE_OPTION_TABLE == 1)
static struct cmos_checksum *lb_cmos_checksum(struct lb_header *header)
{
	struct lb_record *rec;
	struct cmos_checksum *cmos_checksum;
	rec = lb_new_record(header);
	cmos_checksum = (struct cmos_checksum *)rec;
	cmos_checksum->tag = LB_TAG_OPTION_CHECKSUM;

	cmos_checksum->size = (sizeof(*cmos_checksum));

	cmos_checksum->range_start = LB_CKS_RANGE_START * 8;
	cmos_checksum->range_end = ( LB_CKS_RANGE_END * 8 ) + 7;
	cmos_checksum->location = LB_CKS_LOC * 8;
	cmos_checksum->type = CHECKSUM_PCBIOS;

	return cmos_checksum;
}
#endif

static void lb_strings(struct lb_header *header)
{
	static const struct {
		uint32_t tag;
		const char *string;
	} strings[] = {
		{ LB_TAG_VERSION,        coreboot_version,        },
		{ LB_TAG_EXTRA_VERSION,  coreboot_extra_version,  },
		{ LB_TAG_BUILD,          coreboot_build,          },
		{ LB_TAG_COMPILE_TIME,   coreboot_compile_time,   },
		{ LB_TAG_COMPILE_BY,     coreboot_compile_by,     },
		{ LB_TAG_COMPILE_HOST,   coreboot_compile_host,   },
		{ LB_TAG_COMPILE_DOMAIN, coreboot_compile_domain, },
		{ LB_TAG_COMPILER,       coreboot_compiler,       },
		{ LB_TAG_LINKER,         coreboot_linker,         },
		{ LB_TAG_ASSEMBLER,      coreboot_assembler,      },
	};
	unsigned int i;
	for(i = 0; i < ARRAY_SIZE(strings); i++) {
		struct lb_string *rec;
		size_t len;
		rec = (struct lb_string *)lb_new_record(header);
		len = strlen(strings[i].string);
		rec->tag = strings[i].tag;
		rec->size = (sizeof(*rec) + len + 1 + 3) & ~3;
		memcpy(rec->string, strings[i].string, len+1);
	}

}

#if CONFIG_WRITE_HIGH_TABLES == 1
static struct lb_forward *lb_forward(struct lb_header *header, struct lb_header *next_header)
{
	struct lb_record *rec;
	struct lb_forward *forward;
	rec = lb_new_record(header);
	forward = (struct lb_forward *)rec;
	forward->tag = LB_TAG_FORWARD;
	forward->size = sizeof(*forward);
	forward->forward = (uint64_t)(unsigned long)next_header;
	return forward;
}
#endif

void lb_memory_range(struct lb_memory *mem,
	uint32_t type, uint64_t start, uint64_t size)
{
	int entries;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	mem->map[entries].start = pack_lb64(start);
	mem->map[entries].size = pack_lb64(size);
	mem->map[entries].type = type;
	mem->size += sizeof(mem->map[0]);
}

static void lb_reserve_table_memory(struct lb_header *head)
{
	struct lb_record *last_rec;
	struct lb_memory *mem;
	uint64_t start;
	uint64_t end;
	int i, entries;

	last_rec = lb_last_record(head);
	mem = get_lb_mem();
	start = (unsigned long)head;
	end = (unsigned long)last_rec;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	/* Resize the right two memory areas so this table is in
	 * a reserved area of memory.  Everything has been carefully
	 * setup so that is all we need to do.
	 */
	for(i = 0; i < entries; i++ ) {
		uint64_t map_start = unpack_lb64(mem->map[i].start);
		uint64_t map_end = map_start + unpack_lb64(mem->map[i].size);
		/* Does this area need to be expanded? */
		if (map_end == start) {
			mem->map[i].size = pack_lb64(end - map_start);
		}
		/* Does this area need to be contracted? */
		else if (map_start == start) {
			mem->map[i].start = pack_lb64(end);
			mem->map[i].size = pack_lb64(map_end - end);
		}
	}
}

static unsigned long lb_table_fini(struct lb_header *head, int fixup)
{
	struct lb_record *rec, *first_rec;
	rec = lb_last_record(head);
	if (head->table_entries) {
		head->table_bytes += rec->size;
	}

	if (fixup)
		lb_reserve_table_memory(head);

	first_rec = lb_first_record(head);
	head->table_checksum = compute_ip_checksum(first_rec, head->table_bytes);
	head->header_checksum = 0;
	head->header_checksum = compute_ip_checksum(head, sizeof(*head));
	printk(BIOS_DEBUG, "Wrote coreboot table at: %p - %p  checksum %x\n",
		head, rec, head->table_checksum);
	return (unsigned long)rec;
}

static void lb_cleanup_memory_ranges(struct lb_memory *mem)
{
	int entries;
	int i, j;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	/* Sort the lb memory ranges */
	for(i = 0; i < entries; i++) {
		uint64_t entry_start = unpack_lb64(mem->map[i].start);
		for(j = i; j < entries; j++) {
			uint64_t temp_start = unpack_lb64(mem->map[j].start);
			if (temp_start < entry_start) {
				struct lb_memory_range tmp;
				tmp = mem->map[i];
				mem->map[i] = mem->map[j];
				mem->map[j] = tmp;
			}
		}
	}

	/* Merge adjacent entries */
	for(i = 0; (i + 1) < entries; i++) {
		uint64_t start, end, nstart, nend;
		if (mem->map[i].type != mem->map[i + 1].type) {
			continue;
		}
		start  = unpack_lb64(mem->map[i].start);
		end    = start + unpack_lb64(mem->map[i].size);
		nstart = unpack_lb64(mem->map[i + 1].start);
		nend   = nstart + unpack_lb64(mem->map[i + 1].size);
		if ((start <= nstart) && (end > nstart)) {
			if (start > nstart) {
				start = nstart;
			}
			if (end < nend) {
				end = nend;
			}
			/* Record the new region size */
			mem->map[i].start = pack_lb64(start);
			mem->map[i].size  = pack_lb64(end - start);

			/* Delete the entry I have merged with */
			memmove(&mem->map[i + 1], &mem->map[i + 2],
				((entries - i - 2) * sizeof(mem->map[0])));
			mem->size -= sizeof(mem->map[0]);
			entries -= 1;
			/* See if I can merge with the next entry as well */
			i -= 1;
		}
	}
}

static void lb_remove_memory_range(struct lb_memory *mem,
	uint64_t start, uint64_t size)
{
	uint64_t end;
	int entries;
	int i;

	end = start + size;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	/* Remove a reserved area from the memory map */
	for(i = 0; i < entries; i++) {
		uint64_t map_start = unpack_lb64(mem->map[i].start);
		uint64_t map_end   = map_start + unpack_lb64(mem->map[i].size);
		if ((start <= map_start) && (end >= map_end)) {
			/* Remove the completely covered range */
			memmove(&mem->map[i], &mem->map[i + 1],
				((entries - i - 1) * sizeof(mem->map[0])));
			mem->size -= sizeof(mem->map[0]);
			entries -= 1;
			/* Since the index will disappear revisit what will appear here */
			i -= 1;
		}
		else if ((start > map_start) && (end < map_end)) {
			/* Split the memory range */
			memmove(&mem->map[i + 1], &mem->map[i],
				((entries - i) * sizeof(mem->map[0])));
			mem->size += sizeof(mem->map[0]);
			entries += 1;
			/* Update the first map entry */
			mem->map[i].size = pack_lb64(start - map_start);
			/* Update the second map entry */
			mem->map[i + 1].start = pack_lb64(end);
			mem->map[i + 1].size  = pack_lb64(map_end - end);
			/* Don't bother with this map entry again */
			i += 1;
		}
		else if ((start <= map_start) && (end > map_start)) {
			/* Shrink the start of the memory range */
			mem->map[i].start = pack_lb64(end);
			mem->map[i].size  = pack_lb64(map_end - end);
		}
		else if ((start < map_end) && (start > map_start)) {
			/* Shrink the end of the memory range */
			mem->map[i].size = pack_lb64(start - map_start);
		}
	}
}

/* This function is used in mainboard specific code, too */
void lb_add_memory_range(struct lb_memory *mem,
	uint32_t type, uint64_t start, uint64_t size)
{
	lb_remove_memory_range(mem, start, size);
	lb_memory_range(mem, type, start, size);
	lb_cleanup_memory_ranges(mem);
}

static void lb_dump_memory_ranges(struct lb_memory *mem)
{
	int entries;
	int i;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	printk(BIOS_DEBUG, "coreboot memory table:\n");
	for(i = 0; i < entries; i++) {
		uint64_t entry_start = unpack_lb64(mem->map[i].start);
		uint64_t entry_size = unpack_lb64(mem->map[i].size);
		const char *entry_type;

		switch (mem->map[i].type) {
		case LB_MEM_RAM: entry_type="RAM"; break;
		case LB_MEM_RESERVED: entry_type="RESERVED"; break;
		case LB_MEM_ACPI: entry_type="ACPI"; break;
		case LB_MEM_NVS: entry_type="NVS"; break;
		case LB_MEM_UNUSABLE: entry_type="UNUSABLE"; break;
		case LB_MEM_VENDOR_RSVD: entry_type="VENDOR RESERVED"; break;
		case LB_MEM_TABLE: entry_type="CONFIGURATION TABLES"; break;
		default: entry_type="UNKNOWN!"; break;
		}

		printk(BIOS_DEBUG, "%2d. %016llx-%016llx: %s\n",
			i, entry_start, entry_start+entry_size-1, entry_type);

	}
}


/* Routines to extract part so the coreboot table or
 * information from the coreboot table after we have written it.
 * Currently get_lb_mem relies on a global we can change the
 * implementaiton.
 */
static struct lb_memory *mem_ranges = 0;
struct lb_memory *get_lb_mem(void)
{
	return mem_ranges;
}

static void build_lb_mem_range(void *gp, struct device *dev, struct resource *res)
{
	struct lb_memory *mem = gp;
	lb_memory_range(mem, LB_MEM_RAM, res->base, res->size);
}

static struct lb_memory *build_lb_mem(struct lb_header *head)
{
	struct lb_memory *mem;

	/* Record where the lb memory ranges will live */
	mem = lb_memory(head);
	mem_ranges = mem;

	/* Build the raw table of memory */
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_CACHEABLE, IORESOURCE_MEM | IORESOURCE_CACHEABLE,
		build_lb_mem_range, mem);
	lb_cleanup_memory_ranges(mem);
	return mem;
}

static void lb_add_rsvd_range(void *gp, struct device *dev, struct resource *res)
{
	struct lb_memory *mem = gp;
	lb_add_memory_range(mem, LB_MEM_RESERVED, res->base, res->size);
}

static void add_lb_reserved(struct lb_memory *mem)
{
	/* Add reserved ranges */
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_RESERVE, IORESOURCE_MEM | IORESOURCE_RESERVE,
		lb_add_rsvd_range, mem);
}

#if CONFIG_WRITE_HIGH_TABLES == 1
extern uint64_t high_tables_base, high_tables_size;
#endif

unsigned long write_coreboot_table(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end)
{
	struct lb_header *head;
	struct lb_memory *mem;

#if CONFIG_WRITE_HIGH_TABLES == 1
	printk(BIOS_DEBUG, "Writing high table forward entry at 0x%08lx\n",
			low_table_end);
	head = lb_table_init(low_table_end);
	lb_forward(head, (struct lb_header*)rom_table_end);

	low_table_end = (unsigned long) lb_table_fini(head, 0);
	printk(BIOS_DEBUG, "New low_table_end: 0x%08lx\n", low_table_end);
	printk(BIOS_DEBUG, "Now going to write high coreboot table at 0x%08lx\n",
			rom_table_end);

	head = lb_table_init(rom_table_end);
	rom_table_end = (unsigned long)head;
	printk(BIOS_DEBUG, "rom_table_end = 0x%08lx\n", rom_table_end);
#else
	if(low_table_end > (0x1000 - sizeof(struct lb_header))) { /* after 4K */
		/* We need to put lbtable on  to [0xf0000,0x100000) */
		head = lb_table_init(rom_table_end);
		rom_table_end = (unsigned long)head;
	} else {
		head = lb_table_init(low_table_end);
		low_table_end = (unsigned long)head;
	}
#endif

	printk(BIOS_DEBUG, "Adjust low_table_end from 0x%08lx to ", low_table_end);
	low_table_end += 0xfff; // 4K aligned
	low_table_end &= ~0xfff;
	printk(BIOS_DEBUG, "0x%08lx \n", low_table_end);

	/* The Linux kernel assumes this region is reserved */
	printk(BIOS_DEBUG, "Adjust rom_table_end from 0x%08lx to ", rom_table_end);
	rom_table_end += 0xffff; // 64K align
	rom_table_end &= ~0xffff;
	printk(BIOS_DEBUG, "0x%08lx \n", rom_table_end);

#if (CONFIG_USE_OPTION_TABLE == 1)
	{
		struct cmos_option_table *option_table = cbfs_find_file("cmos_layout.bin", 0x1aa);
		if (option_table) {
			struct lb_record *rec_dest = lb_new_record(head);
			/* Copy the option config table, it's already a lb_record... */
			memcpy(rec_dest,  option_table, option_table->size);
			/* Create cmos checksum entry in coreboot table */
			lb_cmos_checksum(head);
		} else {
			printk(BIOS_ERR, "cmos_layout.bin could not be found!\n");
		}
	}
#endif
	/* Record where RAM is located */
	mem = build_lb_mem(head);

	/* Record the mptable and the the lb_table (This will be adjusted later) */
	lb_add_memory_range(mem, LB_MEM_TABLE,
		low_table_start, low_table_end - low_table_start);

	/* Record the pirq table, acpi tables, and maybe the mptable */
	lb_add_memory_range(mem, LB_MEM_TABLE,
		rom_table_start, rom_table_end-rom_table_start);

#if CONFIG_WRITE_HIGH_TABLES == 1
	printk(BIOS_DEBUG, "Adding high table area\n");
	// should this be LB_MEM_ACPI?
	lb_add_memory_range(mem, LB_MEM_TABLE,
		high_tables_base, high_tables_size);
#endif

	/* Add reserved regions */
	add_lb_reserved(mem);

#if (CONFIG_HAVE_MAINBOARD_RESOURCES == 1)
	add_mainboard_resources(mem);
#endif

	lb_dump_memory_ranges(mem);

	/* Note:
	 * I assume that there is always memory at immediately after
	 * the low_table_end.  This means that after I setup the coreboot table.
	 * I can trivially fixup the reserved memory ranges to hold the correct
	 * size of the coreboot table.
	 */

	/* Record our motherboard */
	lb_mainboard(head);
	/* Record the serial port, if present */
	lb_serial(head);
	/* Record our console setup */
	lb_console(head);
	/* Record our various random string information */
	lb_strings(head);
	/* Record our framebuffer */
	lb_framebuffer(head);

	/* Remember where my valid memory ranges are */
	return lb_table_fini(head, 1);

}
