/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/gcd.h>
#include <tests/test.h>

static void test_gcd(void **state)
{
	assert_int_equal(gcd(17, 11), 1);
	assert_int_equal(gcd(64, 36), 4);
	assert_int_equal(gcd(90, 123), 3);
	assert_int_equal(gcd(65536, 339584), 128);
	assert_int_equal(gcd(1, 1), 1);
	assert_int_equal(gcd(1, 123), 1);
	assert_int_equal(gcd(123, 1), 1);
	assert_int_equal(gcd(1, UINT32_MAX), 1);
	assert_int_equal(gcd(UINT32_MAX, 1), 1);
	assert_int_equal(gcd(UINT32_MAX, UINT32_MAX), UINT32_MAX);
	assert_int_equal(gcd(1, UINT64_MAX), 1);
	assert_int_equal(gcd(UINT64_MAX, 1), 1);
	assert_int_equal(gcd(UINT64_MAX, UINT64_MAX), UINT64_MAX);
	assert_int_equal(gcd((uint64_t)UINT32_MAX + 1, UINT64_MAX / 2 + 1),
			 (uint64_t)UINT32_MAX + 1);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_gcd),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
