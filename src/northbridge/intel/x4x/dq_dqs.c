/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Arthur Heymans <arthur@aheymans.xyz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <stdint.h>
#include <string.h>
#include <types.h>
#include "x4x.h"
#include "iomap.h"

static void print_dll_setting(const struct dll_setting *dll_setting,
			u8 default_verbose)
{
	u8 debug_level = default_verbose ? BIOS_DEBUG : RAM_DEBUG;

	printk(debug_level, "%d.%d.%d.%d:%d.%d\n", dll_setting->coarse,
		dll_setting->clk_delay, dll_setting->tap,
		dll_setting->pi, dll_setting->db_en,
		dll_setting->db_sel);
}

struct db_limit {
	u8 tap0;
	u8 tap1;
	u8 pi0;
	u8 pi1;
};

static void set_db(const struct sysinfo *s, struct dll_setting *dq_dqs_setting)
{
        struct db_limit limit;

	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_800MHz:
		limit.tap0 = 3;
		limit.tap1 = 10;
		limit.pi0 = 2;
		limit.pi1 = 3;
		break;
	case MEM_CLOCK_1066MHz:
		limit.tap0 = 2;
		limit.tap1 = 8;
		limit.pi0 = 6;
		limit.pi1 = 7;
		break;
	case MEM_CLOCK_1333MHz:
		limit.tap0 = 3;
		limit.tap1 = 11;
		/* TO CHECK: Might be reverse since this makes little sense */
		limit.pi0 = 6;
		limit.pi1 = 4;
		break;
	}

	if (dq_dqs_setting->tap < limit.tap0) {
		dq_dqs_setting->db_en = 1;
		dq_dqs_setting->db_sel = 1;
	} else if ((dq_dqs_setting->tap == limit.tap0)
			&& (dq_dqs_setting->pi < limit.pi0)) {
		dq_dqs_setting->db_en = 1;
		dq_dqs_setting->db_sel = 1;
	} else if (dq_dqs_setting->tap < limit.tap1) {
		dq_dqs_setting->db_en = 0;
		dq_dqs_setting->db_sel = 0;
	} else if ((dq_dqs_setting->tap == limit.tap1)
			&& (dq_dqs_setting->pi < limit.pi1)) {
		dq_dqs_setting->db_en = 0;
		dq_dqs_setting->db_sel = 0;
	} else {
		dq_dqs_setting->db_en = 1;
		dq_dqs_setting->db_sel = 0;
	}
}

const static u8 max_tap[3] = {12, 10, 13};

static int increment_dq_dqs(const struct sysinfo *s,
			struct dll_setting *dq_dqs_setting)
{
	u8 max_tap_val = max_tap[s->selected_timings.mem_clk
				- MEM_CLOCK_800MHz];

	if (dq_dqs_setting->pi < 6) {
		dq_dqs_setting->pi += 1;
	} else if (dq_dqs_setting->tap < max_tap_val) {
		dq_dqs_setting->pi = 0;
		dq_dqs_setting->tap += 1;
	} else if (dq_dqs_setting->clk_delay < 2) {
		dq_dqs_setting->pi = 0;
		dq_dqs_setting->tap = 0;
		dq_dqs_setting->clk_delay += 1;
	} else if (dq_dqs_setting->coarse < 1) {
		dq_dqs_setting->pi = 0;
		dq_dqs_setting->tap = 0;
		dq_dqs_setting->clk_delay -= 1;
		dq_dqs_setting->coarse += 1;
	} else {
		return CB_ERR;
	}
	set_db(s, dq_dqs_setting);
	return CB_SUCCESS;
}

#define WT_PATTERN_SIZE 80

static const u32 write_training_schedule[WT_PATTERN_SIZE] = {
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0x03030303, 0x04040404, 0x09090909, 0x10101010,
	0x21212121, 0x40404040, 0x81818181, 0x00000000,
	0x03030303, 0x04040404, 0x09090909, 0x10101010,
	0x21212121, 0x40404040, 0x81818181, 0x00000000,
	0xfdfdfdfd, 0xfafafafa, 0xf7f7f7f7, 0xeeeeeeee,
	0xdfdfdfdf, 0xbebebebe, 0x7f7f7f7f, 0xfefefefe,
	0xfdfdfdfd, 0xfafafafa, 0xf7f7f7f7, 0xeeeeeeee,
	0xdfdfdfdf, 0xbebebebe, 0x7f7f7f7f, 0xfefefefe,
};

