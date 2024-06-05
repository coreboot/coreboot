/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/string.h>
#include <tests/test.h>

/* Used to test skip_atoi */
struct str_with_u_val_t {
	char *str;
	uint32_t value;
	uint32_t offset;
} str_with_u_val[] = {
	{"42aa", 42, 2},
	{"a", 0, 0},
	{"0", 0, 1},
	{"4a2", 4, 1},
};

static void test_skip_atoi(void **state)
{
	int i;
	char *ptr, *copy;

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
		cmocka_unit_test(test_skip_atoi),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
