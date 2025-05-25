/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"
#include "ranges.h"

#define RCVEN_PLOT	RAM_DEBUG

static enum raminit_status change_rcven_timing(struct sysinfo *ctrl, const uint8_t channel)
{
	int16_t max_rcven = -4096;
	int16_t min_rcven = 4096;
	int16_t max_rcven_rank[NUM_SLOTRANKS];
	int16_t min_rcven_rank[NUM_SLOTRANKS];
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		max_rcven_rank[rank] = max_rcven;
		min_rcven_rank[rank] = min_rcven;
	}
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			int16_t new_rcven = ctrl->rcven[channel][rank][byte];
			new_rcven -= ctrl->io_latency[channel][rank] * 64;
			if (max_rcven_rank[rank] < new_rcven)
				max_rcven_rank[rank] = new_rcven;

			if (min_rcven_rank[rank] > new_rcven)
				min_rcven_rank[rank] = new_rcven;
		}
		if (max_rcven < max_rcven_rank[rank])
			max_rcven = max_rcven_rank[rank];

		if (min_rcven > min_rcven_rank[rank])
			min_rcven = min_rcven_rank[rank];
	}

	/*
	 * Determine how far we are from the ideal center point for RcvEn timing.
	 * (PiIdeal - AveRcvEn) / 64 is the ideal number of cycles we should have
	 * for IO latency. command training will reduce this by 64, so plan for
	 * that now in the ideal value. Round to closest integer.
	 */
	const int16_t rre_pi_ideal = 256 + 64;
	const int16_t pi_reserve = 64;
	const int16_t rcven_center = (max_rcven + min_rcven) / 2;
	const int8_t iolat_target = DIV_ROUND_CLOSEST(rre_pi_ideal - rcven_center, 64);

	int8_t io_g_offset = 0;
	int8_t io_lat[NUM_SLOTRANKS] = { 0 };
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		io_lat[rank] = iolat_target;

		/* Check for RcvEn underflow/overflow */
		const int16_t rcven_lower = 64 * io_lat[rank] + min_rcven_rank[rank];
		if (rcven_lower < pi_reserve)
			io_lat[rank] += DIV_ROUND_UP(pi_reserve - rcven_lower, 64);

		const int16_t rcven_upper = 64 * io_lat[rank] + max_rcven_rank[rank];
		if (rcven_upper > 511 - pi_reserve)
			io_lat[rank] -= DIV_ROUND_UP(rcven_upper - (511 - pi_reserve), 64);

		/* Check for IO latency over/underflow */
		if (io_lat[rank] - io_g_offset > 14)
			io_g_offset = io_lat[rank] - 14;

		if (io_lat[rank] - io_g_offset < 1)
			io_g_offset = io_lat[rank] - 1;

		const int8_t cycle_offset = io_lat[rank] - ctrl->io_latency[channel][rank];
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			ctrl->rcven[channel][rank][byte] += 64 * cycle_offset;
			update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
		}
	}

	/* Calculate new IO comp latency */
	union sc_io_latency_reg sc_io_lat = {
		.raw = mchbar_read32(SC_IO_LATENCY_ch(channel)),
	};

	/* Check if we are underflowing or overflowing this field */
	if (io_g_offset < 0 && sc_io_lat.rt_iocomp < -io_g_offset) {
		printk(BIOS_ERR, "%s: IO COMP underflow\n", __func__);
		printk(BIOS_ERR, "io_g_offset: %d\n", io_g_offset);
		printk(BIOS_ERR, "rt_iocomp: %u\n", sc_io_lat.rt_iocomp);
		return RAMINIT_STATUS_RCVEN_FAILURE;
	}
	if (io_g_offset > 0 && io_g_offset > 0x3f - sc_io_lat.rt_iocomp) {
		printk(BIOS_ERR, "%s: IO COMP overflow\n", __func__);
		printk(BIOS_ERR, "io_g_offset: %d\n", io_g_offset);
		printk(BIOS_ERR, "rt_iocomp: %u\n", sc_io_lat.rt_iocomp);
		return RAMINIT_STATUS_RCVEN_FAILURE;
	}
	sc_io_lat.rt_iocomp += io_g_offset;
	ctrl->rt_io_comp[channel] = sc_io_lat.rt_iocomp;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (ctrl->rankmap[channel] & BIT(rank))
			ctrl->io_latency[channel][rank] = io_lat[rank] - io_g_offset;

		const uint8_t shift = rank * 4;
		sc_io_lat.raw &= ~(0xf << shift);
		sc_io_lat.raw |= ctrl->io_latency[channel][rank] << shift;
	}
	mchbar_write32(SC_IO_LATENCY_ch(channel), sc_io_lat.raw);
	return RAMINIT_STATUS_SUCCESS;
}

