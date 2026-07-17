/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"
#include "ranges.h"

/* LCT PI range: 2 QCLK = 1 DCLK */
#define LCT_MIN_PI	0
#define LCT_MAX_PI	127
#define LCT_PI_RANGE	(1 + LCT_MAX_PI - LCT_MIN_PI)

#define LCT_MIN_WIDTH	18

#define LCT_PLOT	RAM_DEBUG

struct lct_timing {
	uint8_t t[NUM_CHANNELS];
};

#define LCT_TIMING(val)	((struct lct_timing) {.t = { val, val }})

static const char *const ca_names[MAX_CT_ITERATION] = {
	[CT_ITERATION_CLOCK]     = "CLK",
	[CT_ITERATION_CMD_NORTH] = "CMD North",
	[CT_ITERATION_CMD_SOUTH] = "CMD South",
	[CT_ITERATION_CKE]       = "CKE",
	[CT_ITERATION_CTL]       = "CTL",
	[CT_ITERATION_CA_VREF]   = "CA Vref",
};

static void shift_dq_pis(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rank,
	const uint32_t byte_mask,
	const int8_t offset,
	const bool update_ctrl)
{
	for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
		if (!(BIT(byte) & byte_mask))
			continue;

		/* TODO: We should find DQ PI limits for everything */
		int16_t new_rcven = ctrl->rcven[channel][rank][byte] + offset;
		if (new_rcven < 0) {
			printk(BIOS_ERR, "%s: RcvEn PI underflow!\n", __func__);
			new_rcven = 0;
		} else if (new_rcven > 511) {
			printk(BIOS_ERR, "%s: RcvEn PI overflow!\n", __func__);
			new_rcven = 511;
		}
		update_rxt(ctrl, channel, rank, byte, RXT_RCVEN, new_rcven);
		update_txt(ctrl, channel, rank, byte, TXT_DQDQS_OFF, offset);
		if (update_ctrl) {
			ctrl->rcven[channel][rank][byte] = new_rcven;
			ctrl->txdqs[channel][rank][byte] += offset;
			ctrl->tx_dq[channel][rank][byte] += offset;
		}
	}
}

static uint8_t offset_clk(uint8_t pi_code, int32_t offset)
{
	int32_t new_value = (pi_code + offset) % LCT_PI_RANGE;
	if (new_value < 0)
		new_value = LCT_PI_RANGE - ABS(new_value);

	return new_value % LCT_PI_RANGE;
}

static void shift_clk_pi_lpddr(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t groupmask,
	const int32_t offset,
	const bool update_ctrl)
{
	const uint8_t *bytemask = ctrl->dq_byte_map[channel][CT_ITERATION_CLOCK];

	/* LPDDR clocks are per group, not per rank */
	struct clk_pi_code clk = ctrl->ca[channel].clk;
	for (uint8_t group = 0; group < NUM_GROUPS; group++) {
		if (!(groupmask & BIT(group)))
			continue;

		const uint8_t new_pi = offset_clk(ctrl->ca[channel].clk.pi[group], offset);
		clk.pi[group] = new_pi;
		if (update_ctrl)
			ctrl->ca[channel].clk.pi[group] = new_pi;

		/*
		 * Each clock spans all ranks, so need to shift DQ PIs
		 * on all ranks, but only for bytes in this group.
		 */
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask))
				continue;

			shift_dq_pis(ctrl, channel, rank, bytemask[group], offset, update_ctrl);
		}
	}
	mchbar_write32(DDR_CLK_ch_PI_CODING(channel), encode_clk_pi(clk));
}

static void shift_clk_pi_ddr(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const int32_t offset,
	const bool update_ctrl)
{
	struct clk_pi_code clk = ctrl->ca[channel].clk;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_mask(rank, rankmask))
			continue;

		const uint8_t new_pi = offset_clk(ctrl->ca[channel].clk.pi[rank], offset);
		clk.pi[rank] = new_pi;
		if (update_ctrl)
			ctrl->ca[channel].clk.pi[rank] = new_pi;

		/* Shift DQ PI on all bytes by default on DDR3 */
		shift_dq_pis(ctrl, channel, rank, BIT(ctrl->lanes) - 1, offset, update_ctrl);
	}
	mchbar_write32(DDR_CLK_ch_PI_CODING(channel), encode_clk_pi(clk));
}

static void shift_clk_pi(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t groupmask,
	const int32_t new_val_or_off,
	const bool update_ctrl)
{
	const int32_t offset = new_val_or_off;
	if (ctrl->lpddr)
		shift_clk_pi_lpddr(ctrl, channel, rankmask, groupmask, offset, update_ctrl);
	else
		shift_clk_pi_ddr(ctrl, channel, rankmask, offset, update_ctrl);
}

