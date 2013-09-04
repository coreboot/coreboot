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
#include <boot/coreboot_tables.h>
#include <string.h>
#include <version.h>
#include <device/device.h>
#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <memrange.h>
#if CONFIG_USE_OPTION_TABLE
#include <option_table.h>
#endif
#if CONFIG_CHROMEOS
#if CONFIG_GENERATE_ACPI_TABLES
#include <arch/acpi.h>
#endif
#include <vendorcode/google/chromeos/chromeos.h>
#include <vendorcode/google/chromeos/gnvs.h>
#endif
#if CONFIG_ARCH_X86
#include <cpu/x86/mtrr.h>
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
	serial->type = LB_SERIAL_TYPE_IO_MAPPED;
	serial->baseaddr = CONFIG_TTYS0_BASE;
	serial->baud = CONFIG_TTYS0_BAUD;
	return serial;
#elif CONFIG_CONSOLE_SERIAL8250MEM || CONFIG_CONSOLE_SERIAL_UART
	if (uartmem_getbaseaddr()) {
		struct lb_record *rec;
		struct lb_serial *serial;
		rec = lb_new_record(header);
		serial = (struct lb_serial *)rec;
		serial->tag = LB_TAG_SERIAL;
		serial->size = sizeof(*serial);
		serial->type = LB_SERIAL_TYPE_MEMORY_MAPPED;
		serial->baseaddr = uartmem_getbaseaddr();
		serial->baud = CONFIG_TTYS0_BAUD;
		return serial;
	} else {
		return NULL;
	}
#else
	return NULL;
#endif
}

#if CONFIG_CONSOLE_SERIAL || CONFIG_CONSOLE_LOGBUF || CONFIG_USBDEBUG
static void add_console(struct lb_header *header, u16 consoletype)
{
	struct lb_console *console;

	console = (struct lb_console *)lb_new_record(header);
	console->tag = LB_TAG_CONSOLE;
	console->size = sizeof(*console);
	console->type = consoletype;
}
#endif

static void lb_console(struct lb_header *header)
{
#if CONFIG_CONSOLE_SERIAL8250
	add_console(header, LB_TAG_CONSOLE_SERIAL8250);
#endif
#if CONFIG_CONSOLE_SERIAL8250MEM || CONFIG_CONSOLE_SERIAL_UART
	add_console(header, LB_TAG_CONSOLE_SERIAL8250MEM);
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
#if CONFIG_FRAMEBUFFER_KEEP_VESA_MODE || CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	void fill_lb_framebuffer(struct lb_framebuffer *framebuffer);
	int vbe_mode_info_valid(void);

	// If there isn't any mode info to put in the table, don't ask for it
	// to be filled with junk.
	if (!vbe_mode_info_valid())
		return;
	struct lb_framebuffer *framebuffer;
	framebuffer = (struct lb_framebuffer *)lb_new_record(header);
	framebuffer->tag = LB_TAG_FRAMEBUFFER;
	framebuffer->size = sizeof(*framebuffer);
	fill_lb_framebuffer(framebuffer);
#endif
}

#if CONFIG_CHROMEOS
static void lb_gpios(struct lb_header *header)
{
	struct lb_gpios *gpios;
	gpios = (struct lb_gpios *)lb_new_record(header);
	gpios->tag = LB_TAG_GPIO;
	gpios->size = sizeof(*gpios);
	gpios->count = 0;
	fill_lb_gpios(gpios);
}

static void lb_vdat(struct lb_header *header)
{
#if CONFIG_GENERATE_ACPI_TABLES
	struct lb_vdat* vdat;

	vdat = (struct lb_vdat *)lb_new_record(header);
	vdat->tag = LB_TAG_VDAT;
	vdat->size = sizeof(*vdat);
	acpi_get_vdat_info(&vdat->vdat_addr, &vdat->vdat_size);
#endif
}

static void lb_vbnv(struct lb_header *header)
{
#if CONFIG_PC80_SYSTEM
	struct lb_vbnv* vbnv;

	vbnv = (struct lb_vbnv *)lb_new_record(header);
	vbnv->tag = LB_TAG_VBNV;
	vbnv->size = sizeof(*vbnv);
	vbnv->vbnv_start = CONFIG_VBNV_OFFSET + 14;
	vbnv->vbnv_size = CONFIG_VBNV_SIZE;
#endif
}

