/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/rational.h>
#include <tests/test.h>

struct rational_test_param {
	unsigned long num, den;
	unsigned long max_num, max_den;
	unsigned long exp_num, exp_den;
};

static const struct rational_test_param test_params[] = {
	/* Exceeds bounds, semi-convergent term > half last term */
	{ 1230, 10,     100, 20,        100, 1},
	/* Exceeds bounds, semi-convergent term < half last term */
	{ 34567, 100,   120, 20,        120, 1},
	/* Closest to zero */
	{ 1, 30,        100, 10,        0, 1},
	/* Closest to smallest non-zero */
	{ 1, 19,        100, 10,        1, 10},
	/* Exact answer */
	{ 1155, 7735,   255, 255,       33, 221},
	/* Convergent */
	{ 27, 32,       16, 16,         11, 13},
	/* Convergent, semiconvergent term half convergent term */
	{ 67, 54,       17, 18,         5, 4},
	/* Semiconvergent, semiconvergent term half convergent term */
	{ 453, 182,     60, 60,         57, 23},
	/* Semiconvergent, numerator limit */
	{ 87, 32,       70, 32,         68, 25},
	/* Semiconvergent, demominator limit */
	{ 14533, 4626,  15000, 2400,    7433, 2366},
};

static void test_rational(void **state)
{
	int i;
	unsigned long num = 0, den = 0;

	for (i = 0; i < ARRAY_SIZE(test_params); i++) {
		rational_best_approximation(test_params[i].num, test_params[i].den,
					    test_params[i].max_num, test_params[i].max_den,
					    &num, &den);
		assert_int_equal(num, test_params[i].exp_num);
		assert_int_equal(den, test_params[i].exp_den);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_rational),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}