static uint8_t get_cmd_groupmask(struct sysinfo *ctrl, const uint8_t groupmask)
{
	/* On ULT with LPDDR, the two CMD PIs control separate, per-group things */
	if (ctrl->lpddr)
		return groupmask;

	/*
	 * On ULT with DDR, both CMD PIs should have the same value.
	 * On Trad (DDR only), no harm in setting both PIs to the same value.
	 */
	return groupmask ? 3 : 0;
}

static void shift_cmd_n_pi(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t in_groupmask,
	const int32_t new_val_or_off,
	const bool update_ctrl)
{
	const uint8_t new_pi = clamp_s32(LCT_MIN_PI, new_val_or_off, LCT_MAX_PI);

	/* LPDDR: CMD_N.CmdPi1Code controls CAB, (CMD_N.CmdPi0Code seems unused) */
	const uint8_t groupmask = get_cmd_groupmask(ctrl, in_groupmask);
	const struct cmd_pi_code cmd_n = {
		.pi[0] = groupmask & BIT(0) ? new_pi : ctrl->ca[channel].cmd_n.pi[0],
		.pi[1] = groupmask & BIT(1) ? new_pi : ctrl->ca[channel].cmd_n.pi[1],
	};
	mchbar_write32(DDR_CMD_N_ch_PI_CODING(channel), encode_cmd_pi(cmd_n));
	if (update_ctrl)
		ctrl->ca[channel].cmd_n = cmd_n;
}

static void shift_cmd_s_pi(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t in_groupmask,
	const int32_t new_val_or_off,
	const bool update_ctrl)
{
	const uint8_t new_pi = clamp_s32(LCT_MIN_PI, new_val_or_off, LCT_MAX_PI);

	/* LPDDR: CMD_S.CmdPi0Code controls CAA, CMD_S.CmdPi1Code controls CAB */
	const uint8_t groupmask = get_cmd_groupmask(ctrl, in_groupmask);
	struct cmd_pi_code cmd_s = {
		.pi[0] = groupmask & BIT(0) ? new_pi : ctrl->ca[channel].cmd_s.pi[0],
		.pi[1] = groupmask & BIT(1) ? new_pi : ctrl->ca[channel].cmd_s.pi[1],
	};

	/* Program CMD PI codes in CMD South FUB */
	const uint32_t cmd_s_cr = encode_cmd_pi(cmd_s);
	mchbar_write32(DDR_CMD_S_ch_PI_CODING(channel), cmd_s_cr);
	if (update_ctrl)
		ctrl->ca[channel].cmd_s = cmd_s;

	/* On DDR3, also program CKE FUB with the same values */
	if (!ctrl->lpddr) {
		mchbar_write32(DDR_CKE_ch_CMD_PI_CODING(channel), cmd_s_cr);
		if (update_ctrl)
			ctrl->ca[channel].cke_cmd = cmd_s;
	}
}

/* NOTE: Unused for DDR3, as CKE CMD is programmed along with CMD South (see above) */
static void shift_cke_pi(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t in_groupmask,
	const int32_t new_val_or_off,
	const bool update_ctrl)
{
	const uint8_t new_pi = clamp_s32(LCT_MIN_PI, new_val_or_off, LCT_MAX_PI);

	/* LPDDR: CKE.CmdPi0Code controls CAA, CKE.CmdPi1Code seems unused */
	const uint8_t groupmask = get_cmd_groupmask(ctrl, in_groupmask);
	const struct cmd_pi_code cke_cmd = {
		.pi[0] = groupmask & BIT(0) ? new_pi : ctrl->ca[channel].cke_cmd.pi[0],
		.pi[1] = groupmask & BIT(1) ? new_pi : ctrl->ca[channel].cke_cmd.pi[1],
	};
	mchbar_write32(DDR_CKE_ch_CMD_PI_CODING(channel), encode_cmd_pi(cke_cmd));
	if (update_ctrl)
		ctrl->ca[channel].cke_cmd = cke_cmd;
}

