/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/dram/lpddr4.h>
#include <string.h>
#include <memory_info.h>
#include <smbios.h>
#include <types.h>

enum lpddr4_speed_grade {
	LPDDR4_1600,
	LPDDR4_2400,
	LPDDR4_3200,
	LPDDR4_4266
};

struct lpddr4_speed_attr {
	uint32_t min_clock_mhz; // inclusive
	uint32_t max_clock_mhz; // inclusive
	uint32_t reported_mts;
};

/**
 * LPDDR4 speed attributes derived from JEDEC 209-4C table 210
 *
 * min_clock_mhz = Previous max_clock_mhz + 1
 * max_clock_mhz = 1000/min_tCk_avg(ns)
 * reported_mts  = Standard reported DDR4 speed in MT/s
 *                 May be slightly less than the actual max MT/s
 */
static const struct lpddr4_speed_attr lpddr4_speeds[] = {
	[LPDDR4_1600] = {
		.min_clock_mhz = 10,
		.max_clock_mhz = 800,
		.reported_mts = 1600
	},
	[LPDDR4_2400] = {
		.min_clock_mhz = 801,
		.max_clock_mhz = 1200,
		.reported_mts = 2400
	},
	[LPDDR4_3200] = {
		.min_clock_mhz = 1201,
		.max_clock_mhz = 1600,
		.reported_mts = 3200
	},
	[LPDDR4_4266] = {
		.min_clock_mhz = 1601,
		.max_clock_mhz = 2137,
		.reported_mts = 4266
	},
};

/**
 * Converts LPDDR4 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t lpddr4_speed_mhz_to_reported_mts(uint16_t speed_mhz)
{
	for (enum lpddr4_speed_grade speed = 0; speed < ARRAY_SIZE(lpddr4_speeds); speed++) {
		const struct lpddr4_speed_attr *speed_attr = &lpddr4_speeds[speed];
		if (speed_mhz >= speed_attr->min_clock_mhz &&
		    speed_mhz <= speed_attr->max_clock_mhz) {
			return speed_attr->reported_mts;
		}
	}
	printk(BIOS_ERR, "ERROR: LPDDR4 speed of %d MHz is out of range", speed_mhz);
	return 0;
}
