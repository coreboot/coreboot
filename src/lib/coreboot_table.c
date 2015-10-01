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
 * Foundation, Inc.
 */

#include <console/console.h>
#include <console/uart.h>
#include <ip_checksum.h>
#include <boot/coreboot_tables.h>
#include <boot_device.h>
#include <string.h>
#include <version.h>
#include <boardid.h>
#include <device/device.h>
#include <fmap.h>
#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <bootmem.h>
#include <spi_flash.h>
#if CONFIG_USE_OPTION_TABLE
#include <option_table.h>
#endif
#if CONFIG_CHROMEOS
#if CONFIG_HAVE_ACPI_TABLES
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

struct lb_record *lb_new_record(struct lb_header *header)
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

void lb_add_serial(struct lb_serial *new_serial, void *data)
{
	struct lb_header *header = (struct lb_header *)data;
	struct lb_serial *serial;

	serial = (struct lb_serial *)lb_new_record(header);
	serial->tag = LB_TAG_SERIAL;
	serial->size = sizeof(*serial);
	serial->type = new_serial->type;
	serial->baseaddr = new_serial->baseaddr;
	serial->baud = new_serial->baud;
	serial->regwidth = new_serial->regwidth;
}

void lb_add_console(uint16_t consoletype, void *data)
{
	struct lb_header *header = (struct lb_header *)data;
	struct lb_console *console;

	console = (struct lb_console *)lb_new_record(header);
	console->tag = LB_TAG_CONSOLE;
	console->size = sizeof(*console);
	console->type = consoletype;
}

void __attribute__((weak)) lb_framebuffer(struct lb_header *header)
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
	fill_lb_framebuffer(framebuffer);
	framebuffer->tag = LB_TAG_FRAMEBUFFER;
	framebuffer->size = sizeof(*framebuffer);
#endif
}

void fill_lb_gpio(struct lb_gpio *gpio, int num,
			 int polarity, const char *name, int value)
{
	memset(gpio, 0, sizeof(*gpio));
	gpio->port = num;
	gpio->polarity = polarity;
	if (value >= 0)
		gpio->value = value;
	strncpy((char *)gpio->name, name, GPIO_MAX_NAME_LENGTH);
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
#if CONFIG_HAVE_ACPI_TABLES
	struct lb_range *vdat;

	vdat = (struct lb_range *)lb_new_record(header);
	vdat->tag = LB_TAG_VDAT;
	vdat->size = sizeof(*vdat);
	acpi_get_vdat_info(&vdat->range_start, &vdat->range_size);
#endif
}

static void lb_vbnv(struct lb_header *header)
{
#if CONFIG_PC80_SYSTEM
	struct lb_range *vbnv;

	vbnv = (struct lb_range *)lb_new_record(header);
	vbnv->tag = LB_TAG_VBNV;
	vbnv->size = sizeof(*vbnv);
	vbnv->range_start = CONFIG_VBNV_OFFSET + 14;
	vbnv->range_size = CONFIG_VBNV_SIZE;
#endif
}

#if CONFIG_VBOOT_VERIFY_FIRMWARE
static void lb_vboot_handoff(struct lb_header *header)
{
	void *addr;
	uint32_t size;
	struct lb_range *vbho;

	if (vboot_get_handoff_info(&addr, &size))
		return;

	vbho = (struct lb_range *)lb_new_record(header);
	vbho->tag = LB_TAB_VBOOT_HANDOFF;
	vbho->size = sizeof(*vbho);
	vbho->range_start = (intptr_t)addr;
	vbho->range_size = size;
}
#else
static inline void lb_vboot_handoff(struct lb_header *header) {}
#endif /* CONFIG_VBOOT_VERIFY_FIRMWARE */
#endif /* CONFIG_CHROMEOS */

static void lb_board_id(struct lb_header *header)
{
#if CONFIG_BOARD_ID_AUTO || CONFIG_BOARD_ID_MANUAL
	struct lb_board_id  *bid;

	bid = (struct lb_board_id *)lb_new_record(header);

	bid->tag = LB_TAG_BOARD_ID;
	bid->size = sizeof(*bid);
	bid->board_id = board_id();
#endif
}