static void shift_ctl_pi(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t groupmask,
	const int32_t new_val_or_off,
	const bool update_ctrl)
{
	const uint8_t new_pi = clamp_s32(LCT_MIN_PI, new_val_or_off, LCT_MAX_PI);

	/* CS/ODT PI in CTL FUB */
	struct ctl_pi_code ctl = ctrl->ca[channel].ctl;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_mask(rank, rankmask))
			continue;

		ctl.pi[rank] = new_pi;
		if (update_ctrl) {
			ctrl->ca[channel].ctl.pi[rank] = new_pi;
			ctrl->ca[channel].cke.pi[rank] = new_pi;
		}
	}
	if (ctrl->lpddr && ctrl->lpddr_dram_odt) {
		/* ODT[0] (equal to CS[0] PI setting) goes into PI code 2 */
		ctl.pi[2] = ctrl->ca[channel].ctl.pi[0];
	}
	mchbar_write32(DDR_CTL_ch_CTL_PI_CODING(channel), encode_ctl_pi(ctl));

	/* CTL PI in CKE FUB */
	struct ctl_pi_code cke_ctl = {};
	if (ctrl->lpddr) {
		/* Use CKE-to-rank mapping for LPDDR */
		const uint8_t cke_rank_map = ctrl->lpddr_cke_rank_map[channel];
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			/* HACK: ULT has at most 2 ranks per channel */
			if (rank >= 2)
				break;

			for (uint8_t cke = 0; cke < 4; cke++) {
				if (((cke_rank_map >> cke) & 1) != rank)
					continue;

				/* This CKE pin is connected to this rank */
				cke_ctl.pi[cke] = ctrl->ca[channel].cke.pi[rank];
			}
		}
	} else {
		/* On DDR, use 1:1 mapping */
		cke_ctl = ctl;
	}

	/*
	 * Put the average of CKE2 and CKE3 into CKE2 PI setting.
	 * On DDR3, we have to do CTL centering for ranks 2 and 3
	 * at the same time, since PI settings in the CTL and CKE
	 * FUBs should be the same.
	 */
	cke_ctl.pi[2] = (cke_ctl.pi[2] + cke_ctl.pi[3]) / 2;

	/* PI code 3 field is not implemented in the CKE FUB */
	cke_ctl.pi[3] = 0;
	mchbar_write32(DDR_CKE_ch_CTL_PI_CODING(channel), encode_ctl_pi(cke_ctl));
}

typedef void (*shift_func_t)(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t in_groupmask,
	const int32_t new_val_or_off,
	const bool update_ctrl);

/* NOTE: rankmask is ignored for CMD timings (CMD_NORTH, CMD_SOUTH, CKE) */
#define SHIFT_ALL_RANKS	0xff

/*
 * CMD PI mapping for LPDDR is as follows:
 *   - CMD_S[0] ---> CAA[0,1,2,3,4]
 *   - CKE[0]   ---> CAA[5,6,7,8,9]
 *   - CMD_N[1] ---> CAB[0,1,2,3,4,6,7,9]
 *   - CMD_S[1] ---> CAB[5,8]
 */
void shift_pi_for_cmd_training(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const enum lct_iteration iteration,
	const uint8_t rankmask_in,
	const uint8_t groupmask,
	const int32_t new_val_or_off, /* offset for CLK, new value for others */
	const bool update_ctrl)
{
	static const shift_func_t func_table[] = {
		[CT_ITERATION_CLOCK]     = shift_clk_pi,
		[CT_ITERATION_CMD_NORTH] = shift_cmd_n_pi,
		[CT_ITERATION_CMD_SOUTH] = shift_cmd_s_pi,
		[CT_ITERATION_CKE]       = shift_cke_pi,
		[CT_ITERATION_CTL]       = shift_ctl_pi,
	};

	if (iteration != CT_ITERATION_CLOCK)
		assert(new_val_or_off >= LCT_MIN_PI && new_val_or_off <= LCT_MAX_PI);

	if (iteration >= ARRAY_SIZE(func_table))
		die("%s: LCT parameter %u out of range\n", __func__, iteration);

	/* The rankmask may be zero if we want to restore PI settings from ctrl */
	const uint8_t rankmask = rankmask_in & ctrl->rankmap[channel];
	func_table[iteration](ctrl, channel, rankmask, groupmask, new_val_or_off, update_ctrl);
}

static uint8_t run_io_test_lct(struct sysinfo *ctrl, uint8_t chanmask, bool skip_vref)
{
	if (skip_vref)
		return run_io_test(ctrl, chanmask, ctrl->dq_pat, 1);

	/* Run REUT until both channels fail or we finish all Vref points */
	const int8_t lct_vref_offsets[] = { -8, 8 };
	uint8_t ch_error = 0;
	for (size_t v = 0; v < ARRAY_SIZE(lct_vref_offsets); v++) {
		update_vref_and_wait(ctrl, VREF_CA, false, lct_vref_offsets[v], false);
		ch_error |= run_io_test(ctrl, chanmask, ctrl->dq_pat, 1);
		if (ch_error == chanmask)
			break;
	}
	return ch_error;
}

