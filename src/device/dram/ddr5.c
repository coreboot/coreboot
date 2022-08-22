/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/dram/ddr5.h>
#include <memory_info.h>
#include <smbios.h>
#include <types.h>

enum ddr5_speed_grade {
	DDR5_1333,
	DDR5_1600,
	DDR5_1866,
	DDR5_2133,
	DDR5_2400,
	DDR5_2667,
	DDR5_2933,
	DDR5_3200,
	DDR5_3733,
	DDR5_4267,
	DDR5_4800,
	DDR5_5500,
	DDR5_6000,
	DDR5_6400,
};

struct ddr5_speed_attr {
	uint32_t min_clock_mhz; // inclusive
	uint32_t max_clock_mhz; // inclusive
	uint32_t reported_mts;
};

/**
 * (LP)DDR5 speed attributes derived from JEDEC JESD79-5B, JESD209-5B and industry norms
 *
 * min_clock_mhz = previous max speed + 1
 * max_clock_mhz = 50% of speed grade, +/- 1
 * reported_mts  = Standard reported DDR5 speed in MT/s
 *                 May be slightly less than the actual max MT/s
 */
static const struct ddr5_speed_attr ddr5_speeds[] = {
	[DDR5_1333] = {
		.min_clock_mhz = 10,
		.max_clock_mhz = 667,
		.reported_mts = 1333,
	},
	[DDR5_1600] = {
		.min_clock_mhz = 668,
		.max_clock_mhz = 800,
		.reported_mts = 1600,
	},
	[DDR5_1866] = {
		.min_clock_mhz = 801,
		.max_clock_mhz = 933,
		.reported_mts = 1866,
	},
	[DDR5_2133] = {
		.min_clock_mhz = 934,
		.max_clock_mhz = 1067,
		.reported_mts = 2133,
	},
	[DDR5_2400] = {
		.min_clock_mhz = 1068,
		.max_clock_mhz = 1200,
		.reported_mts = 2400,
	},
	[DDR5_2667] = {
		.min_clock_mhz = 1201,
		.max_clock_mhz = 1333,
		.reported_mts = 2667,
	},
	[DDR5_2933] = {
		.min_clock_mhz = 1334,
		.max_clock_mhz = 1467,
		.reported_mts = 2933,
	},
	[DDR5_3200] = {
		.min_clock_mhz = 1468,
		.max_clock_mhz = 1600,
		.reported_mts = 3200,
	},
	[DDR5_3733] = {
		.min_clock_mhz = 1601,
		.max_clock_mhz = 1866,
		.reported_mts = 3733
	},
	[DDR5_4267] = {
		.min_clock_mhz = 1867,
		.max_clock_mhz = 2133,
		.reported_mts = 4267
	},
	[DDR5_4800] = {
		.min_clock_mhz = 2134,
		.max_clock_mhz = 2400,
		.reported_mts = 4800
	},
	[DDR5_5500] = {
		.min_clock_mhz = 2401,
		.max_clock_mhz = 2750,
		.reported_mts = 5500
	},
	[DDR5_6000] = {
		.min_clock_mhz = 2751,
		.max_clock_mhz = 3000,
		.reported_mts = 6000
	},
	[DDR5_6400] = {
		.min_clock_mhz = 3001,
		.max_clock_mhz = 3200,
		.reported_mts = 6400
	},
};

/**
 * Converts DDR5 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t ddr5_speed_mhz_to_reported_mts(uint16_t speed_mhz)
{
	for (enum ddr5_speed_grade speed = 0; speed < ARRAY_SIZE(ddr5_speeds); speed++) {
		const struct ddr5_speed_attr *speed_attr = &ddr5_speeds[speed];
		if (speed_mhz >= speed_attr->min_clock_mhz &&
		    speed_mhz <= speed_attr->max_clock_mhz) {
			return speed_attr->reported_mts;
		}
	}
	printk(BIOS_ERR, "DDR5 speed of %d MHz is out of range\n", speed_mhz);
	return 0;
}
