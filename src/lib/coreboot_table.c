/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/cbconfig.h>
#include <console/console.h>
#include <console/uart.h>
#include <ip_checksum.h>
#include <boot/coreboot_tables.h>
#include <boot/tables.h>
#include <boot_device.h>
#include <string.h>
#include <version.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/tpm/tpm_ppi.h>
#include <fmap.h>
#include <fw_config.h>
#include <cbfs.h>
#include <cbmem.h>
#include <bootmem.h>
#include <bootsplash.h>
#include <inttypes.h>
#include <spi_flash.h>
#include <smmstore.h>
#include <types.h>

#if CONFIG(USE_OPTION_TABLE)
#include <option_table.h>
#endif
#if CONFIG(PLATFORM_USES_FSP2_0)
#include <fsp/util.h>
#else
void lb_string_platform_blob_version(struct lb_header *header);
#endif

__weak enum cb_err fill_lb_pcie(struct lb_pcie *pcie)
{
	return CB_ERR_NOT_IMPLEMENTED;
}

static struct lb_header *lb_table_init(unsigned long addr)
{
	struct lb_header *header;

	addr = ALIGN_UP(addr, 16);

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
	rec = (void *)(((char *)header) + sizeof(*header)
		+ header->table_bytes);
	return rec;
}

struct lb_record *lb_new_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = lb_last_record(header);
	if (header->table_entries) {
		assert(IS_ALIGNED(rec->size, LB_ENTRY_ALIGN));
		header->table_bytes += rec->size;
		rec = lb_last_record(header);
	}
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

static void lb_add_serial(struct lb_header *header)
{
	struct lb_serial new_serial = { .tag = LB_TAG_SERIAL,
					.size = sizeof(struct lb_serial),
	};
	if (fill_lb_serial(&new_serial) != CB_SUCCESS)
		return;

	struct lb_serial *serial = (struct lb_serial *)lb_new_record(header);
	memcpy(serial, &new_serial, sizeof(*serial));
	assert(serial->type == LB_SERIAL_TYPE_IO_MAPPED
	       || serial->type == LB_SERIAL_TYPE_MEMORY_MAPPED)
	if (serial->type == LB_SERIAL_TYPE_IO_MAPPED)
		lb_add_console(LB_TAG_CONSOLE_SERIAL8250, header);
	else
		lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, header);
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

static void lb_pcie(struct lb_header *header)
{
	struct lb_pcie pcie = { .tag = LB_TAG_PCIE, .size = sizeof(pcie) };

	if (fill_lb_pcie(&pcie) != CB_SUCCESS)
		return;

	memcpy(lb_new_record(header), &pcie, sizeof(pcie));
}

static void lb_framebuffer(struct lb_header *header)
{
	struct lb_framebuffer *framebuffer;
	struct lb_framebuffer fb = {0};

	if (!CONFIG(LINEAR_FRAMEBUFFER) || fill_lb_framebuffer(&fb))
		return;

	framebuffer = (struct lb_framebuffer *)lb_new_record(header);
	memcpy(framebuffer, &fb, sizeof(*framebuffer));
	framebuffer->tag = LB_TAG_FRAMEBUFFER;
	framebuffer->size = sizeof(*framebuffer);

	if (CONFIG(BOOTSPLASH)) {
		uint8_t *fb_ptr = (uint8_t *)(uintptr_t)framebuffer->physical_address;
		unsigned int width = framebuffer->x_resolution;
		unsigned int height = framebuffer->y_resolution;
		unsigned int depth = framebuffer->bits_per_pixel;
		set_bootsplash(fb_ptr, width, height, depth);
	}
}

void lb_add_gpios(struct lb_gpios *gpios, const struct lb_gpio *gpio_table,
		  size_t count)
{
	size_t table_size = count * sizeof(struct lb_gpio);

	memcpy(&gpios->gpios[gpios->count], gpio_table, table_size);
	gpios->count += count;
	gpios->size += table_size;
}

