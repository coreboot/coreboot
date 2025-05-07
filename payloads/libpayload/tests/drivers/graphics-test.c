/* SPDX-License-Identifier: GPL-2.0-only */

#include "../drivers/video/graphics.c"

#include <libpayload.h>
#include <tests/test.h>

static void test_lanczos(void **state)
{
	/* This makes sure the `x` in lanczos_weight() becomes very small. This
	   test is mostly here to prove we won't end up dividing by zero. */
	fpmath_t x = lanczos_weight(fpisub(1, fpfrac(1, 1 << 30)), S0 + 1);
	assert_true(fpequals(x, fp(1)));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_lanczos),
	};

	return lp_run_group_tests(tests, NULL, NULL);
}
