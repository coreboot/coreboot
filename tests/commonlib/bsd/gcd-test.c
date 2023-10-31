/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/gcd.h>
#include <tests/test.h>

static void test_gcd32(void **state)
{
	assert_int_equal(gcd32(17, 11), 1);
	assert_int_equal(gcd32(64, 36), 4);
	assert_int_equal(gcd32(90, 123), 3);
	assert_int_equal(gcd32(65536, 339584), 128);
	assert_int_equal(gcd32(1, 1), 1);
	assert_int_equal(gcd32(1, 123), 1);
	assert_int_equal(gcd32(123, 1), 1);
	assert_int_equal(gcd32(1, UINT32_MAX), 1);
	assert_int_equal(gcd32(UINT32_MAX, 1), 1);
	assert_int_equal(gcd32(UINT32_MAX, UINT32_MAX), UINT32_MAX);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_gcd32),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
