/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <boot/tables.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <fmap_config.h>
#include <fw_config.h>
#include <stdbool.h>
#include <version.h>


/* Copy of lb_table_init() implementation for testing purposes */
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

#define LB_RECORD_FOR_EACH(record_ptr, index, header)                                          \
	for (index = 0, record_ptr = lb_first_record(header); index < header->table_entries;   \
	     record_ptr = (struct lb_record *)((uintptr_t)record_ptr + record_ptr->size),      \
	    index++)

static void test_lb_add_gpios(void **state)
{
	struct lb_gpio gpios[] = {
		{-1, ACTIVE_HIGH, 1, "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, 1, "oprom"},
		{-1, ACTIVE_HIGH, 0, "EC in RW"},
	};
	const size_t gpios_buf_size = sizeof(struct lb_gpios) + sizeof(struct lb_gpio) * 32;
	uint8_t gpios_buf[gpios_buf_size];
	struct lb_gpios *gpios_table = (struct lb_gpios *)gpios_buf;
	gpios_table->count = 0;
	gpios_table->size = 0;
	gpios_table->tag = LB_TAG_GPIO;

	/* Add GPIOs an check if they have been added to the table.
	   GPIOs are added in the same order to the end of the table. */
	lb_add_gpios(gpios_table, gpios, ARRAY_SIZE(gpios));
	assert_int_equal(ARRAY_SIZE(gpios), gpios_table->count);
	assert_int_equal(sizeof(gpios), gpios_table->size);
	assert_memory_equal(&gpios_table->gpios[0], gpios, sizeof(gpios));

	/* Add subset of gpios and check if they have been added correctly. */
	lb_add_gpios(gpios_table, &gpios[1], 2);
	assert_int_equal(ARRAY_SIZE(gpios) + 2, gpios_table->count);
	assert_int_equal(sizeof(gpios) + 2 * sizeof(gpios[0]), gpios_table->size);
	assert_memory_equal(&gpios_table->gpios[0], gpios, sizeof(gpios));
	assert_memory_equal(&gpios_table->gpios[ARRAY_SIZE(gpios)], &gpios[1],
			    2 * sizeof(gpios[0]));
}

uint8_t tables_buffer[sizeof(struct lb_header) + 10 * KiB];
static int setup_test_header(void **state)
{
	*state = lb_table_init((uintptr_t)tables_buffer);

	return 0;
}

static void test_lb_new_record(void **state)
{
	struct lb_header *header = *state;
	const size_t entries = 10;
	int i;
	size_t entries_offset;
	size_t accumulated_size = 0;
	struct lb_record *curr;


	assert_int_equal(0, header->table_entries);
	assert_int_equal(0, header->table_bytes);

	lb_new_record(header);
	assert_int_equal(1, header->table_entries);
	assert_int_equal(0, header->table_bytes);

	/* Create few entries with varying sizes (but at least of sizeof(struct lb_record))
	   Accumulate and check size of table after each lb_new_record() call. */
	entries_offset = header->table_entries;
	accumulated_size = sizeof(struct lb_record);
	for (i = 0; i < entries; ++i) {
		curr = lb_new_record(header);
		curr->size = sizeof(struct lb_record) + ((i + 2) * 7) % 32;

		assert_int_equal(entries_offset + (i + 1), header->table_entries);
		assert_int_equal(accumulated_size, header->table_bytes);
		accumulated_size += curr->size;
	}
}

static void test_lb_add_serial(void **state)
{
	struct lb_header *header = *state;
	struct lb_serial serial;

	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = 0xFEDC6000;
	serial.baud = 115200;
	serial.regwidth = 1;
	serial.input_hertz = 115200 * 16;
	serial.uart_pci_addr = 0x0;
	lb_add_serial(&serial, header);

	assert_int_equal(1, header->table_entries);
	/* Table bytes and checksum should be zero, because it is updated with size of previous
	   record or when table is closed. No previous record is present. */
	assert_int_equal(0, header->table_bytes);
	assert_int_equal(0, header->table_checksum);
}

static void test_lb_add_console(void **state)
{
	struct lb_header *header = *state;

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, header);
	assert_int_equal(1, header->table_entries);
	/* Table bytes and checksum should be zero, because it is updated with size of previous
	   record or when table is closed. No previous record is present. */
	assert_int_equal(0, header->table_bytes);
	assert_int_equal(0, header->table_checksum);
}