enum raminit_status cmd_find_edges_linear(
	struct sysinfo *ctrl,
	const enum lct_iteration iteration,
	const uint8_t chanmask,
	const uint8_t rankmask,
	const uint8_t groupmask,
	const int32_t lct_start,
	const int32_t lct_stop,
	const int32_t lct_step,
	const bool skip_vref,
	const bool skip_print,
	const bool update_ctrl)
{
	/* Clock timings are allowed to wrap around */
	const bool wrap_allowed = iteration == CT_ITERATION_CLOCK;

	/* Clock timing works with offsets, which we need to cancel manually when saving */

	if (!skip_print)
		printk(LCT_PLOT, "Channel\t\t\t0 1\nClkDly ");

	struct phase_train_data region_data[NUM_CHANNELS] = {};
	for (int32_t lct_delay = lct_start; lct_delay <= lct_stop; lct_delay += lct_step) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(BIT(channel) & chanmask))
				continue;

			shift_pi_for_cmd_training(
				ctrl,
				channel,
				iteration,
				rankmask,
				groupmask,
				lct_delay,
				false);
		}
		do_jedec_init(ctrl);

		const uint8_t ch_error = run_io_test_lct(ctrl, chanmask, skip_vref);
		if (!skip_print)
			printk(LCT_PLOT, "\n %d\t\t\t", lct_delay);

		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(BIT(channel) & chanmask)) {
				if (!skip_print)
					printk(LCT_PLOT, "  ");

				continue;
			}
			const bool pass = !(ch_error & BIT(channel));
			if (!skip_print)
				printk(LCT_PLOT, pass ? ". " : "# ");

			phase_record_pass(
				&region_data[channel],
				pass,
				lct_delay,
				lct_start,
				lct_step);
		}
	}
	if (!skip_print)
		printk(BIOS_DEBUG, "\n\nCh\tLeft\tRight\tWidth\tCenter\n");

	enum raminit_status status = 0;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!(BIT(channel) & chanmask))
			continue;

		struct phase_train_data *const curr_data = &region_data[channel];
		if (wrap_allowed)
			phase_append_initial_to_current(curr_data, lct_start, lct_step);

		int32_t center = range_center(curr_data->largest);
		const int32_t lwidth = range_width(curr_data->largest);
		const bool bad_eye = lwidth < 3 * lct_step || lwidth >= lct_stop - lct_start;
		if (bad_eye) {
			/* TODO: Have a default center parameter instead? */
			center = (lct_start + lct_stop) / 2;
			status = RAMINIT_STATUS_LCT_FAILURE;
		}

		/*
		 * Save new center, or restore value from ctrl. Can't update
		 * saved clock timing directly as DQ PIs need to be shifted.
		 */
		shift_pi_for_cmd_training(
			ctrl,
			channel,
			iteration,
			update_ctrl ? rankmask : 0,
			update_ctrl ? groupmask : 0,
			center,
			update_ctrl);

		if (!skip_print) {
			printk(BIOS_DEBUG, " %u\t%d\t%d\t%d\t%d%s\n",
				channel,
				curr_data->largest.start,
				curr_data->largest.end,
				lwidth,
				center,
				bad_eye ? "\t[FAIL]" : "");
		}

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask))
				continue;

			struct last_margin *margin = &ctrl->results[LastCmdT][rank][channel][0];
			if (iteration == CT_ITERATION_CLOCK) {
				margin->start = 10 * ABS(curr_data->largest.start);
				margin->end = 10 * ABS(curr_data->largest.end);
			}
		}
	}
	if (!skip_print) {
		printk(LCT_PLOT, "\n");
		if (status)
			printk(BIOS_ERR, "Bad %s eye in linear search\n", ca_names[iteration]);
	}

	/* Clean up after test */
	if (!skip_vref)
		update_vref_and_wait(ctrl, VREF_CA, false, 0, false);

	const enum raminit_status last_status = do_jedec_init(ctrl);
	return status ? status : last_status;
}

