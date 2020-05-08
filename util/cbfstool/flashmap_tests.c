/* fmap_from_fmd.c, simple launcher for fmap library unit test suite */
/* SPDX-License-Identifier: GPL-2.0-only */

#include "flashmap/fmap.h"

#include <stdio.h>

int main(void)
{
	int result = fmap_test();

	puts("");
	puts("===");
	puts("");
	if (!result)
		puts("RESULT: All unit tests PASSED.");
	else
		puts("RESULT: One or more tests FAILED!");

	return result;
}