#define RL_START (256 + 24)
#define RL_STOP  (384 + 24)
#define RL_STEP  8

#define RE_NUM_SAMPLES	6

static enum raminit_status verify_high_region(const int32_t center, const int32_t lwidth)
{
	if (center > RL_STOP) {
		/* Check if center of high was found where it should be */
		printk(BIOS_ERR, "RcvEn: Center of high (%d) higher than expected\n", center);
		return RAMINIT_STATUS_RCVEN_FAILURE;
	}
	if (lwidth <= 32) {
		/* Check if width is large enough */
		printk(BIOS_ERR, "RcvEn: Width of high region (%d) too small\n", lwidth);
		return RAMINIT_STATUS_RCVEN_FAILURE;
	}
	if (lwidth >= 96) {
		/* Since we're calibrating a phase, a too large region is a problem */
		printk(BIOS_ERR, "RcvEn: Width of high region (%d) too large\n", lwidth);
		return RAMINIT_STATUS_RCVEN_FAILURE;
	}
	return RAMINIT_STATUS_SUCCESS;
}

static void program_io_latency(struct sysinfo *ctrl, const uint8_t channel, const uint8_t rank)
{
	const uint8_t shift = rank * 4;
	const uint8_t iolat = ctrl->io_latency[channel][rank];
	mchbar_clrsetbits32(SC_IO_LATENCY_ch(channel), 0xf << shift, iolat << shift);
}

static void program_rl_delays(struct sysinfo *ctrl, const uint8_t rank, const uint16_t rl_delay)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
			update_rxt(ctrl, channel, rank, byte, RXT_RCVEN, rl_delay);
	}
}

static bool sample_dqs(const uint8_t channel, const uint8_t byte)
{
	return (get_data_train_feedback(channel, byte) & 0x1ff) >= BIT(RE_NUM_SAMPLES - 1);
}

enum raminit_status train_receive_enable(struct sysinfo *ctrl)
{
	const struct reut_box reut_addr = {
		.col = {
			.start    = 0,
			.stop     = 1023,
			.inc_rate = 0,
			.inc_val  = 1,
		},
	};
	const struct wdb_pat wdb_pattern = {
		.start_ptr  = 0,
		.stop_ptr   = 9,
		.inc_rate   = 32,
		.dq_pattern = BASIC_VA,
	};

	const uint16_t bytemask = BIT(ctrl->lanes) - 1;
	const uint8_t fine_step = 1;