static struct lct_timing cmd_find_edge_binary(
	struct sysinfo *ctrl,
	const enum lct_iteration iteration,
	const uint8_t chanmask,
	const uint8_t rankmask,
	const uint8_t groupmask,
	struct lct_timing lower,
	struct lct_timing upper,
	const bool count_up)
{
	const bool skip_vref = false;

	printk(LCT_PLOT, "CmdTgt (%s)\n", count_up ? "up" : "down");
	printk(LCT_PLOT, "Ch0G0\tCh0G1\tCh1G0\tCh1G1\t0 1\n");
	while (true) {
		struct lct_timing target = {};
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(BIT(channel) & chanmask)) {
				printk(LCT_PLOT, "\t\t");
				continue;
			}

			/* count_up gets rounding correct */
			target.t[channel] = upper.t[channel] + lower.t[channel] + count_up;
			target.t[channel] /= 2;

			for (uint8_t group = 0; group < NUM_GROUPS; group++) {
				if (!(BIT(group) & groupmask)) {
					printk(LCT_PLOT, "\t");
					continue;
				}
				printk(LCT_PLOT, "%u\t", target.t[channel]);
				shift_pi_for_cmd_training(
					ctrl,
					channel,
					iteration,
					rankmask,
					BIT(group),
					target.t[channel],
					false);
			}
		}
		do_jedec_init(ctrl);

		const uint8_t ch_error = run_io_test_lct(ctrl, chanmask, skip_vref);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(BIT(channel) & chanmask)) {
				printk(LCT_PLOT, "  ");
				continue;
			}
			const bool fail = ch_error & BIT(channel);
			printk(LCT_PLOT, fail ? "# " : ". ");

			/* Skip if this channel is done */
			if (upper.t[channel] <= lower.t[channel])
				continue;

			if (fail == count_up)
				upper.t[channel] = target.t[channel] - count_up;
			else
				lower.t[channel] = target.t[channel] + !count_up;
		}
		printk(LCT_PLOT, "\n");
		bool done = true;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if ((BIT(channel) & chanmask) && upper.t[channel] > lower.t[channel]) {
				done = false;
				break;
			}
		}
		if (done)
			break;
	}
	printk(LCT_PLOT, "\n");

	/* Clean up after test */
	if (!skip_vref)
		update_vref_and_wait(ctrl, VREF_CA, false, 0, false);

	do_jedec_init(ctrl);
	return count_up ? lower : upper;
}

/* Note: midpoint is only used for non-CLK iterations */
static enum raminit_status center_ca_timing(
	struct sysinfo *ctrl,
	const enum lct_iteration iteration,
	const uint8_t rankmask,
	const uint8_t groupmask,
	const struct lct_timing midpoint)
{
	printk(BIOS_DEBUG, "\n*** Centering %s timing, rankmask = 0x%x ***\n",
		ca_names[iteration], rankmask);

	if (iteration == CT_ITERATION_CA_VREF)
		die("%s: CA Vref is not a timing (bad usage)\n", __func__);

	uint8_t chanmask = 0;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
		chanmask |= select_reut_ranks(ctrl, channel, rankmask);

	if (!chanmask)
		return RAMINIT_STATUS_SUCCESS;

	if (iteration == CT_ITERATION_CLOCK) {
		/* Use a linear search to center clock timings */

		const int32_t lct_start = LCT_MIN_PI;
		const int32_t lct_stop = LCT_MAX_PI;
		const int32_t lct_step = ctrl->lpddr ? 2 : 6;
		const bool skip_vref = false;
		const bool skip_print = false;
		const bool update_ctrl = true;
		return cmd_find_edges_linear(
			ctrl,
			iteration,
			chanmask,
			rankmask,
			groupmask,
			lct_start,
			lct_stop,
			lct_step,
			skip_vref,
			skip_print,
			update_ctrl);
	}

	/* Binary search will use the full PI range */
	struct lct_timing lower = LCT_TIMING(LCT_MIN_PI);
	struct lct_timing upper = LCT_TIMING(LCT_MAX_PI);
	if (ctrl->lpddr && iteration != CT_ITERATION_CLOCK) {
		/*
		 * Limit the binary search to +/- 32 PI ticks from
		 * the ECT midpoint, for LPDDR3 command/control.
		 */
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			const uint8_t mid_value = midpoint.t[channel];
			lower.t[channel] = MAX(mid_value - 32, LCT_MIN_PI);
			upper.t[channel] = MIN(mid_value + 32, LCT_MAX_PI);
			printk(BIOS_DEBUG, "Ch%u: search range is [%u..%u]\n",
				channel, lower.t[channel], upper.t[channel]);
		}
	}

	const struct lct_timing left_edge = cmd_find_edge_binary(
		ctrl,
		iteration,
		chanmask,
		rankmask,
		groupmask,
		lower,
		midpoint,
		false);

	const struct lct_timing right_edge = cmd_find_edge_binary(
		ctrl,
		iteration,
		chanmask,
		rankmask,
		groupmask,
		midpoint,
		upper,
		true);

	printk(BIOS_DEBUG, "Ch\tLeft\tRight\tWidth\tCenter\n");
	enum raminit_status status = 0;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!(BIT(channel) & chanmask))
			continue;

		const uint8_t l_edge = left_edge.t[channel];
		const uint8_t r_edge = right_edge.t[channel];
		const uint8_t lwidth = r_edge - l_edge;
		uint8_t center;
		const char *msg = "";
		if (l_edge == LCT_MIN_PI && r_edge == LCT_MAX_PI) {
			/* No errors found in this channel? Not supposed to happen */
			center = midpoint.t[channel];
			msg = "\t[No errors?]";
		} else {
			center = DIV_ROUND_CLOSEST(l_edge + r_edge, 2);
			if (lwidth < LCT_MIN_WIDTH) {
				status = RAMINIT_STATUS_LCT_FAILURE;
				msg = "\t[FAIL]";
			}
		}

		shift_pi_for_cmd_training(
			ctrl,
			channel,
			iteration,
			rankmask,
			groupmask,
			center,
			true);

		printk(BIOS_DEBUG, " %u\t%u\t%u\t%u\t%u%s\n",
			channel, l_edge, r_edge, lwidth, center, msg);
	}
	printk(LCT_PLOT, "\n");
	if (status)
		printk(BIOS_ERR, "Bad %s eye in binary search\n", ca_names[iteration]);

	return status;
}

