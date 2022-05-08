/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"
#include "ranges.h"

#define RMPR_START	(-32)
#define RMPR_STOP	(32)
#define RMPR_STEP	1

#define RMPR_MIN_WIDTH	12

#define RMPR_PLOT	RAM_DEBUG

/*
 * Clear rx_training_mode. For LPDDR, we first need to disable odt_samp_extend_en,
 * then disable rx_training_mode, and finally re-enable odt_samp_extend_en.
 */
static void clear_rx_training_mode(struct sysinfo *ctrl, const uint8_t channel)
{
	for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
		mchbar_write32(DQ_CONTROL_2(channel, byte), ctrl->dq_control_2[channel][byte]);

	if (ctrl->lpddr) {
		union ddr_data_control_0_reg data_control_0 = {
			.raw = mchbar_read32(DDR_DATA_ch_CONTROL_0(channel)),
		};
		data_control_0.odt_samp_extend_en = 0;
		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		tick_delay(1);
		data_control_0.rx_training_mode = 0;
		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		tick_delay(1);
	}
	mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), ctrl->dq_control_0[channel]);
}

static void set_rxdqs_edges_to_midpoint(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				update_rxt(ctrl, channel, rank, byte, RXT_RXDQS_BOTH, 32);
		}
	}
}

static void enter_mpr_train_ddr_mode(struct sysinfo *ctrl, const uint8_t rank)
{
	/* Program MR3 and mask RAS/WE to prevent scheduler from issuing non-read commands */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		if (!ctrl->lpddr)
			reut_issue_mrs(ctrl, channel, BIT(rank), 3, 1 << 2);

		union reut_misc_odt_ctrl_reg reut_misc_odt_ctrl = {
			.raw = mchbar_read32(REUT_ch_MISC_ODT_CTRL(channel)),
		};
		reut_misc_odt_ctrl.mpr_train_ddr_on = 1;
		mchbar_write32(REUT_ch_MISC_ODT_CTRL(channel), reut_misc_odt_ctrl.raw);
	}
}

static void leave_mpr_train_ddr_mode(struct sysinfo *ctrl, const uint8_t rank)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		/*
		 * The mpr_train_ddr_on bit will force a special command.
		 * Therefore, clear it before issuing the MRS command.
		 */
		union reut_misc_odt_ctrl_reg reut_misc_odt_ctrl = {
			.raw = mchbar_read32(REUT_ch_MISC_ODT_CTRL(channel)),
		};
		reut_misc_odt_ctrl.mpr_train_ddr_on = 0;
		mchbar_write32(REUT_ch_MISC_ODT_CTRL(channel), reut_misc_odt_ctrl.raw);
		if (!ctrl->lpddr)
			reut_issue_mrs(ctrl, channel, BIT(rank), 3, 0 << 2);
	}
}

enum raminit_status train_read_mpr(struct sysinfo *ctrl)
{
	set_rxdqs_edges_to_midpoint(ctrl);
	clear_data_offset_train_all(ctrl);
	setup_io_test_mpr(ctrl, ctrl->chanmap, LOOPCOUNT_INFINITE, NSOE);
	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!does_rank_exist(ctrl, rank))
			continue;

		printk(BIOS_DEBUG, "Rank %u\n", rank);
		printk(RMPR_PLOT, "Channel");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(RMPR_PLOT, "\t%u\t\t", channel);
		}
		printk(RMPR_PLOT, "\nByte");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(RMPR_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(RMPR_PLOT, "%u ", byte);
		}
		enter_mpr_train_ddr_mode(ctrl, rank);
		struct linear_train_data region_data[NUM_CHANNELS][NUM_LANES] = { 0 };
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
			select_reut_ranks(ctrl, channel, BIT(rank));

		printk(RMPR_PLOT, "\nDqsDelay\n");
		int8_t dqs_delay;
		for (dqs_delay = RMPR_START; dqs_delay < RMPR_STOP; dqs_delay += RMPR_STEP) {
			printk(RMPR_PLOT, "% 5d", dqs_delay);
			const enum regfile_mode regfile = REG_FILE_USE_START;
			/* Looks like MRC uses rank 0 here, but it feels wrong */
			change_1d_margin_multicast(ctrl, RdT, dqs_delay, rank, false, regfile);
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					union ddr_data_control_2_reg data_control_2 = {
						.raw = ctrl->dq_control_2[channel][byte],
					};
					data_control_2.force_bias_on = 1;
					data_control_2.force_rx_on   = 1;
					data_control_2.leaker_comp   = 0;
					mchbar_write32(DQ_CONTROL_2(channel, byte),
						data_control_2.raw);
				}
				union ddr_data_control_0_reg data_control_0 = {
					.raw = ctrl->dq_control_0[channel],
				};
				data_control_0.rx_training_mode   = 1;
				data_control_0.force_odt_on       = !ctrl->lpddr;
				data_control_0.en_read_preamble   = 0;
				data_control_0.odt_samp_extend_en = ctrl->lpddr;
				const uint32_t reg_offset = DDR_DATA_ch_CONTROL_0(channel);
				mchbar_write32(reg_offset, data_control_0.raw);
			}
			run_mpr_io_test(false);
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				printk(RMPR_PLOT, "\t");
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					uint32_t fb = get_data_train_feedback(channel, byte);
					const bool pass = fb == 1;
					printk(RMPR_PLOT, pass ? ". " : "# ");
					linear_record_pass(
						&region_data[channel][byte],
						pass,
						dqs_delay,
						RMPR_START,
						RMPR_STEP);
				}
			}
			printk(RMPR_PLOT, "\n");
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				clear_rx_training_mode(ctrl, channel);
			}
			io_reset();
		}
		printk(RMPR_PLOT, "\n");
		leave_mpr_train_ddr_mode(ctrl, rank);
		clear_data_offset_train_all(ctrl);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(BIOS_DEBUG, "C%u.R%u: \tLeft\tRight\tWidth\tCenter\tRxDqsPN\n",
				channel, rank);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				struct linear_train_data *data = &region_data[channel][byte];
				const int32_t lwidth = range_width(data->largest);
				if (lwidth <= RMPR_MIN_WIDTH) {
					printk(BIOS_ERR,
						"Bad eye (lwidth %d <= min %d) for byte %u\n",
						lwidth, RMPR_MIN_WIDTH, byte);
					status = RAMINIT_STATUS_RMPR_FAILURE;
				}
				/*
				 * The MPR center may not be ideal on certain platforms for
				 * unknown reasons. If so, adjust it with a magical number.
				 * For Haswell, the magical number is zero. Hell knows why.
				 */
				const int32_t center = range_center(data->largest);
				ctrl->rxdqsp[channel][rank][byte] = center - RMPR_START;
				ctrl->rxdqsn[channel][rank][byte] = center - RMPR_START;
				printk(BIOS_DEBUG, "  B%u: \t%d\t%d\t%d\t%d\t%u\n", byte,
					data->largest.start, data->largest.end, lwidth,
					center, ctrl->rxdqsp[channel][rank][byte]);
			}
			printk(BIOS_DEBUG, "\n");
		}
	}

	/*
	 * Now program the DQS center values on populated ranks. data is taken from
	 * the host struct. We need to do it after all ranks are trained, because we
	 * need to keep the same DQS value on all ranks during the training procedure.
	 */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
		}
	}
	change_1d_margin_multicast(ctrl, RdT, 0, 0, false, REG_FILE_USE_CURRENT);
	io_reset();
	return status;
}
