/* SPDX-License-Identifier: GPL-2.0-only */

#include <fcntl.h>
#include <helpers/file.h>
#include <lib.h>
#include <lib/lzmadecode.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <tests/test.h>
#include <unistd.h>

struct lzma_test_state {
	char *raw_filename;
	size_t raw_file_sz;
	char *comp_filename;
	size_t comp_file_sz;
};

static int teardown_ulzman_file(void **state)
{
	struct lzma_test_state *s = *state;

	test_free(s->raw_filename);
	test_free(s->comp_filename);
	test_free(s);

	return 0;
}

/* Set data file with prestate */
static int setup_ulzman_file(void **state)
{
	int ret = 0;
	const char *fname_base = *state;
	const char path_prefix[] = "lib/lzma-test/%s%s";
	const char raw_file_suffix[] = ".bin";
	const char comp_file_suffix[] = ".lzma.bin";
	struct lzma_test_state *s = test_malloc(sizeof(*s));
	memset(s, 0, sizeof(*s));

	if (!s)
		return 1;

	const size_t raw_filename_size =
		strlen(path_prefix) + strlen(fname_base) + ARRAY_SIZE(raw_file_suffix);
	s->raw_filename = test_malloc(raw_filename_size);

	const size_t comp_filename_size =
		strlen(path_prefix) + strlen(fname_base) + ARRAY_SIZE(comp_file_suffix);
	s->comp_filename = test_malloc(comp_filename_size);

	if (!s->raw_filename || !s->comp_filename) {
		print_error("File path allocation error\n");
		ret = 2;
		goto error;
	}

	snprintf(s->raw_filename, raw_filename_size, path_prefix, fname_base, raw_file_suffix);
	snprintf(s->comp_filename, comp_filename_size, path_prefix, fname_base,
		 comp_file_suffix);

	s->raw_file_sz = test_get_file_size(s->raw_filename);
	s->comp_file_sz = test_get_file_size(s->comp_filename);

	if (s->raw_file_sz == -1) {
		print_error("Unable to open file: %s\n", s->raw_filename);
		ret = 3;
		goto error;
	}

	if (s->comp_file_sz == -1) {
		print_error("Unable to open file: %s\n", s->comp_filename);
		ret = 3;
		goto error;
	}

	*state = s;
	return 0;
error:
	teardown_ulzman_file((void **)&s);
	return ret;
}

static void test_ulzman_correct_file(void **state)
{
	struct lzma_test_state *s = *state;
	uint8_t *raw_buf = test_malloc(s->raw_file_sz);
	uint8_t *decomp_buf = test_malloc(s->raw_file_sz);
	uint8_t *comp_buf = test_malloc(s->comp_file_sz);

	assert_non_null(raw_buf);
	assert_non_null(decomp_buf);
	assert_non_null(comp_buf);
	assert_int_equal(s->raw_file_sz,
			 test_read_file(s->raw_filename, raw_buf, s->raw_file_sz));
	assert_int_equal(s->comp_file_sz,
			 test_read_file(s->comp_filename, comp_buf, s->comp_file_sz));

	assert_int_equal(s->raw_file_sz,
			 ulzman(comp_buf, s->comp_file_sz, decomp_buf, s->raw_file_sz));
	assert_memory_equal(raw_buf, decomp_buf, s->raw_file_sz);

	test_free(raw_buf);
	test_free(decomp_buf);
	test_free(comp_buf);
}

static void test_ulzman_input_too_small(void **state)
{
	uint8_t in_buf[32] = {0};
	uint8_t out_buf[32];

	assert_int_equal(0, ulzman(in_buf, LZMA_PROPERTIES_SIZE, out_buf, sizeof(out_buf)));
}

static void test_ulzman_zero_buffer(void **state)
{
	uint8_t in_buf[LZMA_PROPERTIES_SIZE + 1 * KiB];
	uint8_t out_buf[2 * KiB];

	memset(in_buf, 0, sizeof(in_buf));
	memset(out_buf, 0, sizeof(out_buf));

	assert_int_equal(0, ulzman(in_buf, sizeof(in_buf), out_buf, sizeof(out_buf)));
}

#define ULZMAN_CORRECT_FILE_TEST(_file_prefix)                                                 \
	{                                                                                      \
		.name = "test_ulzman_correct_file(" _file_prefix ")",                          \
		.test_func = test_ulzman_correct_file, .setup_func = setup_ulzman_file,        \
		.teardown_func = teardown_ulzman_file, .initial_state = (_file_prefix)         \
	}

int main(void)
{
	const struct CMUnitTest tests[] = {
		/* "data.N" in macros below refers to files:
		   - __TEST_DATA_DIR__ /lib/lzma-test/data.N.bin
		   - __TEST_DATA_DIR__ /lib/lzma-test/data.N.bin.lzma
		   Files data.N.bin suffix are raw data, and data.N.lzma.bin are its
		   LZMA-compressed form. Both are required to exist.
		 */

		/* util/cbfs-compression-tool compressed by itself.
		   To test compression of executable files like payloads. */
		ULZMAN_CORRECT_FILE_TEST("data.1"),

		/* README.md compressed by util/cbfs-compression-tool. */
		ULZMAN_CORRECT_FILE_TEST("data.2"),

		/* tests/lib/imd-test.c compressed by util/cbfs-compression-tool
		   Structured text file. */
		ULZMAN_CORRECT_FILE_TEST("data.3"),

		/* libcmocka.so.0.7.0 compressed by util/cbfs-compression-tool
		   Another binary file, shared object. */
		ULZMAN_CORRECT_FILE_TEST("data.4"),

		cmocka_unit_test(test_ulzman_input_too_small),

		cmocka_unit_test(test_ulzman_zero_buffer),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
