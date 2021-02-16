/* SPDX-License-Identifier: GPL-2.0-only */

/* Include memcpy() source code and alter its name to compare results with libc memcpy() */
#define memcpy cb_memcpy
#include "../lib/memcpy.c"
#undef memcpy

#include <stdlib.h>
#include <tests/test.h>
#include <commonlib/helpers.h>
#include <types.h>

#define MEMCPY_BUFFER_SZ (4 * KiB)

/* Prototype of memcpy() from string.h was changed to cb_memcpy().
   It has to be defined again. */
void *memcpy(void *dest, const void *src, size_t n);

struct test_memcpy_data {
	u8 *buffer_from;
	u8 *buffer_to;
	u8 *helper_buffer;
};

int setup_test(void **state)
{
	struct test_memcpy_data *s = malloc(sizeof(struct test_memcpy_data));

	if (!s)
		return -1;

	s->buffer_from = malloc(MEMCPY_BUFFER_SZ);
	s->buffer_to = malloc(MEMCPY_BUFFER_SZ);
	s->helper_buffer = malloc(MEMCPY_BUFFER_SZ);

	if (!s->buffer_from || !s->buffer_to || !s->helper_buffer) {
		free(s->buffer_from);
		free(s->buffer_to);
		free(s->helper_buffer);
		free(s);
		return -1;
	}

	/* Fill buffers with different values (other than zero) to make them distinguishable.
	   The helper buffer is often used as a backup of destination buffer so it has the
	   same value. */
	memset(s->buffer_from, 0xAB, MEMCPY_BUFFER_SZ);
	memset(s->buffer_to, 0xBC, MEMCPY_BUFFER_SZ);
	memset(s->helper_buffer, 0xBC, MEMCPY_BUFFER_SZ);

	*state = s;

	return 0;
}

int teardown_test(void **state)
{
	struct test_memcpy_data *s = *state;

	free(s->buffer_from);
	free(s->buffer_to);
	free(s->helper_buffer);
	free(s);

	return 0;
}

/* Fill buffer with values from provided range [start; end] in circular way. */
static void fill_buffer_data_range(u8 *buffer, size_t sz, u8 start_value, u8 end_value)
{
	for (size_t i = 0; i < sz; ++i)
		buffer[i] = (start_value + i) % (end_value - start_value + 1);
}

static void test_memcpy_full_buffer_copy(void **state)
{
	struct test_memcpy_data *s = *state;
	void *res_cb;

	fill_buffer_data_range(s->buffer_from, MEMCPY_BUFFER_SZ, 0, 255);

	res_cb = cb_memcpy(s->buffer_to, s->buffer_from, MEMCPY_BUFFER_SZ);

	assert_ptr_equal(s->buffer_to, res_cb);
	assert_memory_equal(s->buffer_from, s->buffer_to, MEMCPY_BUFFER_SZ);
}

static void test_memcpy_zero_size(void **state)
{
	struct test_memcpy_data *s = *state;
	void *res_cb;

	fill_buffer_data_range(s->buffer_from, MEMCPY_BUFFER_SZ, 'A', 'Z');

	/* Expect no change in destination buffer. */
	res_cb = cb_memcpy(s->buffer_to, s->buffer_from, 0);
	assert_ptr_equal(s->buffer_to, res_cb);
	assert_memory_equal(s->buffer_to, s->helper_buffer, MEMCPY_BUFFER_SZ);
}