static void set_channel_timing(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t ctl_pi)
{
	const uint8_t groupmask = 3;
	const bool update_ctrl = true;

	/*
	 * In 2N mode, offset CMD PI w.r.t. CTL PI for better margins.
	 *
	 * TODO: Figure out why the heck this is the case. And why 85?
	 */
	const uint8_t cmd_pi = ctl_pi + (ctrl->tCMD > 1 ? 85 - 64 : 0);

	shift_pi_for_cmd_training(
		ctrl,
		channel,
		CT_ITERATION_CMD_SOUTH,
		SHIFT_ALL_RANKS,
		groupmask,
		cmd_pi,
		update_ctrl);

	shift_pi_for_cmd_training(
		ctrl,
		channel,
		CT_ITERATION_CMD_NORTH,
		SHIFT_ALL_RANKS,
		groupmask,
		cmd_pi,
		update_ctrl);

	shift_pi_for_cmd_training(
		ctrl,
		channel,
		CT_ITERATION_CTL,
		SHIFT_ALL_RANKS,
		groupmask,
		ctl_pi,
		update_ctrl);
}

/* This algorithm is backported from SKL */
static enum raminit_status center_ctl_clk_2d(
	struct sysinfo *ctrl,
	const uint8_t rankmask,
	const uint8_t groupmask,
	struct lct_timing *best_pi)
{
	uint8_t chanmask = 0;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
		chanmask |= select_reut_ranks(ctrl, channel, rankmask);

	if (!chanmask)
		return RAMINIT_STATUS_SUCCESS;

	uint8_t first_rank[NUM_CHANNELS] = {};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (rank_in_ch(ctrl, rank, channel)) {
				first_rank[channel] = rank;
				break;
			}
		}
	}

	/* Keep 32 PI ticks on each side for later margining steps */
	const uint8_t pi_reserve = 32;
	const uint8_t ctl_start = LCT_MIN_PI + pi_reserve;
	const uint8_t ctl_stop = LCT_MAX_PI + 1 - pi_reserve;
	const uint8_t ctl_step = 8;

	bool any_pass = false;
	struct {
		uint8_t lower;
		uint8_t upper;
		uint8_t width;
	} best[NUM_CHANNELS] = {};

	printk(BIOS_DEBUG, "\n*** 2D sweep command/control and clock timings ***\n");
	printk(LCT_PLOT, "CtlPi\tCh0\tCh1\n");
	for (uint8_t ctl_pi = ctl_start; ctl_pi <= ctl_stop; ctl_pi += ctl_step) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			set_channel_timing(ctrl, channel, ctl_pi);
		}

		/*
		 * Changing Vref is slow, and it would actually turn this
		 * into an extremely slow 3D algorithm for no good reason
		 */
		const int32_t clk_start = LCT_MIN_PI;
		const int32_t clk_stop = LCT_MAX_PI;
		const int32_t clk_step = 6;
		const bool skip_vref = true;
		const bool skip_print = true;
		const bool update_ctrl = false;
		const enum raminit_status status = cmd_find_edges_linear(
			ctrl,
			CT_ITERATION_CLOCK,
			chanmask,
			rankmask,
			groupmask,
			clk_start,
			clk_stop,
			clk_step,
			skip_vref,
			skip_print,
			update_ctrl);

		if (!status)
			any_pass = true;

		printk(LCT_PLOT, "%u", ctl_pi);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel)) {
				printk(LCT_PLOT, "\t");
				continue;
			}

			const uint8_t rank = first_rank[channel];
			struct last_margin *margin = &ctrl->results[LastCmdT][rank][channel][0];
			const uint8_t lwidth = margin->end / 10 - margin->start / 10;
			printk(LCT_PLOT, "\t%u", lwidth);
			if (lwidth > best[channel].width) {
				/* Found a new best margin */
				best[channel].width = lwidth;
				best[channel].lower = ctl_pi;
				best[channel].upper = ctl_pi;
			} else if (lwidth == best[channel].width) {
				/* Track the last PI offset which still provides max margin */
				best[channel].upper = ctl_pi;
			}
		}
		printk(LCT_PLOT, "\n");
	}
	printk(BIOS_DEBUG, "\nFinal command/control results:\n");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		best_pi->t[channel] = (best[channel].lower + best[channel].upper) / 2;
		printk(BIOS_DEBUG, "  Channel %u: %u\n", channel, best_pi->t[channel]);

		const uint8_t final_pi = best_pi->t[channel];
		set_channel_timing(ctrl, channel, final_pi);
	}
	printk(BIOS_DEBUG, "\n");
	if (!any_pass) {
		printk(BIOS_ERR, "No good eye found in 2D clock/control sweep\n");
		return RAMINIT_STATUS_LCT_FAILURE;
	}
	return RAMINIT_STATUS_SUCCESS;
}