	const uint8_t rt_delta = is_hsw_ult() ? 4 : 2;
	const uint8_t rt_io_comp = 21 + rt_delta;
	const uint8_t rt_latency = 16 + rt_delta;
	setup_io_test(
		ctrl,
		ctrl->chanmap,
		PAT_RD,
		2,
		RE_NUM_SAMPLES + 1,
		&reut_addr,
		0,
		&wdb_pattern,
		0,
		8);

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			union ddr_data_control_2_reg data_control_2 = {
				.raw = ctrl->dq_control_2[channel][byte],
			};
			data_control_2.force_rx_on = 1;
			mchbar_write32(DQ_CONTROL_2(channel, byte), data_control_2.raw);
		}
		union ddr_data_control_0_reg data_control_0 = {
			.raw = ctrl->dq_control_0[channel],
		};
		if (ctrl->lpddr) {
			/**
			 * W/A for b4618574 - @todo: remove for HSW ULT C0
			 * Can't have force_odt_on together with leaker, disable LPDDR
			 * mode during this training step. lpddr_mode is restored
			 * at the end of this function from the host structure.
			 */
			data_control_0.lpddr_mode = 0;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		}
		data_control_0.force_odt_on     = 1;
		data_control_0.rl_training_mode = 1;
		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		mchbar_write32(SC_IO_LATENCY_ch(channel), (union sc_io_latency_reg) {
			.rt_iocomp = rt_io_comp,
		}.raw);
	}
	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!does_rank_exist(ctrl, rank))
			continue;

		/*
		 * Set initial roundtrip latency values. Assume -4 QCLK for worst board
		 * layout. This is calculated as HW_ROUNDT_LAT_DEFAULT_VALUE plus:
		 *
		 *   DDR3: Default + (2 * tAA) + 4 QCLK + PI_CLK + N-mode value * 2
		 * LPDDR3: Default + (2 * tAA) + 4 QCLK + PI_CLK + tDQSCK_max
		 *
		 * N-mode is 3 during training mode. Both channels use the same timings.
		 */
		/** TODO: differs for LPDDR **/
		const uint32_t tmp = MAX(ctrl->multiplier, 4) + 5 + 2 * ctrl->tAA;
		const uint32_t initial_rt_latency = MIN(rt_latency + tmp, 0x3f);

		uint8_t chanmask = 0;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			chanmask |= select_reut_ranks(ctrl, channel, BIT(rank));
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			ctrl->io_latency[channel][rank] = 0;
			mchbar_write8(SC_ROUNDT_LAT_ch(channel) + rank, initial_rt_latency);
			ctrl->rt_latency[channel].rank[rank] = initial_rt_latency;
		}

		printk(BIOS_DEBUG, "Rank %u\n", rank);
		printk(BIOS_DEBUG, "Steps 1 and 2: Find middle of high region\n");
		printk(RCVEN_PLOT, "Byte");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(RCVEN_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(RCVEN_PLOT, "%u ", byte);
		}
		printk(RCVEN_PLOT, "\nRcvEn\n");
		struct phase_train_data region_data[NUM_CHANNELS][NUM_LANES] = { 0 };
		for (uint16_t rl_delay = RL_START; rl_delay < RL_STOP; rl_delay += RL_STEP) {
			printk(RCVEN_PLOT, " % 3d", rl_delay);
			program_rl_delays(ctrl, rank, rl_delay);
			run_io_test(ctrl, chanmask, BASIC_VA, true);
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				printk(RCVEN_PLOT, "\t");
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					const bool high = sample_dqs(channel, byte);
					printk(RCVEN_PLOT, high ? ". " : "# ");
					phase_record_pass(
						&region_data[channel][byte],
						high,
						rl_delay,
						RL_START,
						RL_STEP);
				}
			}
			printk(RCVEN_PLOT, "\n");
		}
		printk(RCVEN_PLOT, "\n");
		printk(BIOS_DEBUG, "Update RcvEn timing to be in the center of high region\n");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(BIOS_DEBUG, "C%u.R%u: \tLeft\tRight\tWidth\tCenter\n",
				channel, rank);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				struct phase_train_data *const curr_data =
						&region_data[channel][byte];
				phase_append_current_to_initial(curr_data, RL_START, RL_STEP);
				const int32_t lwidth = range_width(curr_data->largest);
				const int32_t center = range_center(curr_data->largest);
				printk(BIOS_DEBUG, "   B%u: \t%d\t%d\t%d\t%d\n",
					byte,
					curr_data->largest.start,
					curr_data->largest.end,
					lwidth,
					center);

				status = verify_high_region(center, lwidth);
				if (status) {
					printk(BIOS_ERR,
						"RcvEn problems on channel %u, byte %u\n",
						channel, byte);
					goto clean_up;
				}
				ctrl->rcven[channel][rank][byte] = center;
				update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
			}
			printk(BIOS_DEBUG, "\n");
		}

		printk(BIOS_DEBUG, "Step 3: Quarter preamble - Walk backwards\n");
		printk(RCVEN_PLOT, "Byte");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(RCVEN_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(RCVEN_PLOT, "%u ", byte);
		}
		printk(RCVEN_PLOT, "\nIOLAT\n");
		bool done = false;
		while (!done) {
			run_io_test(ctrl, chanmask, BASIC_VA, true);
			done = true;
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				printk(RCVEN_PLOT, "  %2u\t", ctrl->io_latency[channel][rank]);
				uint16_t highs = 0;
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					const bool high = sample_dqs(channel, byte);
					printk(RCVEN_PLOT, high ? "H " : "L ");
					if (high)
						highs |= BIT(byte);
				}
				if (!highs)
					continue;

				done = false;

				/* If all bytes sample high, adjust timing globally */
				if (highs == bytemask && ctrl->io_latency[channel][rank] < 14) {
					ctrl->io_latency[channel][rank] += 2;
					ctrl->io_latency[channel][rank] %= 16;
					program_io_latency(ctrl, channel, rank);
					continue;
				}

				/* Otherwise, adjust individual bytes */
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					if (!(highs & BIT(byte)))
						continue;

					if (ctrl->rcven[channel][rank][byte] < 128) {
						printk(BIOS_ERR,
							"RcvEn underflow: walking backwards\n");
						printk(BIOS_ERR,
							"For channel %u, rank %u, byte %u\n",
							channel, rank, byte);
						status = RAMINIT_STATUS_RCVEN_FAILURE;
						goto clean_up;
					}
					ctrl->rcven[channel][rank][byte] -= 128;
					update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
				}
			}
			printk(RCVEN_PLOT, "\n");
		}
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(BIOS_DEBUG, "\nC%u:  Preamble\n", channel);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				printk(BIOS_DEBUG,
					" B%u: %u\n", byte, ctrl->rcven[channel][rank][byte]);
			}
		}
		printk(BIOS_DEBUG, "\n");

		printk(BIOS_DEBUG, "Step 4: Add 1 qclk\n");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				ctrl->rcven[channel][rank][byte] += 64;
				update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
			}
		}
		printk(BIOS_DEBUG, "\n");

		printk(BIOS_DEBUG, "Step 5: Walk forward to find rising edge\n");
		printk(RCVEN_PLOT, "Byte");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(RCVEN_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(RCVEN_PLOT, "%u ", byte);
		}
		printk(RCVEN_PLOT, "\n inc\n");
		uint16_t ch_result[NUM_CHANNELS] = { 0 };
		uint8_t inc_preamble[NUM_CHANNELS][NUM_LANES] = { 0 };
		for (uint8_t inc = 0; inc < 64; inc += fine_step) {
			printk(RCVEN_PLOT, " %2u\t", inc);
			run_io_test(ctrl, chanmask, BASIC_VA, true);
			done = true;
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					if (ch_result[channel] & BIT(byte)) {
						/* Skip bytes that are already done */
						printk(RCVEN_PLOT, ". ");
						continue;
					}
					const bool pass = sample_dqs(channel, byte);
					printk(RCVEN_PLOT, pass ? ". " : "# ");
					if (pass) {
						ch_result[channel] |= BIT(byte);
						continue;
					}
					ctrl->rcven[channel][rank][byte] += fine_step;
					update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
					inc_preamble[channel][byte] = inc;
				}
				printk(RCVEN_PLOT, "\t");
				if (ch_result[channel] != bytemask)
					done = false;
			}
			printk(RCVEN_PLOT, "\n");
			if (done)
				break;
		}
		printk(BIOS_DEBUG, "\n");
		if (!done) {
			printk(BIOS_ERR, "Error: Preamble edge not found for all bytes\n");
			printk(BIOS_ERR, "The final RcvEn results are as follows:\n");
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				printk(BIOS_ERR, "Channel %u Rank %u:  preamble\n",
					channel, rank);
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					printk(BIOS_ERR, " Byte %u: %u%s\n", byte,
						ctrl->rcven[channel][rank][byte],
						(ch_result[channel] ^ bytemask) & BIT(byte)
							? ""
							: " *** Check this byte! ***");
				}
			}
			status = RAMINIT_STATUS_RCVEN_FAILURE;
			goto clean_up;
		}

		printk(BIOS_DEBUG, "Step 6: center on preamble and clean up rank\n");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(BIOS_DEBUG, "C%u:  Preamble increment\n", channel);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				/*
				 * For Traditional, pull in RcvEn by 64. For ULT, take the DQS
				 * drift into account to the specified guardband: tDQSCK_DRIFT.
				 */
				ctrl->rcven[channel][rank][byte] -= tDQSCK_DRIFT;
				update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
				printk(BIOS_DEBUG, " B%u: %u      %u\n", byte,
					ctrl->rcven[channel][rank][byte],
					inc_preamble[channel][byte]);
			}
			printk(BIOS_DEBUG, "\n");
		}
		printk(BIOS_DEBUG, "\n");
	}