enum training_modes {
	SUCCEEDING = 0,
	FAILING = 1
};

static u8 test_dq_aligned(const struct sysinfo *s,
					const u8 channel)
{
	u32 address;
	int rank, lane;
	u8 count, count1;
	u8 data[8];
	u8 lane_error = 0;

	FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, channel, rank) {
		address = test_address(channel, rank);
		for (count = 0; count < WT_PATTERN_SIZE; count++) {
			for (count1 = 0; count1 < WT_PATTERN_SIZE; count1++) {
				if ((count1 % 16) == 0)
					MCHBAR32(0xf90) = 1;
				const u32 pattern =
					write_training_schedule[count1];
				write32((u32 *)address + 8 * count1, pattern);
				write32((u32 *)address + 8 * count1 + 4,
					pattern);
			}

			const u32 good = write_training_schedule[count];
			write32(&data[0], read32((u32 *)address + 8 * count));
			write32(&data[4],
				read32((u32 *)address + 8 * count + 4));
			FOR_EACH_BYTELANE(lane) {
				u8 expected = (good >> ((lane % 4) * 8)) & 0xff;
				if (data[lane] != expected)
					lane_error |= 1 << lane;
			}
		}
	}
	return lane_error;
}

#define CONSISTENCY 10

/*
 * This function finds either failing or succeeding writes by increasing DQ.
 * When it has found a failing or succeeding setting it will increase DQ
 * another 10 times to make sure the result is consistent.
 * This is probably done because lanes cannot be trained independent from
 * each other.
 */
static int find_dq_limit(const struct sysinfo *s, const u8 channel,
			struct dll_setting dq_setting[TOTAL_BYTELANES],
			u8 dq_lim[TOTAL_BYTELANES],
			const enum training_modes expected_result)
{
	int status = CB_SUCCESS;
	int lane;
	u8 test_result;
	u8 pass_count[TOTAL_BYTELANES];
	u8 succes_mask = 0xff;

	printk(RAM_DEBUG, "Looking for %s writes on channel %d\n",
		expected_result == FAILING ? "failing" : "succeeding", channel);
	memset(pass_count, 0, sizeof(pass_count));

	while(succes_mask) {
		test_result = test_dq_aligned(s, channel);
		FOR_EACH_BYTELANE(lane) {
			if (((test_result >> lane) & 1) != expected_result) {
				status = increment_dq_dqs(s, &dq_setting[lane]);
				dqset(channel, lane, &dq_setting[lane]);
				dq_lim[lane]++;
			} else if (pass_count[lane] < CONSISTENCY) {
				status = increment_dq_dqs(s, &dq_setting[lane]);
				dqset(channel, lane, &dq_setting[lane]);
				dq_lim[lane]++;
				pass_count[lane]++;
			} else if (pass_count[lane] == CONSISTENCY) {
				succes_mask &= ~(1 << lane);
			}
			if (status == CB_ERR) {
				printk(BIOS_CRIT, "Could not find a case of %s "
					"writes on CH%d, lane %d\n",
					expected_result == FAILING ? "failing"
					: "succeeding", channel, lane);
				return CB_ERR;
			}
		}
	}
	return CB_SUCCESS;
}

/*
 * This attempts to find the ideal delay for DQ to account for the skew between
 * the DQ and the DQS signal.
 * The training works this way:
 * - start from the DQS delay values (DQ is always later than DQS)
 * - increment the DQ delay until a succeeding write is found on all bytelayes,
 *   on all ranks on a channel and save these values
 * - again increment the DQ delay until write start to fail on all bytelanes and
 *   save that value
 * - use the mean between the saved succeeding and failing value
 * - note: bytelanes cannot be trained independently, so the delays need to be
 *   adjusted and tested for all of them at the same time
 */
