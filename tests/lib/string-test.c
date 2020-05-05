/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <tests/test.h>

/*
 * Important note: In every particular test, don't use any string-related
 * functions other than function under test. We are linking against
 * src/lib/string.c not the standard library. This is important for proper test
 * isolation. One can use __builtin_xxx for many of the most simple str*()
 * functions, when non-coreboot one is required.
 */

struct strings_t {
	char *str;
	size_t size;
} strings[] = {
	{"coreboot",	8},
	{"is\0very",	2}, /* strlen should be 2 because of the embedded \0 */
	{"nice\n",	5}
};

static void test_strlen_strings(void **state)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(strings); i++)
		assert_int_equal(strings[i].size, strlen(strings[i].str));
}

static void test_strdup(void **state)
{
	char str[] = "Hello coreboot\n";
	char *duplicate;

	duplicate = strdup(str);

	/* There is a more suitable Cmocka's function 'assert_string_equal()', but it
	   is using strcmp() internally. */
	assert_int_equal(0, memcmp(str, duplicate, __builtin_strlen(str)));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_strlen_strings),
		cmocka_unit_test(test_strdup),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