clean_up:
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		if (ctrl->lpddr) {
			/**
			 * W/A for b4618574 - @todo: remove for HSW ULT C0
			 * Can't have force_odt_on together with leaker, disable LPDDR mode for
			 * this training step. This write will disable force_odt_on while still
			 * keeping LPDDR mode disabled. Second write will restore LPDDR mode.
			 */
			union ddr_data_control_0_reg data_control_0 = {
				.raw = ctrl->dq_control_0[channel],
			};
			data_control_0.lpddr_mode = 0;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		}
		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), ctrl->dq_control_0[channel]);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			mchbar_write32(DQ_CONTROL_2(channel, byte),
					ctrl->dq_control_2[channel][byte]);
		}
	}
	io_reset();
	if (status)
		return status;

	printk(BIOS_DEBUG, "Step 7: Sync IO latency across all ranks\n");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		status = change_rcven_timing(ctrl, channel);
		if (status)
			return status;
	}
	printk(BIOS_DEBUG, "\nFinal Receive Enable and IO latency settings:\n");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			const union sc_io_latency_reg sc_io_latency = {
				.raw = mchbar_read32(SC_IO_LATENCY_ch(channel)),
			};
			printk(BIOS_DEBUG, "  C%u.R%u: IOLAT = %u  rt_iocomp = %u\n", channel,
				rank, ctrl->io_latency[channel][rank], sc_io_latency.rt_iocomp);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				printk(BIOS_DEBUG, "   B%u:   %u\n", byte,
					ctrl->rcven[channel][rank][byte]);
			}
			printk(BIOS_DEBUG, "\n");
		}
	}
	return status;
}