static void lb_gpios(struct lb_header *header)
{
	struct lb_gpios *gpios;
	struct lb_gpio *g;

	gpios = (struct lb_gpios *)lb_new_record(header);
	gpios->tag = LB_TAG_GPIO;
	gpios->size = sizeof(*gpios);
	gpios->count = 0;
	fill_lb_gpios(gpios);

	printk(BIOS_INFO, "Passing %u GPIOs to payload:\n"
		"            NAME |       PORT | POLARITY |     VALUE\n",
		gpios->count);
	for (g = &gpios->gpios[0]; g < &gpios->gpios[gpios->count]; g++) {
		printk(BIOS_INFO, "%16.16s | ", g->name);
		if (g->port == -1)
			printk(BIOS_INFO, " undefined | ");
		else
			printk(BIOS_INFO, "%#.8x | ", g->port);
		if (g->polarity == ACTIVE_HIGH)
			printk(BIOS_INFO, "    high | ");
		else
			printk(BIOS_INFO, "     low | ");
		switch (g->value) {
		case 0:
			printk(BIOS_INFO, "      low\n");
			break;
		case 1:
			printk(BIOS_INFO, "     high\n");
			break;
		default:
			printk(BIOS_INFO, "undefined\n");
			break;
		}
	}
}

__weak uint32_t board_id(void) { return UNDEFINED_STRAPPING_ID; }
__weak uint32_t ram_code(void) { return UNDEFINED_STRAPPING_ID; }
__weak uint32_t sku_id(void) { return UNDEFINED_STRAPPING_ID; }
__weak uint64_t fw_config_get(void) { return UNDEFINED_FW_CONFIG; }

static void lb_boot_media_params(struct lb_header *header)
{
	struct lb_boot_media_params *bmp;
	const struct region_device *boot_dev;
	const struct cbfs_boot_device *cbd = cbfs_get_boot_device(false);
	if (!cbd)
		return;

	boot_dev = boot_device_ro();
	if (boot_dev == NULL)
		return;

	bmp = (struct lb_boot_media_params *)lb_new_record(header);
	bmp->tag = LB_TAG_BOOT_MEDIA_PARAMS;
	bmp->size = sizeof(*bmp);

	bmp->cbfs_offset = region_device_offset(&cbd->rdev);
	bmp->cbfs_size = region_device_sz(&cbd->rdev);
	bmp->boot_media_size = region_device_sz(boot_dev);

	bmp->fmap_offset = get_fmap_flash_offset();
}

static void lb_mmc_info(struct lb_header *header)
{
	struct lb_mmc_info *rec;
	int32_t *ms_cbmem;

	ms_cbmem = cbmem_find(CBMEM_ID_MMC_STATUS);
	if (!ms_cbmem)
		return;

	rec = (struct lb_mmc_info *)lb_new_record(header);

	rec->tag = LB_TAG_MMC_INFO;
	rec->size = sizeof(*rec);
	rec->early_cmd1_status = *ms_cbmem;
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
		{CBMEM_ID_ACPI_CNVS, LB_TAG_ACPI_CNVS},
		{CBMEM_ID_VPD, LB_TAG_VPD},
		{CBMEM_ID_WIFI_CALIBRATION, LB_TAG_WIFI_CALIBRATION},
		{CBMEM_ID_TCPA_LOG, LB_TAG_TCPA_LOG},
		{CBMEM_ID_FMAP, LB_TAG_FMAP},
		{CBMEM_ID_VBOOT_WORKBUF, LB_TAG_VBOOT_WORKBUF},
		{CBMEM_ID_TYPE_C_INFO, LB_TAG_TYPE_C_INFO},
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

	mainboard->size = ALIGN_UP(sizeof(*mainboard) +
		strlen(mainboard_vendor) + 1 +
		strlen(mainboard_part_number) + 1, LB_ENTRY_ALIGN);

	mainboard->vendor_idx = 0;
	mainboard->part_number_idx = strlen(mainboard_vendor) + 1;

	memcpy(mainboard->strings + mainboard->vendor_idx,
		mainboard_vendor,      strlen(mainboard_vendor) + 1);
	memcpy(mainboard->strings + mainboard->part_number_idx,
		mainboard_part_number, strlen(mainboard_part_number) + 1);

	return mainboard;
}

