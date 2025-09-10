/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "raminit_native.h"
#include "ranges.h"

#define MAX_BITLANE_LINES	(2 * MAX_POSSIBLE_BOTH + 1)
#define MARGIN_MIN_WIDTH	8

#define MARGIN_1D_PLOT		RAM_DEBUG

static void print_bitlane(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const int32_t m_start,
	const int32_t m_step,
	const int32_t m_stop,
	const union raw_bitlane_errors bit_failures[NUM_CHANNELS][MAX_BITLANE_LINES])
{
	for (int32_t offset = m_start; offset <= m_stop; offset += m_step) {
		const uint32_t index = offset + MAX_POSSIBLE_BOTH;
		const union raw_bitlane_errors *failures = &bit_failures[channel][index];
		printk(MARGIN_1D_PLOT, "\n% 5d\t", offset);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			const uint8_t byte_errors = failures->per_byte[byte];
			for (uint8_t bit = 0; bit < NUM_BITS; bit++)
				printk(MARGIN_1D_PLOT, byte_errors & BIT(bit) ? "#" : ".");
		}
	}
}

static enum raminit_status apply_dq_offsets(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rank,
	const enum margin_parameter param,
	enum raminit_status status,
	const struct linear_train_data chan_data[])
{
	printk(BIOS_DEBUG, "\nC%u.R%u:  Left\tRight\tWidth\tCenter\n", channel, rank);

	for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
		const struct linear_train_data *const curr_data = &chan_data[byte];
		const int32_t lwidth = range_width(curr_data->largest);
		const int32_t center = range_center(curr_data->largest);
		printk(BIOS_DEBUG, "  B%u:   %d\t%d\t%d\t%d",
			byte,
			curr_data->largest.start,
			curr_data->largest.end,
			lwidth,
			center);
		if (lwidth < MARGIN_MIN_WIDTH) {
			printk(BIOS_ERR,
				"\t1D eye too small! channel: %u byte: %u width: %d\n",
				channel, byte, lwidth);
			status = RAMINIT_STATUS_1D_MARGINING_FAILURE;
		} else {
			printk(BIOS_DEBUG, "\n");
		}
		const struct last_margin margin = {
			.start = ABS(curr_data->largest.start - center) * 10,
			.end   = ABS(curr_data->largest.end   - center) * 10,
		};
		switch (param) {
		case RdT:
			ctrl->rxdqsp[channel][rank][byte] += center;
			ctrl->rxdqsn[channel][rank][byte] += center;
			update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
			ctrl->results[LastRxT][rank][channel][byte] = margin;
			break;
		case WrT:
			ctrl->tx_dq[channel][rank][byte] += center;
			update_txt(ctrl, channel, rank, byte, TXT_RESTORE, 0);
			ctrl->results[LastTxT][rank][channel][byte] = margin;
			break;
		case RdV:
			/* RdV training uses half steps */
			ctrl->rxvref[channel][rank][byte] += center / 2;
			update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
			ctrl->results[LastRxV][rank][channel][byte] = margin;
			break;
		default:
			break;
		}
	}
	return status;
}

static const char *const get_delay_string(const enum margin_parameter param)
{
	switch (param) {
		case RdT:	return "RdDqsDelay";
		case WrT:	return "WrDqsDelay";
		case RdV:	return "RdVoltage";
		default:	return NULL;
	}
}

static enum margin_parameter get_per_bit_margin_param(const enum margin_parameter param)
{
	switch (param) {
		case RdT:	return RdTBit;
		case WrT:	return WrTBit;
		case RdV:	return RdVBit;
		default:	return INVALID_MARGIN;
	}
}