#if CONFIG_VBOOT_VERIFY_FIRMWARE
static void lb_vboot_handoff(struct lb_header *header)
{
	void *addr;
	uint32_t size;
	struct lb_vboot_handoff* vbho;

	if (vboot_get_handoff_info(&addr, &size))
		return;

	vbho = (struct lb_vboot_handoff *)lb_new_record(header);
	vbho->tag = LB_TAB_VBOOT_HANDOFF;
	vbho->size = sizeof(*vbho);
	vbho->vboot_handoff_addr = (intptr_t)addr;
	vbho->vboot_handoff_size = size;
}
#else
static inline void lb_vboot_handoff(struct lb_header *header) {}
#endif /* CONFIG_VBOOT_VERIFY_FIRMWARE */
#endif /* CONFIG_CHROMEOS */

static void lb_x86_rom_cache(struct lb_header *header)
{
#if CONFIG_ARCH_X86
	long mtrr_index;
	struct lb_x86_rom_mtrr *lb_x86_rom_mtrr;

	mtrr_index = x86_mtrr_rom_cache_var_index();

	if (mtrr_index < 0)
		return;

	lb_x86_rom_mtrr = (struct lb_x86_rom_mtrr *)lb_new_record(header);
	lb_x86_rom_mtrr->tag = LB_TAG_X86_ROM_MTRR;
	lb_x86_rom_mtrr->size = sizeof(struct lb_x86_rom_mtrr);
	lb_x86_rom_mtrr->index = mtrr_index;
#endif
}