static struct lb_board_config *lb_board_config(struct lb_header *header)
{
	struct lb_record *rec;
	struct lb_board_config *config;
	rec = lb_new_record(header);
	config = (struct lb_board_config *)rec;

	config->tag = LB_TAG_BOARD_CONFIG;
	config->size = sizeof(*config);

	const uint64_t fw_config = fw_config_get();
	config->board_id = board_id();
	config->ram_code = ram_code();
	config->sku_id = sku_id();
	config->fw_config = fw_config;

	if (config->board_id != UNDEFINED_STRAPPING_ID)
		printk(BIOS_INFO, "Board ID: %d\n", config->board_id);
	if (config->ram_code != UNDEFINED_STRAPPING_ID)
		printk(BIOS_INFO, "RAM code: %d\n", config->ram_code);
	if (config->sku_id != UNDEFINED_STRAPPING_ID)
		printk(BIOS_INFO, "SKU ID: %d\n", config->sku_id);
	if (fw_config != UNDEFINED_FW_CONFIG)
		printk(BIOS_INFO, "FW config: %#" PRIx64 "\n", fw_config);

	return config;
}

#if CONFIG(USE_OPTION_TABLE)
static struct cmos_checksum *lb_cmos_checksum(struct lb_header *header)
{
	struct lb_record *rec;
	struct cmos_checksum *cmos_checksum;
	rec = lb_new_record(header);
	cmos_checksum = (struct cmos_checksum *)rec;
	cmos_checksum->tag = LB_TAG_OPTION_CHECKSUM;

	cmos_checksum->size = (sizeof(*cmos_checksum));

	cmos_checksum->range_start = LB_CKS_RANGE_START * 8;
	cmos_checksum->range_end = (LB_CKS_RANGE_END * 8) + 7;
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
	for (i = 0; i < ARRAY_SIZE(strings); i++) {
		struct lb_string *rec;
		size_t len;
		rec = (struct lb_string *)lb_new_record(header);
		len = strlen(strings[i].string);
		rec->tag = strings[i].tag;
		rec->size = ALIGN_UP(sizeof(*rec) + len + 1, LB_ENTRY_ALIGN);
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

void __weak lb_board(struct lb_header *header) { /* NOOP */ }

/*
 * It's possible that the system is using a SPI flash as the boot device,
 * however it is not probing for devices to fill in specifics. In that
 * case don't provide any information as the correct information is
 * not known.
 */
void __weak lb_spi_flash(struct lb_header *header) { /* NOOP */ }

static struct lb_forward *lb_forward(struct lb_header *header,
	struct lb_header *next_header)
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
		assert(IS_ALIGNED(rec->size, LB_ENTRY_ALIGN));
		head->table_bytes += rec->size;
	}

	first_rec = lb_first_record(head);
	head->table_checksum = compute_ip_checksum(first_rec,
		head->table_bytes);
	head->header_checksum = 0;
	head->header_checksum = compute_ip_checksum(head, sizeof(*head));
	printk(BIOS_DEBUG,
	       "Wrote coreboot table at: %p, 0x%x bytes, checksum %x\n",
	       head, head->table_bytes, head->table_checksum);
	return (unsigned long)rec + rec->size;
}

static void lb_add_acpi_rsdp(struct lb_header *head)
{
	struct lb_acpi_rsdp *acpi_rsdp;
	struct lb_record *rec = lb_new_record(head);
	acpi_rsdp = (struct lb_acpi_rsdp *)rec;
	acpi_rsdp->tag = LB_TAG_ACPI_RSDP;
	acpi_rsdp->size = sizeof(*acpi_rsdp);
	acpi_rsdp->rsdp_pointer = get_coreboot_rsdp();
}

size_t write_coreboot_forwarding_table(uintptr_t entry, uintptr_t target)
{
	struct lb_header *head;

	printk(BIOS_DEBUG, "Writing table forward entry at %p\n",
		(void *)entry);

	head = lb_table_init(entry);
	lb_forward(head, (struct lb_header *)target);

	return (uintptr_t)lb_table_fini(head) - entry;
}

