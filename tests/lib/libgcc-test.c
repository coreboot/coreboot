/* SPDX-License-Identifier: GPL-2.0-only */

#include "../lib/libgcc.c"

#include <tests/test.h>
#include <stdlib.h>
#include <string.h>

struct {
	u32 value;
	int expected_output;
} test_data[] = {
	/* GCC documentation says, __clzsi2() has undefined result for zero as an input value,
	   but coreboot implementation can handle this. */
	{.value = 0, .expected_output = 32},

	{.value = 1, .expected_output = 31},
	{.value = 2, .expected_output = 30},
	{.value = 3, .expected_output = 30},
	{.value = 4, .expected_output = 29},
	{.value = 5, .expected_output = 29},
	{.value = 6, .expected_output = 29},
	{.value = 7, .expected_output = 29},

	{.value = 0xF, .expected_output = 28},
	{.value = 0x10, .expected_output = 27},
	{.value = 0x25, .expected_output = 26},
	{.value = 0x5D, .expected_output = 25},
	{.value = 0xB7, .expected_output = 24},

	{.value = 0x133, .expected_output = 23},
	{.value = 0x3DC, .expected_output = 22},
	{.value = 0x6F1, .expected_output = 21},
	{.value = 0x897, .expected_output = 20},

	{.value = 0x1FFF, .expected_output = 19},
	{.value = 0x2222, .expected_output = 18},
	{.value = 0x7BAD, .expected_output = 17},
	{.value = 0xE708, .expected_output = 16},

	{.value = 0x1DABD, .expected_output = 15},
	{.value = 0x29876, .expected_output = 14},
	{.value = 0x56665, .expected_output = 13},
	{.value = 0xABCDE, .expected_output = 12},

	{.value = 0x18365F, .expected_output = 11},
	{.value = 0x3D0115, .expected_output = 10},
	{.value = 0x4B07EB, .expected_output = 9},
	{.value = 0xCCC74D, .expected_output = 8},

	{.value = 0x17933ED, .expected_output = 7},
	{.value = 0x2B00071, .expected_output = 6},
	{.value = 0x4D4C1A5, .expected_output = 5},
	{.value = 0xAD01FFF, .expected_output = 4},

	{.value = 0x1C5A8057, .expected_output = 3},
	{.value = 0x35AB23C3, .expected_output = 2},
	{.value = 0x7017013B, .expected_output = 1},
	{.value = 0xAD01EB15, .expected_output = 0},

	{.value = 0xFFFFFFFF, .expected_output = 0},
	{.value = 0x80000000, .expected_output = 0},
	{.value = 0x7FFFFFFF, .expected_output = 1},
	{.value = 0x30000000, .expected_output = 2},
	{.value = 0x10000000, .expected_output = 3},
	{.value = 0x0FFFFFFF, .expected_output = 4},

	{.value = 0xFF000000, .expected_output = 0},
	{.value = 0x00FF0000, .expected_output = 8},
	{.value = 0x0000FF00, .expected_output = 16},
	{.value = 0x000000FF, .expected_output = 24},

	{.value = 0x8F000000, .expected_output = 0},
	{.value = 0x008F0000, .expected_output = 8},
	{.value = 0x00008F00, .expected_output = 16},
	{.value = 0x0000008F, .expected_output = 24},

	{.value = 0x7F000000, .expected_output = 1},
	{.value = 0x007F0000, .expected_output = 9},
	{.value = 0x00007F00, .expected_output = 17},
	{.value = 0x0000007F, .expected_output = 25},

	{.value = 0x3F000000, .expected_output = 2},
	{.value = 0x003F0000, .expected_output = 10},
	{.value = 0x00003F00, .expected_output = 18},
	{.value = 0x0000003F, .expected_output = 26},

	{.value = 0x1F000000, .expected_output = 3},
	{.value = 0x001F0000, .expected_output = 11},
	{.value = 0x00001F00, .expected_output = 19},
	{.value = 0x0000001F, .expected_output = 27},

	{.value = 0x0F000000, .expected_output = 4},
	{.value = 0x000F0000, .expected_output = 12},
	{.value = 0x00000F00, .expected_output = 20},
	{.value = 0x0000000F, .expected_output = 28},

	{.value = 0x08000000, .expected_output = 4},
	{.value = 0x00080000, .expected_output = 12},
	{.value = 0x00000800, .expected_output = 20},
	{.value = 0x00000008, .expected_output = 28},

	{.value = 0x07000000, .expected_output = 5},
	{.value = 0x00070000, .expected_output = 13},
	{.value = 0x00000700, .expected_output = 21},
	{.value = 0x00000007, .expected_output = 29},

	{.value = 0x03000000, .expected_output = 6},
	{.value = 0x00030000, .expected_output = 14},
	{.value = 0x00000300, .expected_output = 22},
	{.value = 0x00000003, .expected_output = 30},

	{.value = 0x01000000, .expected_output = 7},
	{.value = 0x00010000, .expected_output = 15},
	{.value = 0x00000100, .expected_output = 23},
	{.value = 0x00000001, .expected_output = 31},
};

void test_clzsi2_with_data(void **state)
{
	for (int i = 0; i < ARRAY_SIZE(test_data); ++i)
		assert_int_equal(test_data[i].expected_output, __clzsi2(test_data[i].value));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_clzsi2_with_data),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
