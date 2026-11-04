/* SPDX-License-Identifier: GPL-2.0-only */

#include <limits.h>
#include <lib.h>
#include <tests/test.h>
#include <helpers/file.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <vendorcode/amd/psp/psp_fw_parser.h>
#include <vendorcode/amd/psp/internal.h>

struct psp_init_test_state {
	char *fname_base;
	size_t num_entries;
	enum psp_dir_type type;
	int level;
};

struct psp_test_state {
	char *raw_filename;
	size_t raw_file_sz;
	size_t num_entries;
	enum psp_dir_type type;
	int level;
	bool compressed;
};

static int teardown_psp_file(void **state)
{
	struct psp_test_state *s = *state;

	test_free(s->raw_filename);
	test_free(s);

	return 0;
}

/* Set data file with prestate */
static int setup_psp_file(void **state)
{
	int ret = 0;
	struct psp_init_test_state *init_state = *state;
	const char path_prefix[] = "soc/amd/psp_fw/%s";
	struct psp_test_state *s = test_malloc(sizeof(*s));
	memset(s, 0, sizeof(*s));

	if (!s)
		return 1;

	s->num_entries = init_state->num_entries;
	s->type = init_state->type;
	s->level = init_state->level;
	const size_t raw_filename_size =
		strlen(path_prefix) + strlen(init_state->fname_base);
	s->raw_filename = test_malloc(raw_filename_size);
	s->compressed = false;

	if (!s->raw_filename) {
		print_error("File path allocation error\n");
		ret = 2;
		goto error;
	}

	snprintf(s->raw_filename, raw_filename_size, path_prefix, init_state->fname_base);

	int file_size = test_get_file_size(s->raw_filename);

	if (file_size <= 0) {
		print_error("Unable to open file: %s\n", s->raw_filename);
		ret = 3;
		goto error;
	}

	s->raw_file_sz = file_size;

	if (strlen(s->raw_filename) > 5 &&
	    strncmp(&s->raw_filename[strlen(s->raw_filename) - 5], ".lzma", 5) == 0)
		s->compressed = true;

	*state = s;
	return 0;
error:
	teardown_psp_file((void **)&s);
	return ret;
}

