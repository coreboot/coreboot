/* SPDX-License-Identifier: GPL-2.0-only */

#include <limits.h>
#include <tests/test.h>
#include <helpers/file.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <vendorcode/amd/psp/psp_fw_parser.h>

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
	const char path_prefix[] = "soc/amd/psp_fw/%s%s";
	const char raw_file_suffix[] = ".bin";
	struct psp_test_state *s = test_malloc(sizeof(*s));
	memset(s, 0, sizeof(*s));

	if (!s)
		return 1;

	s->num_entries = init_state->num_entries;
	s->type = init_state->type;
	s->level = init_state->level;
	const size_t raw_filename_size =
		strlen(path_prefix) + strlen(init_state->fname_base) + ARRAY_SIZE(raw_file_suffix);
	s->raw_filename = test_malloc(raw_filename_size);

	if (!s->raw_filename) {
		print_error("File path allocation error\n");
		ret = 2;
		goto error;
	}

	snprintf(s->raw_filename, raw_filename_size, path_prefix, init_state->fname_base, raw_file_suffix);

	int file_size = test_get_file_size(s->raw_filename);

	if (file_size <= 0) {
		print_error("Unable to open file: %s\n", s->raw_filename);
		ret = 3;
		goto error;
	}

	s->raw_file_sz = file_size;
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

#define PSP_CORRECT_FILE_TEST(_file_prefix, _num_entries, _type, _level)                     \
	{                                                                                    \
		.name = "test_psp_header(" _file_prefix ")",                                 \
		.test_func = test_psp_header, .setup_func = setup_psp_file,                  \
		.teardown_func = teardown_psp_file, .initial_state =                         \
		 &((struct psp_init_test_state) {_file_prefix, _num_entries, _type, _level}) \
	}

int main(void)
{
	const struct CMUnitTest tests[] = {
		PSP_CORRECT_FILE_TEST("psp_l1", 2, PSP_DIR_PSP, 1),
		PSP_CORRECT_FILE_TEST("psp_l2", 0x36, PSP_DIR_PSP, 2),
		PSP_CORRECT_FILE_TEST("bdh_l1", 0x11, PSP_DIR_BIOS, 1),
		PSP_CORRECT_FILE_TEST("bdh_l2", 0x11, PSP_DIR_BIOS, 2),
		PSP_CORRECT_FILE_TEST("ish", 1, PSP_DIR_ISH, 0),
		PSP_CORRECT_FILE_TEST("combo_1", 1, PSP_DIR_COMBO, 1),
		PSP_CORRECT_FILE_TEST("combo_2", 1, PSP_DIR_COMBO, 1),
		PSP_CORRECT_FILE_TEST("efs1", 2, PSP_DIR_EFS, 0),
		PSP_CORRECT_FILE_TEST("efs2", 2, PSP_DIR_EFS, 0),
		PSP_CORRECT_FILE_TEST("efs3", 2, PSP_DIR_EFS, 0),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
