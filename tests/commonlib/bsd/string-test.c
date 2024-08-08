/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/string.h>
#include <stddef.h>
#include <tests/test.h>

static void test_strlen(void **state)
{
	const char *str;

	str = "coreboot";
	assert_int_equal(__builtin_strlen(str), strlen(str));

	str = "is\0very";
	assert_int_equal(__builtin_strlen(str), strlen(str));

	str = "nice\n";
	assert_int_equal(__builtin_strlen(str), strlen(str));

	assert_int_equal(0, strlen(""));
}

static void test_strnlen(void **state)
{
	/* maxlen larger than string len */
	assert_int_equal(8, strnlen("coreboot", 100));

	/* maxlen equal to string len */
	assert_int_equal(8, strnlen("coreboot", 8));

	/* maxlen smaller than string len */
	assert_int_equal(5, strnlen("coreboot", 5));

	/* maxlen is 0 */
	assert_int_equal(0, strnlen("coreboot", 0));

	/* Empty string */
	assert_int_equal(0, strnlen("", 3));
}

static void test_skip_atoi(void **state)
{
	int i;
	char *ptr, *copy;
	const struct str_with_u_val_t {
		char *str;
		uint32_t value;
		uint32_t offset;
	} str_with_u_val[] = {
		{"42aa", 42, 2},
		{"a", 0, 0},
		{"0", 0, 1},
		{"4a2", 4, 1},
	};

	for (i = 0; i < ARRAY_SIZE(str_with_u_val); i++) {
		ptr = str_with_u_val[i].str;
		copy = ptr;
		assert_true(str_with_u_val[i].value == skip_atoi(&ptr));
		assert_int_equal(str_with_u_val[i].offset, ptr - copy);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_strlen),
		cmocka_unit_test(test_strnlen),
		cmocka_unit_test(test_skip_atoi),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
