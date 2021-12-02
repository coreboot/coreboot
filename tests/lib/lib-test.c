/* SPDX-License-Identifier: GPL-2.0-only */

#include <lib.h>
#include <tests/test.h>

void test_popcnt(void **state)
{
	assert_int_equal(popcnt(0x0), 0);
	assert_int_equal(popcnt(0x10), 1);
	assert_int_equal(popcnt(0x10010010), 3);
	assert_int_equal(popcnt(0xffffffff), 32);
}

void test_clz(void **state)
{
	assert_int_equal(clz(0x0), 32);
	assert_int_equal(clz(0xf), 28);
	assert_int_equal(clz(0x80000000), 0);
	assert_int_equal(clz(0xffffffff), 0);
}

void test_log2(void **state)
{
	assert_int_equal(log2(0x0), -1);
	assert_int_equal(log2(0x1), 0);
	assert_int_equal(log2(0x5), 2);
	assert_int_equal(log2(0x80000000), 31);
	assert_int_equal(log2(0xffffffff), 31);
}

void test_ffs(void **state)
{
	assert_int_equal(__ffs(0x0), -1);
	assert_int_equal(__ffs(0x1), 0);
	assert_int_equal(__ffs(0x1010), 4);
	assert_int_equal(__ffs(0x10000000), 28);
	assert_int_equal(__ffs(0xffffffff), 0);
}

void test_log2_ceil(void **state)
{
	assert_int_equal(log2_ceil(0x0), -1);
	assert_int_equal(log2_ceil(0x1), 0);
	assert_int_equal(log2_ceil(0x5), 3);
	assert_int_equal(log2_ceil(0x80000000), 31);
	assert_int_equal(log2_ceil(0xffffffff), 32);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_popcnt),
		cmocka_unit_test(test_clz),
		cmocka_unit_test(test_log2),
		cmocka_unit_test(test_ffs),
		cmocka_unit_test(test_log2_ceil),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
