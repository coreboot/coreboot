/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <string.h>
#include <types.h>
#include "raminit.h"
#include "x4x.h"

static void print_dll_setting(const struct dll_setting *dll_setting, u8 default_verbose)
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

static const u8 max_tap[3] = {12, 10, 13};

static enum cb_err increment_dq_dqs(const struct sysinfo *s, struct dll_setting *dq_dqs_setting)
{
	u8 max_tap_val = max_tap[s->selected_timings.mem_clk - MEM_CLOCK_800MHz];

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

static enum cb_err decrement_dq_dqs(const struct sysinfo *s, struct dll_setting *dq_dqs_setting)
{
	u8 max_tap_val = max_tap[s->selected_timings.mem_clk - MEM_CLOCK_800MHz];

	if (dq_dqs_setting->pi > 0) {
		dq_dqs_setting->pi -= 1;
	} else if (dq_dqs_setting->tap > 0) {
		dq_dqs_setting->pi = 6;
		dq_dqs_setting->tap -= 1;
	} else if (dq_dqs_setting->clk_delay > 0) {
		dq_dqs_setting->pi = 6;
		dq_dqs_setting->tap = max_tap_val;
		dq_dqs_setting->clk_delay -= 1;
	} else if (dq_dqs_setting->coarse > 0) {
		dq_dqs_setting->pi = 6;
		dq_dqs_setting->tap = max_tap_val;
		dq_dqs_setting->clk_delay += 1;
		dq_dqs_setting->coarse -= 1;
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

static u8 test_dq_aligned(const struct sysinfo *s, const u8 channel)
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
					mchbar_write32(0xf90, 1);
				const u32 pattern = write_training_schedule[count1];
				write32p(address + 8 * count1, pattern);
				write32p(address + 8 * count1 + 4, pattern);
			}