static void test_multiple_entries(void **state)
{
	struct lb_header *header = *state;

	/* Add two entries */
	lb_add_console(LB_TAG_CONSOLE_SERIAL8250, header);
	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, header);

	assert_int_equal(2, header->table_entries);
	assert_int_equal(sizeof(struct lb_console), header->table_bytes);
}

static void test_write_coreboot_forwarding_table(void **state)
{
	struct lb_header *header = *state;
	uint8_t forwarding_table_buffer[sizeof(struct lb_header)
					+ 2 * sizeof(struct lb_forward)];
	struct lb_header *forward_header =
		(struct lb_header *)ALIGN_UP((uintptr_t)forwarding_table_buffer, 16);
	size_t forwarding_table_size = write_coreboot_forwarding_table(
		(uintptr_t)forwarding_table_buffer, (uintptr_t)header);
	size_t expected_forwarding_table_size =
		ALIGN_UP((uintptr_t)forwarding_table_buffer, 16) + sizeof(struct lb_header)
		+ sizeof(struct lb_forward) - (uintptr_t)forwarding_table_buffer;
	assert_int_equal(expected_forwarding_table_size, forwarding_table_size);

	assert_int_equal(1, forward_header->table_entries);
	assert_int_equal(sizeof(struct lb_forward), forward_header->table_bytes);
	assert_ptr_equal(header,
			 ((struct lb_forward *)lb_first_record(forward_header))->forward);
}

/* Mocks for write_tables() */
const char mainboard_vendor[] = CONFIG_MAINBOARD_VENDOR;
const char mainboard_part_number[] = CONFIG_MAINBOARD_PART_NUMBER;

const char coreboot_version[] = "4.13";
const char coreboot_extra_version[] = "abcdef";
const char coreboot_build[] = "Coreboot build info";
const unsigned int coreboot_version_timestamp = 1617191902U;
const unsigned int coreboot_major_revision = 4;
const unsigned int coreboot_minor_revision = 13;

const char coreboot_compile_time[] = "13:58:22";
const char coreboot_dmi_date[] = "03/31/2021";

const struct bcd_date coreboot_build_date = {
	.century = 0x20,
	.year = 0x20,
	.month = 0x03,
	.day = 0x31,
	.weekday = 0x2,
};

const unsigned int asl_revision = 0x20200925;

void arch_write_tables(uintptr_t coreboot_table)
{
}

static const uintptr_t ebda_base = 0xf0000;
uintptr_t get_coreboot_rsdp(void)
{
	return ebda_base;
}

struct resource mock_bootmem_ranges[] = {
	{.base = 0x1000, .size = 0x2000, .flags = LB_MEM_RAM},
	{.base = 0x0000, .size = 0x4000, .flags = LB_MEM_RAM},
};

void bootmem_write_memory_table(struct lb_memory *mem)
{
	struct lb_memory_range *lb_r = &mem->map[0];
	int i;

	/* Insert entries for testing */
	for (i = 0; i < ARRAY_SIZE(mock_bootmem_ranges); ++i) {
		struct resource *res = &mock_bootmem_ranges[i];
		lb_r->start = pack_lb64(res->base);
		lb_r->size = pack_lb64(res->size);
		lb_r->type = res->flags;
		lb_r++;
		mem->size += sizeof(struct lb_memory_range);
	}
}

void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = 0xFEDC6000;
	serial.baud = 115200;
	serial.regwidth = 1;
	serial.input_hertz = 115200 * 16;
	serial.uart_pci_addr = 0x0;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}

struct cbfs_boot_device cbfs_boot_dev = {
	.rdev = REGION_DEV_INIT(NULL, 0, 0x1000),
	.mcache = (void *)0x1000,
	.mcache_size = 0x1000,
};

const struct cbfs_boot_device *cbfs_get_boot_device(bool force_ro)
{
	return &cbfs_boot_dev;
}

void cbmem_run_init_hooks(int is_recovery)
{
}

extern uintptr_t _cbmem_top_ptr;
void *cbmem_top_chipset(void)
{
	return (void *)_cbmem_top_ptr;
}

#define CBMEM_SIZE (64 * KiB)

static int teardown_write_tables_test(void **state)
{
	free(*state);
	_cbmem_top_ptr = 0;
	return 0;
}

