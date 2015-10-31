/*
 * fmap_from_fmd.c, simple launcher for fmap library unit test suite
 *
 * Copyright (C) 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
