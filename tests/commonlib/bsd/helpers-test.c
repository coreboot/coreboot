/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <tests/test.h>

static void func(void)
{
	function_called();
}

static void test_genmask(void **state)
{
	assert_int_equal(GENMASK(4, 4), 0x10);
	assert_int_equal(GENMASK(4, 3), 0x18);
	assert_int_equal(GENMASK(4, 0), 0x1f);
	/* Edge cases */
	assert_int_equal(GENMASK(0, 0), 1);
	assert_int_equal(GENMASK(31, 31), 0x80000000);
	assert_int_equal(GENMASK(31, 0), 0xffffffff);
	assert_int_equal(GENMASK(63, 63), 0x8000000000000000);
	assert_int_equal(GENMASK(63, 0), 0xffffffffffffffff);
}

static void test_retry(void **state)
{
	int count;

	/* 2-argument form */
	count = 0;
	assert_true(retry(3, ++count == 1));
	count = 0;
	assert_true(retry(3, ++count == 3));
	count = 0;
	assert_false(retry(3, ++count == 4));

	/* 3-argument form */
	expect_function_calls(func, 9);
	assert_null(retry(10, NULL, func()));

	assert_int_equal(retry(10, 999, func()), 999);

	count = 0;
	expect_function_calls(func, 3);
	assert_true(retry(10, ++count == 4, func()));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_genmask),
		cmocka_unit_test(test_retry),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