int do_write_training(struct sysinfo *s)
{
	int i;
	u8 channel, lane;
	u8 dq_lower[TOTAL_BYTELANES];
	u8 dq_upper[TOTAL_BYTELANES];
	struct dll_setting dq_setting[TOTAL_BYTELANES];
	u8 dq_average;
	u32 dq_absolute;

	printk(BIOS_DEBUG, "Starting DQ write training\n");

	FOR_EACH_POPULATED_CHANNEL(s->dimms, channel) {
		printk(BIOS_DEBUG, "Doing DQ write training on CH%d\n", channel);

		dq_average = 0;
		dq_absolute = 0;
		/* Start all lanes at DQS values */
		FOR_EACH_BYTELANE(lane) {
			dqset(channel, lane, &s->dqs_settings[channel][lane]);
			s->dq_settings[channel][lane] = s->dqs_settings[channel][lane];
		}
		memset(dq_lower, 0, sizeof(dq_lower));
			/* Start from DQS settings */
		memcpy(dq_setting, s->dqs_settings[channel], sizeof(dq_setting));

		if (find_dq_limit(s, channel, dq_setting, dq_lower,
					SUCCEEDING)) {
			printk(BIOS_CRIT,
				"Could not find working lower limit DQ setting\n");
			return CB_ERR;
		}

		memcpy(dq_upper, dq_lower, sizeof(dq_lower));

		if (find_dq_limit(s, channel, dq_setting, dq_upper,
					FAILING)) {
			printk(BIOS_WARNING,
				"Could not find failing upper limit DQ setting\n");
			return CB_ERR;
		}

		FOR_EACH_BYTELANE(lane) {
			dq_lower[lane] -= CONSISTENCY - 1;
			dq_upper[lane] -= CONSISTENCY - 1;
			u8 dq_center = (dq_upper[lane] + dq_lower[lane]) / 2;

			printk(RAM_DEBUG, "Centered value for DQ DLL:"
				" ch%d, lane %d, #steps = %d\n",
				channel, lane, dq_center);
			for (i = 0; i < dq_center; i++) {
				/* Should never happen */
				if (increment_dq_dqs(s, &s->dq_settings[channel][lane])
					== CB_ERR)
					printk(BIOS_ERR,
						"Huh? write training overflowed!!\n");
			}
		}

		/* Reset DQ DLL settings and increment with centered value*/
		printk(BIOS_DEBUG, "Final DQ timings on CH%d\n", channel);
	        FOR_EACH_BYTELANE(lane) {
			printk(BIOS_DEBUG, "\tlane%d: ", lane);
			print_dll_setting(&s->dq_settings[channel][lane], 1);
			dqset(channel, lane, &s->dq_settings[channel][lane]);
		}
	}
	printk(BIOS_DEBUG, "Done DQ write training\n");
	return CB_SUCCESS;
}

#define RT_PATTERN_SIZE 40

static const u32 read_training_schedule[RT_PATTERN_SIZE] = {
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0x03030303, 0x04040404, 0x09090909, 0x10101010,
	0x21212121, 0x40404040, 0x81818181, 0x00000000,
	0xfdfdfdfd, 0xfafafafa, 0xf7f7f7f7, 0xeeeeeeee,
	0xdfdfdfdf, 0xbebebebe, 0x7f7f7f7f, 0xfefefefe
};