static void test_memcpy_buffer_part(void **state)
{
	struct test_memcpy_data *s = *state;
	void *res_cb;
	const size_t offset = MEMCPY_BUFFER_SZ / 4;
	const size_t sz = MEMCPY_BUFFER_SZ / 2;

	/* Self-test for correct data ranges */
	assert_true(offset + sz <= MEMCPY_BUFFER_SZ);

	fill_buffer_data_range(s->buffer_from, MEMCPY_BUFFER_SZ, '0', '9');

	/* Expect only *sz* bytes of buffer to be copied. Helper buffer is used as template. */
	res_cb = cb_memcpy(s->buffer_to + offset, s->buffer_from, sz);
	assert_ptr_equal(s->buffer_to + offset, res_cb);
	assert_memory_equal(s->buffer_to, s->helper_buffer, offset);
	assert_memory_equal(s->buffer_to + offset, s->buffer_from, sz);
	assert_memory_equal(s->buffer_to + offset + sz, s->helper_buffer + offset + sz,
				MEMCPY_BUFFER_SZ - (offset + sz));
}

static void test_memcpy_buffer_part_unaligned(void **state)
{
	struct test_memcpy_data *s = *state;
	void *res_cb;
	const size_t dst_offset = MEMCPY_BUFFER_SZ / 8 + 3;
	const size_t src_offset = MEMCPY_BUFFER_SZ / 4 - 3;
	const size_t sz = MEMCPY_BUFFER_SZ / 4 + 7;

	/* Self-test for correct data ranges */
	assert_true(dst_offset + sz <= MEMCPY_BUFFER_SZ);
	assert_true(src_offset + sz <= MEMCPY_BUFFER_SZ);

	fill_buffer_data_range(s->buffer_from, MEMCPY_BUFFER_SZ, 0x13, 0xB7);

	res_cb = cb_memcpy(s->buffer_to + dst_offset, s->buffer_from + src_offset, sz);
	assert_ptr_equal(s->buffer_to + dst_offset, res_cb);
	assert_memory_equal(s->buffer_to, s->helper_buffer, dst_offset);
	assert_memory_equal(s->buffer_to + dst_offset, s->buffer_from + src_offset, sz);
	assert_memory_equal(s->buffer_to + dst_offset + sz, s->helper_buffer + dst_offset + sz,
				MEMCPY_BUFFER_SZ - (dst_offset + sz));
}

static void test_memcpy_copy_to_itself(void **state)
{
	struct test_memcpy_data *s = *state;
	void *res_cb;

	fill_buffer_data_range(s->buffer_to, MEMCPY_BUFFER_SZ, 'G', 'X');
	memcpy(s->buffer_to, s->helper_buffer, MEMCPY_BUFFER_SZ);

	/* Expect no change in source/destination buffer. */
	res_cb = cb_memcpy(s->buffer_to, s->buffer_to, MEMCPY_BUFFER_SZ);
	assert_ptr_equal(s->buffer_to, res_cb);
	assert_memory_equal(s->buffer_to, s->helper_buffer, MEMCPY_BUFFER_SZ);
}

static void test_memcpy_copy_part_of_itself_to_itself(void **state)
{
	struct test_memcpy_data *s = *state;
	void *res_cb;
	const size_t offset = MEMCPY_BUFFER_SZ / 8;
	const size_t sz = MEMCPY_BUFFER_SZ - offset;

	/* Self-test for correct data ranges */
	assert_true(offset + sz <= MEMCPY_BUFFER_SZ);

	fill_buffer_data_range(s->buffer_to, MEMCPY_BUFFER_SZ, 'd', 'v');
	memcpy(s->helper_buffer, s->buffer_to, MEMCPY_BUFFER_SZ);

	/* Expect only *sz* bytes to be overwritten and *offset* bytes to be left at the end. */
	res_cb = cb_memcpy(s->buffer_to, s->buffer_to + offset, sz);
	assert_ptr_equal(s->buffer_to, res_cb);
	assert_memory_equal(s->buffer_to, s->helper_buffer + offset, sz);
	assert_memory_equal(s->buffer_to + sz, s->helper_buffer + sz, offset);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_memcpy_full_buffer_copy,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memcpy_zero_size,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memcpy_buffer_part,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memcpy_buffer_part_unaligned,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memcpy_copy_to_itself,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memcpy_copy_part_of_itself_to_itself,
						setup_test, teardown_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