static void test_psp_header(void **state)
{
	struct psp_test_state *s = *state;
	uint8_t *raw_buf = test_malloc(s->raw_file_sz);
	assert_non_null(raw_buf);

	assert_int_equal(s->raw_file_sz,
		test_read_file(s->raw_filename, raw_buf, s->raw_file_sz));

	if (s->compressed) {
		uint8_t *decomp_buf = test_malloc(16 * MiB);
		assert_non_null(decomp_buf);

		assert_int_equal(16 * MiB,
			ulzman(raw_buf, s->raw_file_sz, decomp_buf, 16 * MiB));
		test_free(raw_buf);

		raw_buf = decomp_buf;
		s->raw_file_sz = 16 * MiB;
	}

	struct mem_region_device mem_rdev;
	mem_region_device_ro_init(&mem_rdev, raw_buf, s->raw_file_sz);
	struct psp_dir_region dir_rdev;

	/* Test success */
	assert_int_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 0, s->type, &dir_rdev), CB_SUCCESS);
	assert_int_not_equal(0, psp_dir_get_rdev(&dir_rdev)->region.size);

	/* Match entry count */
	assert_int_equal(s->num_entries, psp_dir_count_entries(&dir_rdev));

	/* Read all entries */
	for (size_t i = 0; i < s->num_entries; i++) {
		union psp_dir_entry entry;
		assert_int_equal(psp_dir_get_entry_by_index(&dir_rdev, i, &entry), CB_SUCCESS);
		switch (s->type) {
		case PSP_DIR_PSP:
			print_message("PSP entry %zu: type 0x%02x, subprog 0x%02x\n", i, entry.psp.type, entry.psp.subprog);
			break;
		case PSP_DIR_BIOS:
			print_message("BIOS entry %zu: type 0x%02x, subprog 0x%02x\n", i, entry.bios.type, entry.bios.subprog);
			break;
		case PSP_DIR_COMBO:
			print_message("COMBO entry %zu: offset 0x%" PRIx64 ", ID 0x%08x\n", i, entry.combo.lvl2_addr, entry.combo.id);
			break;
		case PSP_DIR_ISH:
			print_message("ISH entry %zu: offset 0x%08x, PSPID 0x%08x\n", i, entry.ish.pl2_location, entry.ish.psp_id);
			break;
		case PSP_DIR_EFS:
			print_message("EFS entry %zu: offset 0x%08x, type 0x%08x\n", i, entry.efs.offset, entry.efs.type);
			break;
		default:
			assert_true(false);
		}
	}

	/* Check if cookie matches */
	if (s->type == PSP_DIR_PSP || s->type == PSP_DIR_BIOS || s->type == PSP_DIR_COMBO)
		assert_true(psp_fw_test_header(&mem_rdev.rdev, 0, s->type));

	/* Check if level is correct */
	if (s->type == PSP_DIR_PSP || s->type == PSP_DIR_BIOS || s->type == PSP_DIR_COMBO)
		assert_int_equal(dir_rdev.level, s->level);

	/* Test wrong psp_dir_types */
	for (size_t i = 0; i <= PSP_DIR_ISH; i++) {
		enum psp_dir_type test_type = (enum psp_dir_type)i;
		if (test_type == s->type)
			continue;
		memset(&dir_rdev, 0, sizeof(dir_rdev));

		assert_int_not_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 0, test_type, &dir_rdev), CB_SUCCESS);
	}

	/* Test invalid offset */
	assert_int_not_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 1, s->type, &dir_rdev), CB_SUCCESS);

	/* Test offset out of bounds */
	assert_int_not_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 10000, s->type, &dir_rdev), CB_SUCCESS);

	if (s->type != PSP_DIR_EFS) {
		/* Test checksum error */
		raw_buf[4] ^= 0xff;
		assert_int_not_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 0, s->type, &dir_rdev), CB_SUCCESS);
		raw_buf[4] ^= 0xff;
	}

	/* Test cookie error */
	raw_buf[0] ^= 0xff;
	assert_int_not_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 0, s->type, &dir_rdev), CB_SUCCESS);
	raw_buf[0] ^= 0xff;

	/* Test rdev too small */
	mem_region_device_ro_init(&mem_rdev, raw_buf, 16);
	assert_int_not_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 0, s->type, &dir_rdev), CB_SUCCESS);

	test_free(raw_buf);
}

struct walk_state {
	/* Input */
	enum psp_dir_type type;
	int level;
	uint32_t id;
	/* Output */
	bool seen;	/* Set to true by callback */
};

static bool walk_callback(const struct psp_dir_region *dir_rdev,
			  const size_t level,
			  const size_t index,
			  const union psp_dir_entry *entry,
			  void *user_data)
{
	(void) index;
	(void) level;
	struct walk_state *state = (struct walk_state *)user_data;

	if (dir_rdev->type != state->type)
		return true;

	switch (dir_rdev->type) {
	case PSP_DIR_PSP:
		if (dir_rdev->level != state->level)
			return true;
		if (state->id && entry->psp.type == state->id)
			state->seen = true;
		break;
	case PSP_DIR_BIOS:
		if (dir_rdev->level != state->level)
			return true;
		if (state->id && entry->bios.type == state->id)
			state->seen = true;
		break;
	case PSP_DIR_COMBO:
		if (state->id && entry->combo.id == state->id)
			state->seen = true;
		break;
	case PSP_DIR_ISH:
		if (state->id && entry->ish.psp_id == state->id)
			state->seen = true;
		break;
	case PSP_DIR_EFS:
		if (state->id && entry->efs.type == state->id)
			state->seen = true;
		break;
	default:
		return true;
	}

	return !state->seen;
}