static void lb_boot_media_params(struct lb_header *header)
{
	struct lb_boot_media_params *bmp;
	struct cbfs_props props;
	const struct region_device *boot_dev;
	struct region_device fmrd;

	boot_device_init();

	if (cbfs_boot_region_properties(&props))
		return;

	boot_dev = boot_device_ro();
	if (boot_dev == NULL)
		return;

	bmp = (struct lb_boot_media_params *)lb_new_record(header);
	bmp->tag = LB_TAG_BOOT_MEDIA_PARAMS;
	bmp->size = sizeof(*bmp);

	bmp->cbfs_offset = props.offset;
	bmp->cbfs_size = props.size;
	bmp->boot_media_size = region_device_sz(boot_dev);

	bmp->fmap_offset = ~(uint64_t)0;
	if (find_fmap_directory(&fmrd) == 0) {
		bmp->fmap_offset = region_device_offset(&fmrd);
	}
}

static void lb_ram_code(struct lb_header *header)
{
#if IS_ENABLED(CONFIG_RAM_CODE_SUPPORT)
	struct lb_ram_code *code;

	code = (struct lb_ram_code *)lb_new_record(header);

	code->tag = LB_TAG_RAM_CODE;
	code->size = sizeof(*code);
	code->ram_code = ram_code();
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
		{CBMEM_ID_CONSOLE, LB_TAG_CBMEM_CONSOLE},
		{CBMEM_ID_ACPI_GNVS, LB_TAG_ACPI_GNVS},
		{CBMEM_ID_WIFI_CALIBRATION, LB_TAG_WIFI_CALIBRATION}
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

static void lb_record_version_timestamp(struct lb_header *header)
{
	struct lb_timestamp *rec;
	rec = (struct lb_timestamp *)lb_new_record(header);
	rec->tag = LB_TAG_VERSION_TIMESTAMP;
	rec->size = sizeof(*rec);
	rec->timestamp = coreboot_version_timestamp;
}

void __attribute__((weak)) lb_board(struct lb_header *header) { /* NOOP */ }

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

unsigned long write_coreboot_table(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end)
{
	struct lb_header *head;

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
		struct cmos_option_table *option_table =
			cbfs_boot_map_with_leak("cmos_layout.bin",
				CBFS_COMPONENT_CMOS_LAYOUT, NULL);
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

	/* Initialize the memory map at boot time. */
	bootmem_init();

	if (low_table_start || low_table_end) {
		uint64_t size = low_table_end - low_table_start;
		/* Record the mptable and the the lb_table.
		 * (This will be adjusted later)  */
		bootmem_add_range(low_table_start, size, LB_MEM_TABLE);
	}

	/* Record the pirq table, acpi tables, and maybe the mptable. However,
	 * these only need to be added when the rom_table is sitting below
	 * 1MiB. If it isn't that means high tables are being written.
	 * The code below handles high tables correctly. */
	if (rom_table_end <= (1 << 20)) {
		uint64_t size = rom_table_end - rom_table_start;
		bootmem_add_range(rom_table_start, size, LB_MEM_TABLE);
	}

	/* No other memory areas can be added after the memory table has been
	 * committed as the entries won't show up in the serialize mem table. */
	bootmem_write_memory_table(lb_memory(head));

	/* Record our motherboard */
	lb_mainboard(head);

	/* Record the serial ports and consoles */
#if CONFIG_CONSOLE_SERIAL
	uart_fill_lb(head);
#endif
#if CONFIG_CONSOLE_USB
	lb_add_console(LB_TAG_CONSOLE_EHCI, head);
#endif

	/* Record our various random string information */
	lb_strings(head);
	lb_record_version_timestamp(head);
	/* Record our framebuffer */
	lb_framebuffer(head);

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

	/* Add board ID if available */
	lb_board_id(head);

	/* Add RAM config if available */
	lb_ram_code(head);

#if IS_ENABLED(CONFIG_SPI_FLASH)
	/* Add SPI flash description if available */
	lb_spi_flash(head);
#endif

	add_cbmem_pointers(head);

	/* Add board-specific table entries, if any. */
	lb_board(head);

#if IS_ENABLED(CONFIG_CHROMEOS_RAMOOPS)
	lb_ramoops(head);
#endif

	lb_boot_media_params(head);

	/* Remember where my valid memory ranges are */
	return lb_table_fini(head);
}
