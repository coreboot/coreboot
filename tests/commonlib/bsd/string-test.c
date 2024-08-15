/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/string.h>
#include <stddef.h>
#include <string.h>
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
	/* maxlen is SIZE_MAX */
	assert_int_equal(8, strnlen("coreboot", SIZE_MAX));

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

static void test_strcat(void **state)
{
	static const char str[] = "Hello ";
	size_t len = __builtin_strlen(str);
	static const char src[] = "World";
	static const char expected[] = "Hello World";
	size_t expected_len = __builtin_strlen(expected);
	char dst[100];
	char *ret;

	/* Empty src & dst */
	dst[0] = '\0';
	memset(dst + 1, 0xee, sizeof(dst) - 1);
	ret = strcat(dst, "");
	assert_ptr_equal(dst, ret);
	assert_int_equal('\0', dst[0]);

	/* Empty src */
	memcpy(dst, str, len + 1);
	memset(dst + len + 1, 0xee, sizeof(dst) - len - 1);
	ret = strcat(dst, "");
	assert_memory_equal(str, dst, len + 1);

	/* Empty dst */
	memset(dst, 0x0, sizeof(dst));
	memset(dst + 1, 0xee, sizeof(dst) - 1);
	ret = strcat(dst, src);
	assert_ptr_equal(dst, ret);
	assert_memory_equal(src, dst, __builtin_strlen(src) + 1);

	/* Non-empty str & dst */
	memcpy(dst, str, len + 1);
	memset(dst + len + 1, 0xee, sizeof(dst) - len - 1);
	ret = strcat(dst, src);
	assert_ptr_equal(dst, ret);
	assert_memory_equal(expected, dst, expected_len + 1);
}

static void test_strncat(void **state)
{
	static const char str[] = "Hello ";
	size_t len = __builtin_strlen(str);
	static const char src[] = "World";
	size_t src_len = __builtin_strlen(src);
	static const char expected[] = "Hello World";
	size_t expected_len = __builtin_strlen(expected);
	char dst[100];
	char *ret;

	/* n larger than src len */
	memcpy(dst, str, len + 1);
	memset(dst + len + 1, 0xee, sizeof(dst) - len - 1);
	ret = strncat(dst, src, src_len + 5);
	assert_ptr_equal(dst, ret);
	assert_memory_equal(expected, dst, expected_len + 1);

	/* n smaller than src len */
	memcpy(dst, str, len + 1);
	memset(dst + len + 1, 0xee, sizeof(dst) - len - 1);
	ret = strncat(dst, src, src_len - 2);
	assert_ptr_equal(dst, ret);
	assert_memory_equal("Hello Wor", dst, expected_len - 2 + 1);

	/* n is 0 */
	memcpy(dst, str, len + 1);
	memset(dst + len + 1, 0xee, sizeof(dst) - len - 1);
	ret = strncat(dst, src, 0);
	assert_ptr_equal(dst, ret);
	assert_memory_equal(str, dst, len + 1);
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
		cmocka_unit_test(test_strcat),
		cmocka_unit_test(test_strncat),
		cmocka_unit_test(test_skip_atoi),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
