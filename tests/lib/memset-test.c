/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <tests/test.h>
#include <commonlib/helpers.h>
#include <types.h>
#include <string.h>

#define MEMSET_BUFFER_SZ (4 * KiB)

struct memset_test_state {
	u8 *base_buffer;
	u8 *helper_buffer;
};

static int setup_test(void **state)
{
	struct memset_test_state *s = malloc(sizeof(struct memset_test_state));

	u8 *buf = malloc(MEMSET_BUFFER_SZ);
	u8 *helper_buf = malloc(MEMSET_BUFFER_SZ);
	if (!buf || !helper_buf)
		goto error;

	s->base_buffer = buf;
	s->helper_buffer = helper_buf;
	*state = s;

	return 0;

error:
	free(buf);
	free(helper_buf);
	return -1;
}

static int teardown_test(void **state)
{
	struct memset_test_state *s = *state;

	if (s) {
		free(s->base_buffer);
		free(s->helper_buffer);
	}

	free(s);

	return 0;
}

static void test_memset_full_range(void **state)
{
	struct memset_test_state *s = *state;

	for (int i = 0; i < MEMSET_BUFFER_SZ; ++i)
		s->helper_buffer[i] = 0x42;

	memset(s->base_buffer, 0x42, MEMSET_BUFFER_SZ);
	assert_memory_equal(s->base_buffer, s->helper_buffer, MEMSET_BUFFER_SZ);
}

static void test_memset_subrange(void **state)
{
	struct memset_test_state *s = *state;
	int i = 0;

	for (; i < MEMSET_BUFFER_SZ / 2; ++i)
		s->helper_buffer[i] = 0xEF;
	for (; i < MEMSET_BUFFER_SZ; ++i)
		s->helper_buffer[i] = 0xCD;

	/* Expect correct buffer subranges to be filled */
	memset(s->base_buffer, 0xEF, MEMSET_BUFFER_SZ / 2);
	memset(s->base_buffer + MEMSET_BUFFER_SZ / 2, 0xCD, MEMSET_BUFFER_SZ / 2);
	assert_memory_equal(s->base_buffer, s->helper_buffer, MEMSET_BUFFER_SZ);
}

static void test_memset_zero_size(void **state)
{
	struct memset_test_state *s = *state;

	for (int i = 0; i < MEMSET_BUFFER_SZ; ++i) {
		s->base_buffer[i] = 0xFF;
		s->helper_buffer[i] = 0xFF;
	}

	/* Expect no change in buffer after calling memset with zero size */
	memset(s->base_buffer, 0xAA, (0));
	assert_memory_equal(s->base_buffer, s->helper_buffer, MEMSET_BUFFER_SZ);
}

static void test_memset_one_byte(void **state)
{
	struct memset_test_state *s = *state;

	for (int i = 0; i < MEMSET_BUFFER_SZ; ++i) {
		s->base_buffer[i] = 0x66;
		s->helper_buffer[i] = 0x66;
	}

	s->helper_buffer[MEMSET_BUFFER_SZ / 4] = 0xCC;
	memset(&s->base_buffer[MEMSET_BUFFER_SZ / 4], 0xCC, 1);
	assert_memory_equal(s->base_buffer, s->helper_buffer, MEMSET_BUFFER_SZ);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_memset_full_range,
				setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memset_subrange,
				setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memset_zero_size,
				setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_memset_one_byte,
				setup_test, teardown_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
