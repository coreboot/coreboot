/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>
#include <tests/test.h>

static void test_strsep(void **state)
{
	/* Use `char x[] = ` instead of `char *x = ` to avoid writing to .rodata. */
	char test1[] = "abc,def|ghi jklm,\no";
	const char *delim = ", \n";

	char *stringp = test1;
	assert_string_equal(strsep(&stringp, delim), "abc");
	assert_ptr_equal(stringp, test1 + 4);
	assert_string_equal(strsep(&stringp, delim), "def|ghi");
	assert_ptr_equal(stringp, test1 + 12);
	assert_string_equal(strsep(&stringp, delim), "jklm");
	assert_ptr_equal(stringp, test1 + 17);
	assert_string_equal(strsep(&stringp, delim), "");
	assert_ptr_equal(stringp, test1 + 18);
	assert_string_equal(strsep(&stringp, delim), "o");
	assert_null(stringp);
	assert_null(strsep(&stringp, delim));
	assert_null(stringp);

	char test2[] = "";
	stringp = test2;
	assert_string_equal(strsep(&stringp, delim), "");
	assert_null(strsep(&stringp, delim));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_strsep),
	};

	return lp_run_group_tests(tests, NULL, NULL);
}