static uintptr_t write_coreboot_table(uintptr_t rom_table_end)
{
	struct lb_header *head;

	printk(BIOS_DEBUG, "Writing coreboot table at 0x%08lx\n",
		(long)rom_table_end);

	head = lb_table_init(rom_table_end);

#if CONFIG(USE_OPTION_TABLE)
	{
		struct cmos_option_table *option_table =
			cbfs_map("cmos_layout.bin", NULL);
		if (option_table) {
			struct lb_record *rec_dest = lb_new_record(head);
			/* Copy the option config table, it's already a
			 * lb_record...
			 */
			memcpy(rec_dest,  option_table, option_table->size);
			/* Create CMOS checksum entry in coreboot table */
			lb_cmos_checksum(head);
		} else {
			printk(BIOS_ERR,
				"cmos_layout.bin could not be found!\n");
		}
	}
#endif

	/* Serialize resource map into mem table types (LB_MEM_*) */
	bootmem_write_memory_table(lb_memory(head));

	/* Record our motherboard */
	lb_mainboard(head);

	/* Record the serial ports and consoles */
	if (CONFIG(CONSOLE_SERIAL))
		lb_add_serial(head);

	if (CONFIG(CONSOLE_USB))
		lb_add_console(LB_TAG_CONSOLE_EHCI, head);

	if (CONFIG(PCI))
		lb_pcie(head);

	/* Record our various random string information */
	lb_strings(head);
	if (CONFIG(PLATFORM_USES_FSP2_0))
		lb_string_platform_blob_version(head);
	lb_record_version_timestamp(head);
	/* Record our framebuffer */
	lb_framebuffer(head);

	/* Record our GPIO settings (ChromeOS specific) */
	if (CONFIG(CHROMEOS))
		lb_gpios(head);

	/* pass along VBNV offsets in CMOS */
	if (CONFIG(VBOOT_VBNV_CMOS))
		lb_table_add_vbnv_cmos(head);

	/* Pass mmc early init status */
	lb_mmc_info(head);

	/* Add SPI flash description if available */
	if (CONFIG(BOOT_DEVICE_SPI_FLASH))
		lb_spi_flash(head);

	add_cbmem_pointers(head);

	/* SMMSTORE v2 */
	if (CONFIG(SMMSTORE_V2))
		lb_smmstorev2(head);

	/* Add board-specific table entries, if any. */
	lb_board(head);

	if (CONFIG(CHROMEOS_RAMOOPS))
		lb_ramoops(head);

	lb_boot_media_params(head);

	/* Board configuration information (including straps) */
	lb_board_config(head);

	if (CONFIG(TPM_PPI))
		lb_tpm_ppi(head);

	/* Add architecture records. */
	lb_arch_add_records(head);

	/* Add all cbmem entries into the coreboot tables. */
	cbmem_add_records_to_cbtable(head);

	if (CONFIG(HAVE_ACPI_TABLES))
		lb_add_acpi_rsdp(head);

	/* Remember where my valid memory ranges are */
	return lb_table_fini(head);
}

void *write_tables(void)
{
	uintptr_t cbtable_start;
	uintptr_t cbtable_end;
	size_t cbtable_size;
	const size_t max_table_size = COREBOOT_TABLE_SIZE;

	cbtable_start = (uintptr_t)cbmem_add(CBMEM_ID_CBTABLE, max_table_size);

	if (!cbtable_start) {
		printk(BIOS_ERR, "Could not add CBMEM for coreboot table.\n");
		return NULL;
	}

	/* Add architecture specific tables. */
	arch_write_tables(cbtable_start);

	/* Write the coreboot table. */
	cbtable_end = write_coreboot_table(cbtable_start);
	cbtable_size = cbtable_end - cbtable_start;

	if (cbtable_size > max_table_size) {
		printk(BIOS_ERR, "%s: coreboot table didn't fit (%zx/%zx)\n",
			__func__, cbtable_size, max_table_size);
	}

	printk(BIOS_DEBUG, "coreboot table: %zd bytes.\n", cbtable_size);

	/* Print CBMEM sections */
	cbmem_list();
	return (void *)cbtable_start;
}
