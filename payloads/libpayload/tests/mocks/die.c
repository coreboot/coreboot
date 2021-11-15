/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <stdbool.h>

void die_work(const char *file, const char *func, int line, const char *fmt, ...)
{
	/* Failing asserts are jumping to the user code (test) if expect_assert_failed() was
	   previously called. Otherwise it jumps to the cmocka code and fails the test. */
	mock_assert(false, "Mock assetion called", file, line);

	/* Should never be reached */
	print_error("%s() called...\n", __func__);
	while (1)
		;
}