/* Caller must ensure the offset does not saturate any of the PIs */
static void shift_channel_timing(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const int32_t offset,
	const bool update_ctrl)
{
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		printk(BIOS_DEBUG, "  Rank %u:\n", rank);

		/* CLK is per rank in DDR3 */
		if (!ctrl->lpddr) {
			const int32_t new_clk = ctrl->ca[channel].clk.pi[rank] + offset;
			shift_pi_for_cmd_training(
				ctrl,
				channel,
				CT_ITERATION_CLOCK,
				BIT(rank),
				BIT(0),
				offset,
				update_ctrl);
			printk(BIOS_DEBUG, "    New CLK: %d\n", new_clk);
		}

		const int32_t new_ctl = ctrl->ca[channel].ctl.pi[rank] + offset;
		shift_pi_for_cmd_training(
			ctrl,
			channel,
			CT_ITERATION_CTL,
			BIT(rank),
			BIT(0),
			new_ctl,
			update_ctrl);
		printk(BIOS_DEBUG, "    New CTL: %d\n", new_ctl);
	}

	const int32_t new_cmd_s0 = ctrl->ca[channel].cmd_s.pi[0] + offset;
	shift_pi_for_cmd_training(
		ctrl,
		channel,
		CT_ITERATION_CMD_SOUTH,
		SHIFT_ALL_RANKS,
		BIT(0),
		new_cmd_s0,
		update_ctrl);
	printk(BIOS_DEBUG, "  New CMD_S[0]: %d\n", new_cmd_s0);

	if (ctrl->lpddr) {
		/* Clock is per group in LPDDR */
		for (uint8_t group = 0; group < NUM_GROUPS; group++) {
			if (!ctrl->dq_byte_map[channel][CT_ITERATION_CLOCK][group])
				continue;

			shift_pi_for_cmd_training(
				ctrl,
				channel,
				CT_ITERATION_CLOCK,
				0, /* TODO: Why are you zero? */
				BIT(group),
				offset,
				update_ctrl);
		}

		const int32_t new_cke_cmd = ctrl->ca[channel].cke_cmd.pi[0] + offset;
		shift_pi_for_cmd_training(
			ctrl,
			channel,
			CT_ITERATION_CKE,
			SHIFT_ALL_RANKS,
			BIT(0),
			new_cke_cmd,
			update_ctrl);
		printk(BIOS_DEBUG, "  New CKE_CMD[0]: %d\n", new_cke_cmd);

		const int32_t new_cmd_s1 = ctrl->ca[channel].cmd_s.pi[1] + offset;
		shift_pi_for_cmd_training(
			ctrl,
			channel,
			CT_ITERATION_CMD_SOUTH,
			SHIFT_ALL_RANKS,
			BIT(1),
			new_cmd_s1,
			update_ctrl);
		printk(BIOS_DEBUG, "  New CMD_S[1]: %d\n", new_cmd_s1);

		const int32_t new_cmd_n1 = ctrl->ca[channel].cmd_n.pi[1] + offset;
		shift_pi_for_cmd_training(
			ctrl,
			channel,
			CT_ITERATION_CMD_NORTH,
			SHIFT_ALL_RANKS,
			BIT(1),
			new_cmd_n1,
			update_ctrl);
		printk(BIOS_DEBUG, "  New CMD_N[1]: %d\n", new_cmd_n1);
	} else {
		/* DDR3 */
		const int32_t new_cmd_n0 = ctrl->ca[channel].cmd_n.pi[0] + offset;
		shift_pi_for_cmd_training(
			ctrl,
			channel,
			CT_ITERATION_CMD_NORTH,
			SHIFT_ALL_RANKS,
			BIT(0),
			new_cmd_n0,
			update_ctrl);
		printk(BIOS_DEBUG, "  New CMD_N[0]: %d\n", new_cmd_n0);
	}
	printk(BIOS_DEBUG, "\n");
}

