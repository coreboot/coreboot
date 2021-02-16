/* SPDX-License-Identifier: GPL-2.0-only */

/* Include memcmp() source code and alter its name to compare results with libc memcmp() */
#define memcmp cb_memcmp

#include "../lib/memcmp.c"

#undef cb_memcmp

#include <tests/test.h>
#include <string.h>

const char test_data1[] = "TEST_DATA @4321 !@#$%^&*\\/";
const size_t test_data1_sz = sizeof(test_data1);

const char test_data2[] = "TEST_DATA @8765 !@#$%^&*\\/";
const char test_data2_sz = sizeof(test_data2);

static void test_data_correctness(void **state)
{
	assert_int_equal(sizeof(test_data1), test_data1_sz);
	assert_int_equal(sizeof(test_data2), test_data2_sz);
	assert_int_equal(test_data1_sz, test_data2_sz);
}

static void test_memcmp_equal(void **state)
{
	const int res_cb = cb_memcmp(test_data1, test_data1, test_data1_sz);
	const int res_std = memcmp(test_data1, test_data1, test_data1_sz);

	assert_int_equal(0, res_cb);
	assert_int_equal(res_cb, res_std);
}

static void test_memcmp_first_not_matching_lower(void **state)
{
	const int res_cb = cb_memcmp(test_data1, test_data2, test_data1_sz);
	const int res_std = memcmp(test_data1, test_data2, test_data1_sz);

	assert_true(res_cb < 0);
	assert_int_equal(res_cb, res_std);
}

static void test_memcmp_first_not_matching_higher(void **state)
{
	const int res_cb = cb_memcmp(test_data2, test_data1, test_data1_sz);
	const int res_std = memcmp(test_data2, test_data1, test_data1_sz);

	assert_true(res_cb > 0);
	assert_int_equal(res_cb, res_std);
}

static void test_memcmp_zero_size(void **state)
{
	const int res_cb = cb_memcmp(test_data1, test_data2, 0);
	const int res_std = memcmp(test_data1, test_data2, 0);

	assert_int_equal(0, res_cb);
	assert_int_equal(res_cb, res_std);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_data_correctness),
		cmocka_unit_test(test_memcmp_equal),
		cmocka_unit_test(test_memcmp_first_not_matching_lower),
		cmocka_unit_test(test_memcmp_first_not_matching_higher),
		cmocka_unit_test(test_memcmp_zero_size),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