static int setup_write_tables_test(void **state)
{
	/* Allocate more data to have space for alignment */
	void *top_ptr = malloc(CBMEM_SIZE + DYN_CBMEM_ALIGN_SIZE);
	int32_t *mmc_status = NULL;

	if (!top_ptr)
		return -1;

	*state = top_ptr;

	_cbmem_top_ptr = ALIGN_UP((uintptr_t)top_ptr + CBMEM_SIZE, DYN_CBMEM_ALIGN_SIZE);

	cbmem_initialize_empty();

	mmc_status = cbmem_add(CBMEM_ID_MMC_STATUS, sizeof(int32_t));

	if (mmc_status == NULL) {
		teardown_write_tables_test(state);
		return -1;
	}

	*mmc_status = 0x4433AADD;

	return 0;
}

const struct region_device *boot_device_ro(void)
{
	return &cbfs_boot_dev.rdev;
}

uint64_t get_fmap_flash_offset(void)
{
	return FMAP_OFFSET;
}

uint32_t freq_khz = 5000 * 1000;
void lb_arch_add_records(struct lb_header *header)
{
	struct lb_tsc_info *tsc_info;

	tsc_info = (void *)lb_new_record(header);
	tsc_info->tag = LB_TAG_TSC_INFO;
	tsc_info->size = sizeof(*tsc_info);
	tsc_info->freq_khz = freq_khz;
}

