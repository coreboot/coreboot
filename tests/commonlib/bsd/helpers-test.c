/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <tests/test.h>

static void func(void)
{
	function_called();
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
		cmocka_unit_test(test_retry),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