static void test_full_rom(void **state)
{
	struct psp_test_state *s = *state;
	uint8_t *raw_buf = test_malloc(s->raw_file_sz);
	assert_non_null(raw_buf);

	assert_int_equal(s->raw_file_sz,
		test_read_file(s->raw_filename, raw_buf, s->raw_file_sz));

	if (s->compressed) {
		uint8_t *decomp_buf = test_malloc(16 * MiB);
		assert_non_null(decomp_buf);

		assert_int_equal(16 * MiB,
			ulzman(raw_buf, s->raw_file_sz, decomp_buf, 16 * MiB));
		test_free(raw_buf);

		raw_buf = decomp_buf;
		s->raw_file_sz = 16 * MiB;
	}

	struct mem_region_device mem_rdev;
	mem_region_device_ro_init(&mem_rdev, raw_buf, s->raw_file_sz);
	struct psp_dir_region dir_rdev;

	assert_int_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, 0x20000, PSP_DIR_EFS, &dir_rdev), CB_SUCCESS);
	assert_int_equal(2, psp_dir_count_entries(&dir_rdev));
	assert_int_equal(PSP_DIR_EFS, dir_rdev.type);

	/* On full ROMs the efs entry type must not be PSP_DIR_NONE. */
	for (size_t i = 0; i < psp_dir_count_entries(&dir_rdev); i++) {
		union psp_dir_entry entry;
		enum psp_dir_type next_dir_type;
		assert_int_equal(psp_dir_get_entry_by_index(&dir_rdev, i, &entry), CB_SUCCESS);
		assert_int_not_equal(0, entry.efs.offset);
		assert_int_not_equal(PSP_DIR_NONE, entry.efs.type);
		assert_true(psp_entry_is_table_pointer(&dir_rdev, &entry, 0, &next_dir_type));
		assert_int_equal(next_dir_type, entry.efs.type);
	}

	if (s->type == PSP_DIR_ISH) {
		/* Scan for PSP BOOTLOADER in LVL2 */
		struct walk_state walk_state = {
			.type = PSP_DIR_PSP,
			.level = 2,
			.id = AMD_FW_PSP_BOOTLOADER,
			.seen = false,
		};
		psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state);
		assert_true(walk_state.seen);

		/* Scan for BIOS APOB_NV in LVL2 */
		walk_state = (struct walk_state){
			.type = PSP_DIR_BIOS,
			.level = 2,
			.id = AMD_BIOS_APOB_NV,
			.seen = false,
		};
		psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state);
		assert_true(walk_state.seen);

		/* Scan for non existing entry */
		walk_state = (struct walk_state){
			.type = PSP_DIR_BIOS,
			.level = 2,
			.id = AMD_BIOS_INVALID,
			.seen = false,
		};
		psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state);
		assert_false(walk_state.seen);

		/* Scan ISH with ID 0xbc0e1000 */
		walk_state = (struct walk_state){
			.type = PSP_DIR_ISH,
			.level = 0,
			.id = 0xbc0e1000,
			.seen = false,
		};
		psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state);
		assert_true(walk_state.seen);

		/* Scan ISH with ID 0xdeadbeef */
		walk_state = (struct walk_state){
			.type = PSP_DIR_ISH,
			.level = 0,
			.id = 0xdeadbeef,
			.seen = false,
		};
		psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state);
		assert_false(walk_state.seen);
	} else if (s->type == PSP_DIR_PSP) {
		/* Scan for PSP BOOTLOADER in LVL1 */
		struct walk_state walk_state = {
			.type = PSP_DIR_PSP,
			.level = 1,
			.id = AMD_FW_PSP_BOOTLOADER,
			.seen = false,
		};
		assert_false(psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state));
		assert_true(walk_state.seen);

		/* Scan for PSP AMD_FW_PSP_SECURED_OS in LVL1 */
		walk_state = (struct walk_state){
			.type = PSP_DIR_PSP,
			.level = 1,
			.id = AMD_FW_PSP_SECURED_OS,
			.seen = false,
		};
		assert_true(psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state));
		assert_false(walk_state.seen);

		/* Scan for PSP AMD_FW_PSP_SECURED_OS in LVL2 */
		walk_state = (struct walk_state){
			.type = PSP_DIR_PSP,
			.level = 2,
			.id = AMD_FW_PSP_SECURED_OS,
			.seen = false,
		};
		assert_false(psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state));
		assert_true(walk_state.seen);

		/* Scan for BIOS AMD_BIOS_PMUD in LVL1 */
		walk_state = (struct walk_state){
			.type = PSP_DIR_BIOS,
			.level = 1,
			.id = AMD_BIOS_PMUD,
			.seen = false,
		};
		assert_false(psp_dir_walk_recursive(&dir_rdev, 0, 0, NULL, walk_callback, &walk_state));
		assert_true(walk_state.seen);
	}

	/* Test psp_entry_is_table_pointer() */
	{
		enum psp_dir_type next_dir_type;
		union psp_dir_entry entry;

		assert_int_equal(psp_dir_get_entry_by_index(&dir_rdev, 0, &entry), CB_SUCCESS);
		assert_true(psp_entry_is_table_pointer(&dir_rdev, &entry, 0, &next_dir_type));
		assert_int_equal(next_dir_type, PSP_DIR_PSP);

		struct psp_dir_region psp_dir_rdev;
		uint32_t dir_offset = psp_dir_entry_offset(&dir_rdev, &entry);

		/* Validate table and fill next_dir */
		assert_int_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, dir_offset,
								next_dir_type, &psp_dir_rdev), CB_SUCCESS);

		assert_int_equal(psp_dir_rdev.level, 1);

		/* Test psp_fw_psp_is_table_pointer() */
		for (size_t i = 0; i < psp_dir_count_entries(&psp_dir_rdev); i++) {
			union psp_dir_entry entry;
			assert_int_equal(psp_dir_get_entry_by_index(&psp_dir_rdev, i, &entry), CB_SUCCESS);

			if (entry.psp.type == AMD_FW_PSP_L2_PTR ||
			    entry.psp.type == AMD_FW_PSP_RECOVERYAB_A ||
			    entry.psp.type == AMD_FW_PSP_RECOVERYAB_B) {

				assert_true(psp_entry_is_table_pointer(&psp_dir_rdev, &entry, 0, &next_dir_type));
				print_message("PSP pointer Address 0x%llx, next_dir_type 0x%02x\n",
					(uint64_t)entry.psp.addr, next_dir_type);

				/* Validate table and fill next_dir */
				uint32_t flash_rel_offset = psp_dir_entry_offset(&psp_dir_rdev, &entry);
				assert_int_not_equal(flash_rel_offset, 0);

				struct psp_dir_region next_dir;
				assert_int_equal(psp_fw_read_and_validate_table(&mem_rdev.rdev, flash_rel_offset,
										next_dir_type, &next_dir), CB_SUCCESS);
			}
		}
	}

	test_free(raw_buf);
}