static void test_write_tables(void **state)
{
	void *cbtable_start;
	struct lb_header *header;
	struct lb_record *record;
	int32_t *mmc_status = cbmem_find(CBMEM_ID_MMC_STATUS);
	size_t i = 0;

	/* Expect function to store cbtable entry in cbmem */
	cbtable_start = write_tables();
	assert_ptr_equal(cbtable_start, cbmem_find(CBMEM_ID_CBTABLE));

	/* Expect correct lb_header at cbtable_start address */
	header = (struct lb_header *)cbtable_start;
	assert_non_null(header);
	assert_memory_equal("LBIO", header, 4);
	assert_int_equal(sizeof(*header), header->header_bytes);
	/* At least one entry should be present. */
	assert_int_not_equal(0, header->table_entries);

	LB_RECORD_FOR_EACH(record, i, header)
	{
		switch (record->tag) {
		case LB_TAG_MEMORY:
			/* Should be the same as in bootmem_write_memory_table() */
			assert_int_equal(sizeof(struct lb_memory)
						 + ARRAY_SIZE(mock_bootmem_ranges)
							   * sizeof(struct lb_memory_range),
					 record->size);

			const struct lb_memory *memory = (struct lb_memory *)record;
			const struct lb_memory_range *range;
			const struct resource *res;
			struct lb_uint64 value;

			for (int i = 0; i < ARRAY_SIZE(mock_bootmem_ranges); ++i) {
				res = &mock_bootmem_ranges[i];
				range = &memory->map[i];

				value = pack_lb64(res->base);
				assert_memory_equal(&value, &range->start,
						    sizeof(struct lb_uint64));
				value = pack_lb64(res->size);
				assert_memory_equal(&value, &range->size,
						    sizeof(struct lb_uint64));
				assert_int_equal(range->type, res->flags);
			}
			break;
		case LB_TAG_MAINBOARD:
			/* Mainboard record contains its header followed
			   by two null-terminated strings */
			assert_int_equal(ALIGN_UP(sizeof(struct lb_mainboard)
							  + ARRAY_SIZE(mainboard_vendor)
							  + ARRAY_SIZE(mainboard_part_number),
						  8),
					 record->size);
			break;
		case LB_TAG_VERSION:
			assert_int_equal(ALIGN_UP(sizeof(struct lb_string)
							  + ARRAY_SIZE(coreboot_version),
						  8),
					 record->size);
			break;
		case LB_TAG_EXTRA_VERSION:
			assert_int_equal(ALIGN_UP(sizeof(struct lb_string)
							  + ARRAY_SIZE(coreboot_extra_version),
						  8),
					 record->size);
			break;
		case LB_TAG_BUILD:
			assert_int_equal(
				ALIGN_UP(sizeof(struct lb_string) + ARRAY_SIZE(coreboot_build),
					 8),
				record->size);
			break;
		case LB_TAG_COMPILE_TIME:
			assert_int_equal(ALIGN_UP(sizeof(struct lb_string)
							  + ARRAY_SIZE(coreboot_compile_time),
						  8),
					 record->size);
			break;
		case LB_TAG_SERIAL:
			assert_int_equal(sizeof(struct lb_serial), record->size);

			/* This struct have the same values as created in uart_fill_lb() */
			const struct lb_serial *serial = (struct lb_serial *)record;
			assert_int_equal(LB_SERIAL_TYPE_MEMORY_MAPPED, serial->type);
			assert_int_equal(0xFEDC6000, serial->baseaddr);
			assert_int_equal(115200, serial->baud);
			assert_int_equal(1, serial->regwidth);
			assert_int_equal(115200 * 16, serial->input_hertz);
			assert_int_equal(0x0, serial->uart_pci_addr);
			break;
		case LB_TAG_CONSOLE:
			assert_int_equal(sizeof(struct lb_console), record->size);

			/* This struct have the same values as created in uart_fill_lb() */
			const struct lb_console *console = (struct lb_console *)record;
			assert_int_equal(LB_TAG_CONSOLE_SERIAL8250MEM, console->type);
			break;
		case LB_TAG_VERSION_TIMESTAMP:
			assert_int_equal(sizeof(struct lb_timestamp), record->size);

			const struct lb_timestamp *timestamp = (struct lb_timestamp *)record;
			assert_int_equal(coreboot_version_timestamp, timestamp->timestamp);
			break;
		case LB_TAG_BOOT_MEDIA_PARAMS:
			assert_int_equal(sizeof(struct lb_boot_media_params), record->size);

			const struct lb_boot_media_params *bmp =
				(struct lb_boot_media_params *)record;
			const struct cbfs_boot_device *cbd = cbfs_get_boot_device(false);
			const struct region_device *boot_dev = boot_device_ro();
			assert_int_equal(region_device_offset(&cbd->rdev), bmp->cbfs_offset);
			assert_int_equal(region_device_sz(&cbd->rdev), bmp->cbfs_size);
			assert_int_equal(region_device_sz(boot_dev), bmp->boot_media_size);
			assert_int_equal(get_fmap_flash_offset(), bmp->fmap_offset);

			break;
		case LB_TAG_CBMEM_ENTRY:
			assert_int_equal(sizeof(struct lb_cbmem_entry), record->size);

			const struct lb_cbmem_entry *cbmem_entry =
				(struct lb_cbmem_entry *)record;
			const LargestIntegralType expected_tags[] = {CBMEM_ID_CBTABLE,
								     CBMEM_ID_MMC_STATUS};
			assert_in_set(cbmem_entry->id, expected_tags,
				      ARRAY_SIZE(expected_tags));
			break;
		case LB_TAG_TSC_INFO:
			assert_int_equal(sizeof(struct lb_tsc_info), record->size);

			const struct lb_tsc_info *tsc_info = (struct lb_tsc_info *)record;
			assert_int_equal(freq_khz, tsc_info->freq_khz);
			break;
		case LB_TAG_MMC_INFO:
			assert_int_equal(sizeof(struct lb_mmc_info), record->size);

			const struct lb_mmc_info *mmc_info = (struct lb_mmc_info *)record;
			assert_int_equal(*mmc_status, mmc_info->early_cmd1_status);
			break;
		case LB_TAG_BOARD_CONFIG:
			assert_int_equal(sizeof(struct lb_board_config), record->size);

			const struct lb_board_config *board_config =
				(struct lb_board_config *)record;
			const struct lb_uint64 expected_fw_version = pack_lb64(fw_config_get());
			assert_memory_equal(&expected_fw_version, &board_config->fw_config,
					    sizeof(struct lb_uint64));
			assert_int_equal(board_id(), board_config->board_id);
			assert_int_equal(ram_code(), board_config->ram_code);
			assert_int_equal(sku_id(), board_config->sku_id);
			break;
		case LB_TAG_ACPI_RSDP:
			assert_int_equal(sizeof(struct lb_acpi_rsdp), record->size);

			const struct lb_acpi_rsdp *acpi_rsdp = (struct lb_acpi_rsdp *)record;
			assert_int_equal(ebda_base, unpack_lb64(acpi_rsdp->rsdp_pointer));
			break;
		default:
			fail_msg("Unexpected tag found in record. Tag ID: 0x%x", record->tag);
		}
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_lb_add_gpios),
		cmocka_unit_test_setup(test_lb_new_record, setup_test_header),
		cmocka_unit_test_setup(test_lb_add_serial, setup_test_header),
		cmocka_unit_test_setup(test_lb_add_console, setup_test_header),
		cmocka_unit_test_setup(test_multiple_entries, setup_test_header),
		cmocka_unit_test_setup(test_write_coreboot_forwarding_table, setup_test_header),
		cmocka_unit_test_setup_teardown(test_write_tables, setup_write_tables_test,
						teardown_write_tables_test),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