static enum raminit_status train_1d_margin(
	struct sysinfo *ctrl,
	const uint8_t chanmask_in,
	const enum margin_parameter param,
	const bool reset_per_bit,
	const uint8_t loopcount)
{
	const char *const delay_string = get_delay_string(param);
	if (!delay_string) {
		printk(BIOS_ERR, "%s: Invalid margin parameter %u\n", __func__, param);
		return RAMINIT_STATUS_INVALID_PARAMETER;
	}
	setup_io_test_basic_va(ctrl, chanmask_in, loopcount, NSOE);

	const enum margin_parameter param_bit = get_per_bit_margin_param(param);
	if (reset_per_bit && param_bit == INVALID_MARGIN) {
		printk(BIOS_ERR, "%s: Invalid per-bit margin for param %u\n", __func__, param);
		return RAMINIT_STATUS_INVALID_PARAMETER;
	}

	const int32_t m_stop = get_max_margin_for_param(param);
	const int32_t m_start = -m_stop;
	const int32_t m_step = 1;
	enum raminit_status status = 0;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!does_rank_exist(ctrl, rank))
			continue;

		/* Hell thinks this should be resetting the currently selected rank */
		if (reset_per_bit) {
			change_1d_margin_multicast(
				ctrl,
				param_bit,
				0x88888888,
				rank,
				true,
				REG_FILE_USE_RANK);
		}
		printk(BIOS_DEBUG, "Rank %u\n", rank);
		printk(MARGIN_1D_PLOT, "Channel");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(MARGIN_1D_PLOT, "\t%u\t\t", channel);
		}
		printk(MARGIN_1D_PLOT, "\nByte");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(MARGIN_1D_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(MARGIN_1D_PLOT, "%u ", byte);
		}
		uint8_t chanmask = 0;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			chanmask |= select_reut_ranks(ctrl, channel, BIT(rank));
			if (!(BIT(channel) & chanmask))
				continue;

			/* Update rank timing to middle value */
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				if (param == RdT) {
					ctrl->rxdqsp[channel][rank][byte] = 32;
					ctrl->rxdqsn[channel][rank][byte] = 32;
					update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
				} else if (param == WrT) {
					/*
					 * Put TxDq in the middle of the strobe and ensure
					 * there is enough room to sweep to the right.
					 */
					uint16_t tx_dq = ctrl->txdqs[channel][rank][byte] + 32;
					if ((tx_dq + m_stop) > 511)
						tx_dq = 511 - m_stop;

					ctrl->tx_dq[channel][rank][byte] = tx_dq;
					update_txt(ctrl, channel, rank, byte, TXT_RESTORE, 0);
				} else if (param == RdV) {
					ctrl->rxvref[channel][rank][byte] = 0;
					update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
				}
			}

			/* Set up REUT error counters to count errors per channel */
			mchbar_write32(REUT_ch_ERR_COUNTER_CTL_x(channel, 0), 0);
		}
		if (!chanmask)
			continue;

		clear_data_offset_train_all(ctrl);
		printk(MARGIN_1D_PLOT, "\n%s", delay_string);
		struct linear_train_data region_data[NUM_CHANNELS][NUM_LANES] = { 0 };
		union raw_bitlane_errors bit_errors[NUM_CHANNELS][MAX_BITLANE_LINES] = { 0 };
		for (int32_t offset = m_start; offset <= m_stop; offset += m_step) {
			printk(MARGIN_1D_PLOT, "\n% 5d", offset);
			change_1d_margin_multicast(
				ctrl,
				param,
				offset,
				0,
				false,
				REG_FILE_USE_START);

			run_io_test(ctrl, chanmask, ctrl->dq_pat, 1);
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!(chanmask & BIT(channel)))
					continue;

				/* Read out byte group error results and update limit */
				const uint16_t result = get_byte_group_errors(channel);
				bit_errors[channel][offset + MAX_POSSIBLE_BOTH] =
						get_bitlane_errors(channel);
				printk(MARGIN_1D_PLOT, "\t");
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					const bool pass = !(result & BIT(byte));
					printk(MARGIN_1D_PLOT, pass ? ". " : "# ");
					linear_record_pass(
						&region_data[channel][byte],
						pass,
						offset,
						m_start,
						m_step);
				}
			}
		}
		printk(MARGIN_1D_PLOT, "\n\nBit lane information");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(chanmask & BIT(channel)))
				continue;

			printk(MARGIN_1D_PLOT, "\nChannel %u\nByte    ", channel);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(MARGIN_1D_PLOT, "%u       ", byte);

			printk(MARGIN_1D_PLOT, "\nBit     ");
			for (uint8_t bit_count = 0; bit_count < ctrl->lanes * 8; bit_count++)
				printk(MARGIN_1D_PLOT, "%u", bit_count % 8);

			printk(MARGIN_1D_PLOT, "\n%s", delay_string);
			print_bitlane(
				ctrl,
				channel,
				m_start,
				m_step,
				m_stop,
				bit_errors);

			printk(MARGIN_1D_PLOT, "\n");
		}
		change_1d_margin_multicast(
			ctrl,
			param,
			0,
			0,
			false,
			REG_FILE_USE_CURRENT);

		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(chanmask & BIT(channel)))
				continue;

			status = apply_dq_offsets(
				ctrl,
				channel,
				rank,
				param,
				status,
				region_data[channel]);
		}
		printk(BIOS_DEBUG, "\n");
	}

	return status;
}

#define DATA_TRAIN_LOOP_COUNT	15

enum raminit_status train_read_timing_centering(struct sysinfo *ctrl)
{
	return train_1d_margin(ctrl, ctrl->chanmap, RdT, true, DATA_TRAIN_LOOP_COUNT);
}

enum raminit_status train_write_timing_centering(struct sysinfo *ctrl)
{
	return train_1d_margin(ctrl, ctrl->chanmap, WrT, true, DATA_TRAIN_LOOP_COUNT);
}

enum raminit_status train_read_voltage_centering(struct sysinfo *ctrl)
{
	/*
	 * We do not reset per-bit RdV. Haswell only has one RX_OFFSET_VDQ register
	 * per rank, so this would require combining training results for all ranks
	 * somehow. While Broadwell and newer platforms have per-rank RX_OFFSET_VDQ
	 * registers, we can keep using sense amp offset training results, for now.
	 */
	return train_1d_margin(ctrl, ctrl->chanmap, RdV, false, DATA_TRAIN_LOOP_COUNT);
}