#define PSP_TABLE_FILE_TEST(_file_prefix, _num_entries, _type, _level)                       \
	{                                                                                    \
		.name = "test_psp_header(" _file_prefix ")",                                 \
		.test_func = test_psp_header, .setup_func = setup_psp_file,                  \
		.teardown_func = teardown_psp_file, .initial_state =                         \
		 &((struct psp_init_test_state) {_file_prefix, _num_entries, _type, _level}) \
	}

#define PSP_FULL_ROM_TEST(_file_prefix, _num_entries, _type, _level)                         \
	{                                                                                    \
		.name = "test_full_rom(" _file_prefix ")",                                   \
		.test_func = test_full_rom, .setup_func = setup_psp_file,                    \
		.teardown_func = teardown_psp_file, .initial_state =                         \
		 &((struct psp_init_test_state) {_file_prefix, _num_entries, _type, _level}) \
	}

int main(void)
{
	const struct CMUnitTest tests[] = {
		PSP_TABLE_FILE_TEST("psp_l1.bin", 2, PSP_DIR_PSP, 1),
		PSP_TABLE_FILE_TEST("psp_l2.bin", 0x36, PSP_DIR_PSP, 2),
		PSP_TABLE_FILE_TEST("bdh_l1.bin", 0x11, PSP_DIR_BIOS, 1),
		PSP_TABLE_FILE_TEST("bdh_l2.bin", 0x11, PSP_DIR_BIOS, 2),
		PSP_TABLE_FILE_TEST("ish.bin", 1, PSP_DIR_ISH, 0),
		PSP_TABLE_FILE_TEST("combo_1.bin", 1, PSP_DIR_COMBO, 1),
		PSP_TABLE_FILE_TEST("combo_2.bin", 1, PSP_DIR_COMBO, 1),
		PSP_TABLE_FILE_TEST("efs1.bin", 2, PSP_DIR_EFS, 0),
		PSP_TABLE_FILE_TEST("efs2.bin", 2, PSP_DIR_EFS, 0),
		PSP_TABLE_FILE_TEST("efs3.bin", 2, PSP_DIR_EFS, 0),
		PSP_FULL_ROM_TEST("full_rom_ish.bin.lzma", 2, PSP_DIR_ISH, 0),
		PSP_FULL_ROM_TEST("full_rom_legacy.bin.lzma", 1, PSP_DIR_PSP, 0),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
