/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/dram/ddr4.h>
#include <tests/test.h>

static void ddr4_speed_mhz_to_mts_test(void **state)
{
	assert_int_equal(0, ddr4_speed_mhz_to_reported_mts(0));
	assert_int_equal(0, ddr4_speed_mhz_to_reported_mts(667));

	assert_int_equal(1600, ddr4_speed_mhz_to_reported_mts(668));
	assert_int_equal(1600, ddr4_speed_mhz_to_reported_mts(800));

	assert_int_equal(1866, ddr4_speed_mhz_to_reported_mts(801));
	assert_int_equal(1866, ddr4_speed_mhz_to_reported_mts(933));
	assert_int_equal(1866, ddr4_speed_mhz_to_reported_mts(934));

	assert_int_equal(2133, ddr4_speed_mhz_to_reported_mts(1066));
	assert_int_equal(2133, ddr4_speed_mhz_to_reported_mts(1067));

	assert_int_equal(2400, ddr4_speed_mhz_to_reported_mts(1200));

	assert_int_equal(2666, ddr4_speed_mhz_to_reported_mts(1333));

	assert_int_equal(2933, ddr4_speed_mhz_to_reported_mts(1466));

	assert_int_equal(3200, ddr4_speed_mhz_to_reported_mts(1467));
	assert_int_equal(3200, ddr4_speed_mhz_to_reported_mts(1600));

	assert_int_equal(0, ddr4_speed_mhz_to_reported_mts(1601));
	assert_int_equal(0, ddr4_speed_mhz_to_reported_mts(INT16_MAX));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(ddr4_speed_mhz_to_mts_test)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