static uint8_t get_min_ca_pi_code(struct sysinfo *ctrl, const uint8_t channel)
{
	/*
	 * Find the minimum PI code across all relevant CMD and CTL FUBs.
	 * Some of the PI code values in the FUBs are only used depending
	 * on the type of memory (DDR or LPDDR).
	 */
	uint8_t min_code = UINT8_MAX;
	min_code = MIN(min_code, ctrl->ca[channel].cke_cmd.pi[0]);
	min_code = MIN(min_code, ctrl->ca[channel].cmd_s.pi[0]);
	if (ctrl->lpddr) {
		min_code = MIN(min_code, ctrl->ca[channel].cmd_s.pi[1]);
		min_code = MIN(min_code, ctrl->ca[channel].cmd_n.pi[1]);
	} else {
		min_code = MIN(min_code, ctrl->ca[channel].cmd_n.pi[0]);
	}
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		min_code = MIN(min_code, ctrl->ca[channel].cke.pi[rank]);
		min_code = MIN(min_code, ctrl->ca[channel].ctl.pi[rank]);
	}
	return min_code;
}

enum raminit_status train_late_command(struct sysinfo *ctrl)
{
	if (ctrl->lpddr)
		die("%s: LPDDR is not yet supported\n", __func__);

	/* Selecting the REUT ranks is done within the centering functions */
	setup_io_test_cadb(ctrl, ctrl->chanmap, 10, NTHSOE);

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		const uint8_t groupmask = 1;
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			/*
			 * Reset clock PI to zero. We have to do this one rank
			 * at a time since we also need to shift the DQ PIs so
			 * that things remain in sync. MRC shifts all ranks at
			 * once, using the clock PI from rank 0. While it also
			 * works, clock PIs on the other ranks can end up with
			 * an offset, which could cause discontinuities during
			 * margining (at the point where the PI wraps around).
			 *
			 * In any case, things break without this. So keep it.
			 */
			shift_pi_for_cmd_training(
				ctrl,
				channel,
				CT_ITERATION_CLOCK,
				BIT(rank),
				groupmask,
				0 - ctrl->ca[channel].clk.pi[rank],
				true);
		}
	}

	const uint8_t rankmask = ctrl->rankmap[0] | ctrl->rankmap[1];
	printk(BIOS_DEBUG, "\nLCT: Current tCMD = %uT\n", ctrl->tCMD);

	/* Do a 2D search to find best command/control PI, to maximise clock margin */
	struct lct_timing best_pi = {};
	enum raminit_status status = center_ctl_clk_2d(ctrl, rankmask, 1, &best_pi);
	if (status)
		return status;

	/* Center clock timing in the global eye (midpoint is N/A) */
	status = center_ca_timing(ctrl, CT_ITERATION_CLOCK, rankmask, 1, best_pi);
	if (status)
		return status;

	/* For DDR3, this also centers the CMD PI in the CKE FUB */
	status = center_ca_timing(ctrl, CT_ITERATION_CMD_SOUTH, rankmask, 1, best_pi);
	if (status)
		return status;

	status = center_ca_timing(ctrl, CT_ITERATION_CMD_NORTH, rankmask, 1, best_pi);
	if (status)
		return status;

	/* For control pins, CKE PI is shared between rank 2 and 3 */
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS - 1; rank++) {
		uint8_t ranks = BIT(rank);
		if (rank == 2)
			ranks |= BIT(rank + 1);

		ranks &= rankmask;
		if (!ranks)
			continue;

		status = center_ca_timing(ctrl, CT_ITERATION_CTL, ranks, 1, best_pi);
		if (status)
			return status;
	}

	/* Normalise timings back to 0 to improve performance */
	printk(BIOS_DEBUG, "\n*** Normalise timings back to 0 ***\n");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		const uint8_t min_code = get_min_ca_pi_code(ctrl, channel);
		printk(BIOS_DEBUG, "Channel %u: min PI = %u\n", channel, min_code);
		shift_channel_timing(ctrl, channel, -min_code, true);
	}

	/* Clean up after testing */
	reut_disable_cadb_deselects();
	return do_jedec_init(ctrl);
}