static int rt_increment_dqs(struct rt_dqs_setting *setting)
{
	if (setting->pi < 7) {
		setting->pi++;
	} else if (setting->tap < 14) {
		setting->pi = 0;
		setting->tap++;
	} else {
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static u8 test_dqs_aligned(const struct sysinfo *s, const u8 channel)
{
	int i, rank, lane;
	volatile u8 data[8];
	u32 address;
	u8 bytelane_error = 0;

	FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, channel, rank) {
		address = test_address(channel, rank);
		for (i = 0; i < RT_PATTERN_SIZE; i++) {
			const u32 good = read_training_schedule[i];
			write32(&data[0], read32((u32 *)address + i * 8));
			write32(&data[4], read32((u32 *)address + i * 8 + 4));

			FOR_EACH_BYTELANE(lane) {
				if (data[lane] != (good & 0xff))
					bytelane_error |= 1 << lane;
			}
		}
	}
	return bytelane_error;
}

static int rt_find_dqs_limit(struct sysinfo *s, u8 channel,
			struct rt_dqs_setting dqs_setting[TOTAL_BYTELANES],
			u8 dqs_lim[TOTAL_BYTELANES],
			const enum training_modes expected_result)
{
	int lane;
	u8 test_result;
	int status = CB_SUCCESS;

	FOR_EACH_BYTELANE(lane)
		rt_set_dqs(channel, lane, 0, &dqs_setting[lane]);

	while(status == CB_SUCCESS) {
		test_result = test_dqs_aligned(s, channel);
		if (test_result == (expected_result == SUCCEEDING ? 0 : 0xff))
			return CB_SUCCESS;
		FOR_EACH_BYTELANE(lane) {
			if (((test_result >> lane) & 1) != expected_result) {
				status = rt_increment_dqs(&dqs_setting[lane]);
				dqs_lim[lane]++;
				rt_set_dqs(channel, lane, 0, &dqs_setting[lane]);
			}
		}
	}

	if (expected_result == SUCCEEDING) {
		printk(BIOS_CRIT,
			"Could not find RT DQS setting\n");
		return CB_ERR;
	} else {
		printk(RAM_DEBUG,
			"Read succeeded over all DQS"
			" settings, continuing\n");
		return CB_SUCCESS;
	}
}

#define RT_LOOPS 3

/*
 * This attempts to find the ideal delay for DQS on reads (rx).
 * The training works this way:
 * - start from the lowest possible delay (0) on all bytelanes
 * - increment the DQS rx delays until a succeeding write is found on all
 *   bytelayes, on all ranks on a channel and save these values
 * - again increment the DQS rx delay until write start to fail on all bytelanes
 *   and save that value
 * - use the mean between the saved succeeding and failing value
 * - note0: bytelanes cannot be trained independently, so the delays need to be
 *   adjusted and tested for all of them at the same time
 * - note1: this memory controller appears to have per rank registers for these
 *   DQS rx delays, but only the one rank 0 seems to be used for all of them
 */
int do_read_training(struct sysinfo *s)
{
	int loop, channel, i, lane, rank;
	u32 address, content;
	u8 dqs_lower[TOTAL_BYTELANES];
	u8 dqs_upper[TOTAL_BYTELANES];
	struct rt_dqs_setting dqs_setting[TOTAL_BYTELANES];
	u16 saved_dqs_center[TOTAL_CHANNELS][TOTAL_BYTELANES];

	memset(saved_dqs_center, 0 , sizeof(saved_dqs_center));

	printk(BIOS_DEBUG, "Starting DQS read training\n");

	for (loop = 0; loop < RT_LOOPS; loop++) {
		FOR_EACH_POPULATED_CHANNEL(s->dimms, channel) {
			printk(RAM_DEBUG, "Doing DQS read training on CH%d\n",
				channel);

			/* Write pattern to strobe address */
			FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, channel, rank) {
				address = test_address(channel, rank);
				for (i = 0; i < RT_PATTERN_SIZE; i++) {
					content = read_training_schedule[i];
					write32((u32 *)address + 8 * i, content);
					write32((u32 *)address + 8 * i + 4, content);
				}
			}

			memset(dqs_lower, 0, sizeof(dqs_lower));
			memset(&dqs_setting, 0, sizeof(dqs_setting));
			if (rt_find_dqs_limit(s, channel, dqs_setting, dqs_lower,
						SUCCEEDING)) {
				printk(BIOS_CRIT,
					"Could not find working lower limit DQS setting\n");
				return CB_ERR;
			}

			FOR_EACH_BYTELANE(lane)
				dqs_upper[lane] = dqs_lower[lane];

			if (rt_find_dqs_limit(s, channel, dqs_setting, dqs_upper,
						FAILING)) {
				printk(BIOS_CRIT,
					"Could not find failing upper limit DQ setting\n");
				return CB_ERR;
			}

			printk(RAM_DEBUG, "Centered values, loop %d:\n", loop);
			FOR_EACH_BYTELANE(lane) {
				u8 center = (dqs_lower[lane] + dqs_upper[lane]) / 2;
				printk(RAM_DEBUG, "\t lane%d: #%d\n", lane, center);
				saved_dqs_center[channel][lane] += center;
			}
		} /* END FOR_EACH_POPULATED_CHANNEL */
	} /* end RT_LOOPS */

	memset(s->rt_dqs, 0, sizeof(s->rt_dqs));

	FOR_EACH_POPULATED_CHANNEL(s->dimms, channel) {
		printk(BIOS_DEBUG, "Final timings on CH%d:\n", channel);
		FOR_EACH_BYTELANE(lane) {
			saved_dqs_center[channel][lane] /= RT_LOOPS;
			while (saved_dqs_center[channel][lane]--) {
				if(rt_increment_dqs(&s->rt_dqs[channel][lane])
							== CB_ERR)
					/* Should never happen */
					printk(BIOS_ERR,
						"Huh? read training overflowed!!\n");
			}
			FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, channel, rank)
				rt_set_dqs(channel, lane, rank,
					&s->rt_dqs[channel][lane]);
			printk(BIOS_DEBUG, "\tlane%d: %d.%d\n",
				lane, s->rt_dqs[channel][lane].tap,
				s->rt_dqs[channel][lane].pi);
		}
	}
	printk(BIOS_DEBUG, "Done DQS read training\n");
	return CB_SUCCESS;
}