			const u32 good = write_training_schedule[count];
			write32(&data[0], read32p(address + 8 * count));
			write32(&data[4], read32p(address + 8 * count + 4));
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
static enum cb_err find_dq_limit(const struct sysinfo *s, const u8 channel,
			struct dll_setting dq_setting[TOTAL_BYTELANES],
			u8 dq_lim[TOTAL_BYTELANES],
			const enum training_modes expected_result)
{
	enum cb_err status = CB_SUCCESS;
	int lane;
	u8 test_result;
	u8 pass_count[TOTAL_BYTELANES];
	u8 success_mask = 0xff;

	printk(RAM_DEBUG, "Looking for %s writes on channel %d\n",
		expected_result == FAILING ? "failing" : "succeeding", channel);
	memset(pass_count, 0, sizeof(pass_count));

	while (success_mask) {
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
				success_mask &= ~(1 << lane);
			}
			if (status == CB_ERR) {
				printk(BIOS_CRIT,
					"Could not find a case of %s writes on CH%d, lane %d\n",
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
enum cb_err do_write_training(struct sysinfo *s)
{
	int i;
	u8 channel, lane;
	u8 dq_lower[TOTAL_BYTELANES];
	u8 dq_upper[TOTAL_BYTELANES];
	struct dll_setting dq_setting[TOTAL_BYTELANES];

	printk(BIOS_DEBUG, "Starting DQ write training\n");

	FOR_EACH_POPULATED_CHANNEL(s->dimms, channel) {
		printk(BIOS_DEBUG, "Doing DQ write training on CH%d\n", channel);

		/* Start all lanes at DQS values */
		FOR_EACH_BYTELANE(lane) {
			dqset(channel, lane, &s->dqs_settings[channel][lane]);
			s->dq_settings[channel][lane] = s->dqs_settings[channel][lane];
		}
		memset(dq_lower, 0, sizeof(dq_lower));
		/* Start from DQS settings */
		memcpy(dq_setting, s->dqs_settings[channel], sizeof(dq_setting));

		if (find_dq_limit(s, channel, dq_setting, dq_lower, SUCCEEDING)) {
			printk(BIOS_CRIT, "Could not find working lower limit DQ setting\n");
			return CB_ERR;
		}

		memcpy(dq_upper, dq_lower, sizeof(dq_lower));

		if (find_dq_limit(s, channel, dq_setting, dq_upper, FAILING)) {
			printk(BIOS_WARNING, "Could not find failing upper limit DQ setting\n");
			return CB_ERR;
		}

		FOR_EACH_BYTELANE(lane) {
			dq_lower[lane] -= CONSISTENCY - 1;
			dq_upper[lane] -= CONSISTENCY - 1;
			u8 dq_center = (dq_upper[lane] + dq_lower[lane]) / 2;

			printk(RAM_DEBUG,
				"Centered value for DQ DLL: ch%d, lane %d, #steps = %d\n",
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

static enum cb_err rt_increment_dqs(struct rt_dqs_setting *setting)
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
			write32(&data[0], read32p(address + i * 8));
			write32(&data[4], read32p(address + i * 8 + 4));

			FOR_EACH_BYTELANE(lane) {
				if (data[lane] != (good & 0xff))
					bytelane_error |= 1 << lane;
			}
		}
	}
	return bytelane_error;
}

static enum cb_err rt_find_dqs_limit(struct sysinfo *s, u8 channel,
			struct rt_dqs_setting dqs_setting[TOTAL_BYTELANES],
			u8 dqs_lim[TOTAL_BYTELANES],
			const enum training_modes expected_result)
{
	int lane;
	u8 test_result;
	enum cb_err status = CB_SUCCESS;

	FOR_EACH_BYTELANE(lane)
		rt_set_dqs(channel, lane, 0, &dqs_setting[lane]);

	while (status == CB_SUCCESS) {
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
		printk(BIOS_CRIT, "Could not find RT DQS setting\n");
		return CB_ERR;
	} else {
		printk(RAM_DEBUG, "Read succeeded over all DQS settings, continuing\n");
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
 * - note1: At this stage all ranks effectively use the rank0's rt_dqs settings,
 *   but later on their respective settings are used (TODO where is the
 *   'switch' register??). So programming the results for all ranks at the end
 *   of the training. Programming on all ranks instead of all populated ranks,
 *   seems to be required, most likely because the signals can't really be generated
 *   separately.
 */
enum cb_err do_read_training(struct sysinfo *s)
{
	int loop, channel, i, lane, rank;
	u32 address, content;
	u8 dqs_lower[TOTAL_BYTELANES];
	u8 dqs_upper[TOTAL_BYTELANES];
	struct rt_dqs_setting dqs_setting[TOTAL_BYTELANES];
	u16 saved_dqs_center[TOTAL_CHANNELS][TOTAL_BYTELANES];

	memset(saved_dqs_center, 0, sizeof(saved_dqs_center));

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
					write32p(address + 8 * i, content);
					write32p(address + 8 * i + 4, content);
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
				if (rt_increment_dqs(&s->rt_dqs[channel][lane])
							== CB_ERR)
					/* Should never happen */
					printk(BIOS_ERR,
						"Huh? read training overflowed!!\n");
			}
			/* Later on separate settings for each rank are used so program
			   all of them */
			FOR_EACH_RANK_IN_CHANNEL(rank)
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

/* Enable write leveling on selected rank and disable output on other ranks */
static void set_rank_write_level(struct sysinfo *s, u8 channel, u8 config,
				u8 config_rank, u8 target_rank, int wl_enable)
{
	u32 emrs1;

	/* Is shifted by bits 2 later so u8 can be used to reduce size */
	static const u8 emrs1_lut[8][4][4] = { /* [Config][Leveling Rank][Rank] */
		{ /* Config 0: 2R2R */
			{0x11, 0x00, 0x91, 0x00},
			{0x00, 0x11, 0x91, 0x00},
			{0x91, 0x00, 0x11, 0x00},
			{0x91, 0x00, 0x00, 0x11}
		},
		{ /* Config 1: 2R1R */
			{0x11, 0x00, 0x91, 0x00},
			{0x00, 0x11, 0x91, 0x00},
			{0x91, 0x00, 0x11, 0x00},
			{0x00, 0x00, 0x00, 0x00}
		},
		{ /* Config 2: 1R2R */
			{0x11, 0x00, 0x91, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x91, 0x00, 0x11, 0x00},
			{0x91, 0x00, 0x00, 0x11}
		},
		{ /* Config 3: 1R1R */
			{0x11, 0x00, 0x91, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x91, 0x00, 0x11, 0x00},
			{0x00, 0x00, 0x00, 0x00}
		},
		{ /* Config 4: 2R0R */
			{0x11, 0x00, 0x00, 0x00},
			{0x00, 0x11, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00}
		},
		{ /* Config 5: 0R2R */
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x11, 0x00},
			{0x00, 0x00, 0x00, 0x11}
		},
		{ /* Config 6: 1R0R */
			{0x11, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00}
		},
		{ /* Config 7: 0R1R */
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x00, 0x00},
			{0x00, 0x00, 0x11, 0x00},
			{0x00, 0x00, 0x00, 0x00}
		}
	};

	if (wl_enable) {
		printk(RAM_DEBUG, "Entering WL mode\n");
		printk(RAM_DEBUG, "Using WL ODT values\n");
		emrs1 = emrs1_lut[config][target_rank][config_rank];
	} else {
		printk(RAM_DEBUG, "Exiting WL mode\n");
		emrs1 = ddr3_emrs1_rtt_nom_config[s->dimm_config[channel]][config_rank];
	}
	printk(RAM_DEBUG, "Setting ODT for rank%d to ", config_rank);
	switch (emrs1) {
	case 0:
		printk(RAM_DEBUG, "High-Z\n");
		break;
	case 0x11:
		printk(RAM_DEBUG, "40 Ohm\n");
		break;
	case 0x81:
		printk(RAM_DEBUG, "30 Ohm\n");
		break;
	case 0x80:
		printk(RAM_DEBUG, "20 Ohm\n");
		break;
	case 0x10:
		printk(RAM_DEBUG, "120 Ohm\n");
		break;
	case 0x01:
		printk(RAM_DEBUG, "60 Ohm\n");
		break;
	default:
		printk(BIOS_WARNING, "ODT value Undefined!\n");
		break;
	}

	emrs1 <<= 2;
	/* Set output drive strength to 34 Ohm during write levelling */
	emrs1 |= (1 << 1);

	if (wl_enable && (target_rank != config_rank)) {
		printk(RAM_DEBUG, "Disabling output for rank%d\n", config_rank);
		emrs1 |= (1 << 12);
	}
	if (wl_enable && (target_rank == config_rank)) {
		printk(RAM_DEBUG, "Enabling WL for rank%d\n", config_rank);
		emrs1 |= (1 << 7);
	}
	send_jedec_cmd(s, config_rank, channel, EMRS1_CMD, emrs1);
}

#define N_SAMPLES 5

static void sample_dq(const struct sysinfo *s, u8 channel, u8 rank,
		u8 high_found[8]) {
	u32 address = test_address(channel, rank);
	int samples, lane;

	memset(high_found, 0, TOTAL_BYTELANES * sizeof(high_found[0]));
	for (samples = 0; samples < N_SAMPLES; samples++) {
		write32p(address, 0x12341234);
		write32p(address + 4, 0x12341234);
		udelay(5);
		FOR_EACH_BYTELANE(lane) {
			u8 dq_high = (mchbar_read8(0x561 + 0x400 * channel
					+ (lane * 4)) >> 7) & 1;
			high_found[lane] += dq_high;
		}
	}
}

static enum cb_err increment_to_dqs_edge(struct sysinfo *s, u8 channel, u8 rank)
{
	int lane;
	u8 saved_24d;
	struct dll_setting dqs_setting[TOTAL_BYTELANES];
	u8 bytelane_ok = 0;
	u8 dq_sample[TOTAL_BYTELANES];

	memcpy(dqs_setting, s->dqs_settings[channel], sizeof(dqs_setting));
	FOR_EACH_BYTELANE(lane)
		dqsset(channel, lane, &dqs_setting[lane]);

	saved_24d = mchbar_read8(0x24d + 0x400 * channel);

	/* Loop 0: Find DQ sample low, by decreasing */
	while (bytelane_ok != 0xff) {
		sample_dq(s, channel, rank, dq_sample);
		FOR_EACH_BYTELANE(lane) {
			if (bytelane_ok & (1 << lane))
				continue;

			printk(RAM_SPEW, "%d, %d, %02d, %d, lane%d sample: %d\n",
				dqs_setting[lane].coarse,
				dqs_setting[lane].clk_delay,
				dqs_setting[lane].tap,
				dqs_setting[lane].pi,
				lane,
				dq_sample[lane]);

			if (dq_sample[lane] == 0) {
				bytelane_ok |= (1 << lane);
			} else if (decrement_dq_dqs(s, &dqs_setting[lane])) {
				printk(BIOS_EMERG,
					"DQS setting channel%d, lane %d reached a minimum!\n",
					channel, lane);
				return CB_ERR;
			}
			dqsset(channel, lane, &dqs_setting[lane]);
		}
	}

	printk(RAM_DEBUG, "DQS settings on PASS #0:\n");
	FOR_EACH_BYTELANE(lane) {
		printk(RAM_DEBUG, "lane %d: ", lane);
		print_dll_setting(&dqs_setting[lane], 0);
	}

	/* Loop 1: Find DQ sample high, by increasing */
	bytelane_ok = 0;
	while (bytelane_ok != 0xff) {
		sample_dq(s, channel, rank, dq_sample);
		FOR_EACH_BYTELANE(lane) {
			if (bytelane_ok & (1 << lane))
				continue;

			printk(RAM_SPEW, "%d, %d, %02d, %d, lane%d sample: %d\n",
				dqs_setting[lane].coarse,
				dqs_setting[lane].clk_delay,
				dqs_setting[lane].tap,
				dqs_setting[lane].pi,
				lane,
				dq_sample[lane]);

			if (dq_sample[lane] == N_SAMPLES) {
				bytelane_ok |= (1 << lane);
			} else if (increment_dq_dqs(s, &dqs_setting[lane])) {
				printk(BIOS_EMERG,
					"DQS setting channel%d, lane %d reached a maximum!\n",
					channel, lane);
				return CB_ERR;
			}
			dqsset(channel, lane, &dqs_setting[lane]);
		}
	}

	printk(RAM_DEBUG, "DQS settings on PASS #1:\n");
	FOR_EACH_BYTELANE(lane) {
		printk(RAM_DEBUG, "lane %d: ", lane);
		print_dll_setting(&dqs_setting[lane], 0);
	}

	printk(BIOS_DEBUG, "final WL DQS settings on CH%d\n", channel);
	FOR_EACH_BYTELANE(lane) {
		printk(BIOS_DEBUG, "\tlane%d: ", lane);
		print_dll_setting(&dqs_setting[lane], 1);
		s->dqs_settings[channel][lane] = dqs_setting[lane];
	}

	mchbar_write8(0x24d + 0x400 * channel, saved_24d);
	return CB_SUCCESS;
}

/*
 * DDR3 uses flyby topology where the clock signal takes a different path
 * than the data signal, to allow for better signal intergrity.
 * Therefore the delay on the data signals needs to account for this.
 * This is done by sampling the DQS write (tx) signal back over the DQ
 * signal and looking for delay values where the sample transitions
 * from high to low.
 * Here the following is done:
 * - Enable write levelling on the first populated rank.
 * - Disable output on other populated ranks.
 * - Start from safe DQS (tx) delays. Other transitions can be
 *   found at different starting values but are generally bad.
 * - loop0: decrease DQS (tx) delays until low is sampled,
 *   loop1: increase DQS (tx) delays until high is sampled,
 *   This way, we are sure to have hit a low-high transition.
 * - Put all ranks in normal mode of operation again.
 * Note: All ranks need to be leveled together.
 */
void search_write_leveling(struct sysinfo *s)
{
	int i, ch, count;
	u8 config, rank0, rank1, lane;
	struct dll_setting dq_setting;

	const u8 chanconfig_lut[16] = {0, 6, 4, 6, 7, 3, 1, 3, 5, 2, 0, 2, 7, 3, 1, 3};

	const u8 odt_force[8][4] = { /* [Config][leveling rank] */
		{0x5, 0x6, 0x5, 0x9},
		{0x5, 0x6, 0x5, 0x0},
		{0x5, 0x0, 0x5, 0x9},
		{0x5, 0x0, 0x5, 0x0},
		{0x1, 0x2, 0x0, 0x0},
		{0x0, 0x0, 0x4, 0x8},
		{0x1, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x4, 0x0}
	};

	printk(BIOS_DEBUG, "Starting write levelling.\n");

	FOR_EACH_POPULATED_CHANNEL(s->dimms, ch) {
		printk(BIOS_DEBUG, "\tCH%d\n", ch);
		config = chanconfig_lut[s->dimm_config[ch]];

		mchbar_clrbits8(0x5d8 + 0x400 * ch, 0x0e);
		mchbar_clrsetbits16(0x5c4 + 0x400 * ch, 0x3fff, 0x3fff);
		mchbar_clrbits8(0x265 + 0x400 * ch, 0x1f);
		/* find the first populated rank */
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, ch, rank0)
			break;

		/* Enable WL for the first populated rank and disable output
		   for others */
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, ch, rank1)
			set_rank_write_level(s, ch, config, rank1, rank0, 1);

		mchbar_clrsetbits8(0x298 + 2 + 0x400 * ch, 0x0f, odt_force[config][rank0]);
		mchbar_clrsetbits8(0x271 + 0x400 * ch, 0x7e, 0x4e);
		mchbar_setbits8(0x5d9 + 0x400 * ch, 1 << 2);
		mchbar_clrsetbits32(0x1a0, 0x07ffffff, 0x00014000);

		if (increment_to_dqs_edge(s, ch, rank0))
			die("Write Leveling failed!");

		mchbar_clrbits8(0x298 + 2 + 0x400 * ch, 0x0f);
		mchbar_clrsetbits8(0x271 + 0x400 * ch, 0x7e, 0x0e);
		mchbar_clrbits8(0x5d9 + 0x400 * ch, 1 << 2);
		mchbar_clrsetbits32(0x1a0, 0x07ffffff, 0x00555801);

		/* Disable WL on the trained rank */
		set_rank_write_level(s, ch, config, rank0, rank0, 0);
		send_jedec_cmd(s, rank0, ch, NORMALOP_CMD, 1 << 12);

		mchbar_setbits8(0x5d8 + 0x400 * ch, 0x0e);
		mchbar_clrsetbits16(0x5c4 + 0x400 * ch, 0x3fff, 0x1807);
		mchbar_clrbits8(0x265 + 0x400 * ch, 0x1f);

		/* Disable write level mode for all ranks */
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, ch, rank0)
			set_rank_write_level(s, ch, config, rank0, rank0, 0);
	}

	mchbar_setbits8(0x5dc, 1 << 7);

	/* Increment DQ (rx) dll setting by a standard amount past DQS,
	   This is further trained in write training. */
	switch (s->selected_timings.mem_clk) {
	default:
	case MEM_CLOCK_800MHz:
		count = 39;
		break;
	case MEM_CLOCK_1066MHz:
		count = 32;
		break;
	case MEM_CLOCK_1333MHz:
		count = 42;
		break;
	}

	FOR_EACH_POPULATED_CHANNEL_AND_BYTELANE(s->dimms, ch, lane) {
		dq_setting = s->dqs_settings[ch][lane];
		for (i = 0; i < count; i++)
			if (increment_dq_dqs(s, &dq_setting))
				die("Can't further increase DQ past DQS delay");
		dqset(ch, lane, &dq_setting);
	}

	printk(BIOS_DEBUG, "Done write levelling.\n");
}