static void add_cbmem_pointers(struct lb_header *header)
{
	/*
	 * These CBMEM sections' addresses are included in the coreboot table
	 * with the appropriate tags.
	 */
	const struct section_id {
		int cbmem_id;
		int table_tag;
	} section_ids[] = {
		{CBMEM_ID_TIMESTAMP, LB_TAG_TIMESTAMPS},
		{CBMEM_ID_CONSOLE, LB_TAG_CBMEM_CONSOLE}
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(section_ids); i++) {
		const struct section_id *sid = section_ids + i;
		struct lb_cbmem_ref *cbmem_ref;
		void *cbmem_addr = cbmem_find(sid->cbmem_id);

		if (!cbmem_addr)
			continue;  /* This section is not present */

		cbmem_ref = (struct lb_cbmem_ref *)lb_new_record(header);
		if (!cbmem_ref) {
			printk(BIOS_ERR, "No more room in coreboot table!\n");
			break;
		}
		cbmem_ref->tag = sid->table_tag;
		cbmem_ref->size = sizeof(*cbmem_ref);
		cbmem_ref->cbmem_addr = (unsigned long)cbmem_addr;
	}
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

#if CONFIG_USE_OPTION_TABLE
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

static unsigned long lb_table_fini(struct lb_header *head)
{
	struct lb_record *rec, *first_rec;
	rec = lb_last_record(head);
	if (head->table_entries) {
		head->table_bytes += rec->size;
	}

	first_rec = lb_first_record(head);
	head->table_checksum = compute_ip_checksum(first_rec, head->table_bytes);
	head->header_checksum = 0;
	head->header_checksum = compute_ip_checksum(head, sizeof(*head));
	printk(BIOS_DEBUG,
	       "Wrote coreboot table at: %p, 0x%x bytes, checksum %x\n",
	       head, head->table_bytes, head->table_checksum);
	return (unsigned long)rec + rec->size;
}

/* Routines to extract part so the coreboot table or
 * information from the coreboot table after we have written it.
 * Currently get_lb_mem relies on a global we can change the
 * implementation.
 */
static struct lb_memory *mem_ranges = NULL;

struct lb_memory *get_lb_mem(void)
{
	return mem_ranges;
}

/* This structure keeps track of the coreboot table memory ranges. */
static struct memranges lb_ranges;

static struct lb_memory *build_lb_mem(struct lb_header *head)
{
	struct lb_memory *mem;

	/* Record where the lb memory ranges will live */
	mem = lb_memory(head);
	mem_ranges = mem;

	/* Fill the memory map out. The order of operations is important in
	 * that each overlapping range will take over the next. Therefore,
	 * add cacheable resources as RAM then add the reserved resources. */
	memranges_init(&lb_ranges, IORESOURCE_CACHEABLE,
	               IORESOURCE_CACHEABLE, LB_MEM_RAM);
	memranges_add_resources(&lb_ranges, IORESOURCE_RESERVE,
	                        IORESOURCE_RESERVE, LB_MEM_RESERVED);

	return mem;
}

static void commit_lb_memory(struct lb_memory *mem)
{
	struct range_entry *r;
	struct lb_memory_range *lb_r;
	int i;

	lb_r = &mem->map[0];
	i = 0;

	memranges_each_entry(r, &lb_ranges) {
		const char *entry_type;

		switch (range_entry_tag(r)) {
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
			i, range_entry_base(r), range_entry_end(r)-1,
			entry_type);

		lb_r->start = pack_lb64(range_entry_base(r));
		lb_r->size = pack_lb64(range_entry_size(r));
		lb_r->type = range_entry_tag(r);

		i++;
		lb_r++;
		mem->size += sizeof(struct lb_memory_range);
	}
}

void lb_add_memory_range(struct lb_memory *mem,
	uint32_t type, uint64_t start, uint64_t size)
{
	memranges_insert(&lb_ranges, start, size, type);
}


unsigned long write_coreboot_table(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end)
{
	struct lb_header *head;
	struct lb_memory *mem;

	if (low_table_start || low_table_end) {
		printk(BIOS_DEBUG, "Writing table forward entry at 0x%08lx\n",
				low_table_end);
		head = lb_table_init(low_table_end);
		lb_forward(head, (struct lb_header*)rom_table_end);

		low_table_end = (unsigned long) lb_table_fini(head);
		printk(BIOS_DEBUG, "Table forward entry ends at 0x%08lx.\n",
			low_table_end);
		low_table_end = ALIGN(low_table_end, 4096);
		printk(BIOS_DEBUG, "... aligned to 0x%08lx\n", low_table_end);
	}

	printk(BIOS_DEBUG, "Writing coreboot table at 0x%08lx\n",
		rom_table_end);

	head = lb_table_init(rom_table_end);
	rom_table_end = (unsigned long)head;
	printk(BIOS_DEBUG, "rom_table_end = 0x%08lx\n", rom_table_end);
	rom_table_end = ALIGN(rom_table_end, (64 * 1024));
	printk(BIOS_DEBUG, "... aligned to 0x%08lx\n", rom_table_end);

#if CONFIG_USE_OPTION_TABLE
	{
		struct cmos_option_table *option_table = cbfs_get_file_content(
				CBFS_DEFAULT_MEDIA, "cmos_layout.bin",
				CBFS_COMPONENT_CMOS_LAYOUT);
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

	/* The Linux kernel assumes this region is reserved */
	/* Record where RAM is located */
	mem = build_lb_mem(head);

	if (low_table_start || low_table_end) {
		/* Record the mptable and the the lb_table.
		 * (This will be adjusted later)  */
		lb_add_memory_range(mem, LB_MEM_TABLE,
			low_table_start, low_table_end - low_table_start);
	}

	/* Record the pirq table, acpi tables, and maybe the mptable. However,
	 * these only need to be added when the rom_table is sitting below
	 * 1MiB. If it isn't that means high tables are being written.
	 * The code below handles high tables correctly. */
	if (rom_table_end <= (1 << 20))
		lb_add_memory_range(mem, LB_MEM_TABLE,
			rom_table_start, rom_table_end - rom_table_start);

	cbmem_add_lb_mem(mem);

	/* No other memory areas can be added after the memory table has been
	 * committed as the entries won't show up in the serialize mem table. */
	commit_lb_memory(mem);

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
	/* Communicate x86 variable MTRR ROM cache information. */
	lb_x86_rom_cache(head);

#if CONFIG_CHROMEOS
	/* Record our GPIO settings (ChromeOS specific) */
	lb_gpios(head);

	/* pass along the VDAT buffer address */
	lb_vdat(head);

	/* pass along VBNV offsets in CMOS */
	lb_vbnv(head);

	/* pass along the vboot_handoff address. */
	lb_vboot_handoff(head);
#endif
	add_cbmem_pointers(head);

	/* Remember where my valid memory ranges are */
	return lb_table_fini(head);
}
