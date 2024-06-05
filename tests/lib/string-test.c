/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <tests/test.h>

/*
 * Important note: In every particular test, don't use any string-related
 * functions other than function under test. We are linking against
 * src/lib/string.c not the standard library. This is important for proper test
 * isolation.
 */

struct string_pairs_t {
	char *dst;
	char *src;
} string_pairs[] = {
	{"Hello ", "world!"},
	{"He\0llo ", "world"},
	{"", "world!"},
	{"", ""},
};

const char *strings[] = {
	"coreboot",
	"is\0very",
	"nice\n"
};

/* Used to test atol */
struct str_with_l_val_t {
	char *str;
	long value;
} str_with_l_val[] = {
	{"42", 42},
	{"four42", 0},
	{"42five5",	42},
	{"4\02", 4},
	{"+42", 42},
	{"-42", -42},
	{"\t\n\r\f\v-42", -42},
};

static void test_strdup(void **state)
{
	char str[] = "Hello coreboot\n";
	char *duplicate = strdup(str);

	/* There is a more suitable Cmocka's function 'assert_string_equal()', but it
	   is using strcmp() internally. */
	assert_int_equal(0, memcmp(str, duplicate, __builtin_strlen(str)));

	free(duplicate);
}

static void test_strconcat(void **state)
{
	int i;
	size_t str_len, str2_len, res_len;
	char *result;

	for (i = 0; i < ARRAY_SIZE(string_pairs); i++) {
		str_len = __builtin_strlen(string_pairs[i].dst);
		str2_len = __builtin_strlen(string_pairs[i].src);

		result = strconcat(string_pairs[i].dst, string_pairs[i].src);
		res_len = __builtin_strlen(result);

		assert_int_equal(res_len, str_len + str2_len);
		assert_int_equal(0, memcmp(string_pairs[i].dst, result, str_len));
		assert_int_equal(0, memcmp(string_pairs[i].src, result + str_len, str2_len));

		free(result);
	}
}

static void test_strnlen(void **state)
{
	int i, n = 5;
	size_t str_len, limited_len;

	for (i = 0; i < ARRAY_SIZE(strings); i++) {
		str_len = __builtin_strlen(strings[i]);
		limited_len = MIN(n, str_len);
		assert_int_equal(limited_len, strnlen(strings[i], n));
	}
}

static void test_strlen(void **state)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(strings); i++)
		assert_int_equal(__builtin_strlen(strings[i]), strlen(strings[i]));
}

static void test_strchr(void **state)
{
	char str[] = "Abracadabra!\n";

	assert_ptr_equal(str, strchr(str, 'A'));
	assert_ptr_equal(str + 3, strchr(str, 'a'));
	assert_ptr_equal(str + 12, strchr(str, '\n'));

	assert_null(strchr(str, 'z'));
}


static void test_strrchr(void **state)
{
	char str[] = "Abracadabra!\n";

	assert_ptr_equal(str, strrchr(str, 'A'));
	assert_ptr_equal(str + 9, strrchr(str, 'r'));
	assert_ptr_equal(str + 12, strrchr(str, '\n'));

	assert_null(strrchr(str, 'z'));
}

static void test_strncpy(void **state)
{
	int i;
	int n1 = 2, n2 = 8;
	char src[] = "Hello";
	char dst[sizeof(src) + 5];
	size_t src_len = __builtin_strlen(src);
	size_t dst_len = sizeof(dst);

	/* n1 case  */

	/* Needed for ensuring that characters behind the limit
	   are not overwritten */
	memset(dst, 'x', dst_len);

	strncpy(dst, src, n1);

	assert_int_equal(0, memcmp(dst, src, n1));

	for (i = n1; i < dst_len; i++)
		assert_true(dst[i] == 'x');

	/* n2 case: */

	memset(dst, 'x', dst_len);

	strncpy(dst, src, n2);

	assert_int_equal(0, memcmp(dst, src, src_len));

	for (i = src_len; i < n2; i++)
		assert_true(dst[i] == '\0');

	for (i = n2; i < dst_len; i++)
		assert_true(dst[i] == 'x');
}

static void test_strcpy(void **state)
{
	char src[] = "Hello coreboot\n";
	char dst[sizeof(src)];

	/* Make sure that strcpy() sets '\0' by initializing a whole
	   dst array to fixed, non-'\0' value */
	memset(dst, 'x', sizeof(dst));

	strcpy(dst, src);

	assert_int_equal(0, memcmp(dst, src, __builtin_strlen(src) + 1));
}

static void test_strcmp(void **state)
{
	char str[] = "Banana";
	char str2[] = "Banana";
	char str3[] = "Bananas";

	assert_true(strcmp(str, str3) < 0);
	assert_int_equal(0, strcmp(str, str2));
	assert_true(strcmp(str3, str2) > 0);
}

static void test_strncmp(void **state)
{
	char str[] = "Banana";
	char str2[] = "Bananas";

	size_t str2_len = __builtin_strlen(str2);

	assert_true(strncmp(str, str2, str2_len) < 0);
	assert_int_equal(0, strncmp(str, str2, str2_len - 1));
}

static void test_strspn(void **state)
{
	char str[] = "4213401234";
	char str2[] = "01234";
	char str3[] = "1234";

	assert_int_equal(5, strspn(str, str3));
	assert_int_equal(0, strspn(str2, str3));
}

static void test_strcspn(void **state)
{
	char str[] = "12340000";
	char str2[] = "00001234";
	char str3[] = "1234";

	assert_int_equal(0, strcspn(str, str3));
	assert_int_equal(4, strcspn(str2, str3));
}

/* Please bear in mind that `atol()` uses `strspn()` internally, so the result
   of `test_atol` is dependent on the result of `test_strspn`. */
static void test_atol(void **state)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(str_with_l_val); i++)
		assert_int_equal(str_with_l_val[i].value, atol(str_with_l_val[i].str));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_strdup),
		cmocka_unit_test(test_strconcat),
		cmocka_unit_test(test_strnlen),
		cmocka_unit_test(test_strlen),
		cmocka_unit_test(test_strchr),
		cmocka_unit_test(test_strrchr),
		cmocka_unit_test(test_strncpy),
		cmocka_unit_test(test_strcpy),
		cmocka_unit_test(test_strcmp),
		cmocka_unit_test(test_strncmp),
		cmocka_unit_test(test_strspn),
		cmocka_unit_test(test_strcspn),
		cmocka_unit_test(test_atol),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
