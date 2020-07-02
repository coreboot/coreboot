/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <string.h>
#include <lib.h>
#include <stdint.h>
#include <tests/test.h>

struct hexstr_t {
	char *str;
	int *val;
	size_t res;
} hexstr[] = {
	{.str = "A", .res = 0},
	{.str = "AB", .val = (int[]) {171}, .res = 1},
	{.str = "277a", .val = (int[]) {39, 122}, .res = 2},
	{.str = "277ab", .val = (int[]) {39, 122}, .res = 2},
	{.str = "\n\rx1234567ijkl", .val = (int[]) {18, 52, 86}, .res = 3},
	{.str = "\nB*e/ef-", .val = (int[]) {190, 239}, .res = 2},
};

static void test_hexstrtobin(void **state)
{
	uint8_t *buf;
	size_t res, len;

	for (int i = 0; i < ARRAY_SIZE(hexstr); i++) {
		len = strlen(hexstr[i].str) / 2 + 1;
		buf = malloc(len);
		res = hexstrtobin(hexstr[i].str, buf, len);

		assert_int_equal(hexstr[i].res, res);

		for (int j = 0; j < res; j++)
			assert_int_equal(hexstr[i].val[j], buf[j]);

		free(buf);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_hexstrtobin),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
