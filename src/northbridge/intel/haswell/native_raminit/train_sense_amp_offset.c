/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <lib.h>
#include <types.h>

#include "raminit_native.h"

#define VREF_OFFSET_PLOT	RAM_DEBUG
#define SAMP_OFFSET_PLOT	RAM_DEBUG

struct vref_train_data {
	int8_t best_sum;
	int8_t best_vref;
	int8_t sum_bits;
	uint8_t high_mask;
	uint8_t low_mask;
};

static void propagate_delay_values(struct sysinfo *ctrl, uint8_t channel)
{
	/* Propagate delay values (without a read command) */
	union ddr_data_control_0_reg data_control_0 = {
		.raw = ctrl->dq_control_0[channel],
	};
	data_control_0.read_rf_rd      = 1;
	data_control_0.read_rf_wr      = 0;
	data_control_0.read_rf_rank    = 0;
	data_control_0.force_odt_on    = 1;
	data_control_0.samp_train_mode = 1;
	mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
	udelay(1);
	data_control_0.samp_train_mode = 0;
	mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
}

static enum raminit_status train_vref_offset(struct sysinfo *ctrl)
{
	const int8_t vref_start = -15;
	const int8_t vref_stop  = 15;
	const struct vref_train_data initial_vref_values = {
		.best_sum  = -NUM_LANES,
		.best_vref = 0,
		.high_mask = 0,
		.low_mask  = 0xff,
	};
	struct vref_train_data vref_data[NUM_CHANNELS][NUM_LANES];

	printk(VREF_OFFSET_PLOT, "Plot of sum_bits across Vref settings\nChannel");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		printk(VREF_OFFSET_PLOT, "\t%u\t\t", channel);
	}

	printk(VREF_OFFSET_PLOT, "\nByte");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		printk(VREF_OFFSET_PLOT, "\t");
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			printk(VREF_OFFSET_PLOT, "%u ", byte);
			vref_data[channel][byte] = initial_vref_values;
			union ddr_data_control_2_reg data_control_2 = {
				.raw = ctrl->dq_control_2[channel][byte],
			};
			data_control_2.force_bias_on = 1;
			data_control_2.force_rx_on   = 1;
			mchbar_write32(DQ_CONTROL_2(channel, byte), data_control_2.raw);
		}
	}

	/* Sweep through Vref settings and find point SampOffset of +/- 7 passes */
	printk(VREF_OFFSET_PLOT, "\n1/2 Vref");
	for (int8_t vref = vref_start; vref <= vref_stop; vref++) {
		printk(VREF_OFFSET_PLOT, "\n% 3d", vref);

		/*
		 * To perform this test, enable offset cancel mode and enable ODT.
		 * Check results and update variables. Ideal result is all zeroes.
		 * Clear offset cancel mode at end of test to write RX_OFFSET_VDQ.
		 */
		change_1d_margin_multicast(ctrl, RdV, vref, 0, false, REG_FILE_USE_RANK);

		/* Program settings for Vref and SampOffset = 7 (8 + 7) */
		mchbar_write32(DDR_DATA_RX_OFFSET_VDQ, 0xffffffff);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			propagate_delay_values(ctrl, channel);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				const uint8_t feedback = get_data_train_feedback(channel, byte);
				struct vref_train_data *curr_data = &vref_data[channel][byte];
				curr_data->low_mask &= feedback;
				curr_data->sum_bits = -popcnt(feedback);
			}
		}

		/* Program settings for Vref and SampOffset = -7 (8 - 7) */
		mchbar_write32(DDR_DATA_RX_OFFSET_VDQ, 0x11111111);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			propagate_delay_values(ctrl, channel);
			printk(VREF_OFFSET_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				const uint8_t feedback = get_data_train_feedback(channel, byte);
				struct vref_train_data *curr_data = &vref_data[channel][byte];
				curr_data->high_mask |= feedback;
				curr_data->sum_bits += popcnt(feedback);
				printk(VREF_OFFSET_PLOT, "%d ", curr_data->sum_bits);
				if (curr_data->sum_bits > curr_data->best_sum) {
					curr_data->best_sum  = curr_data->sum_bits;
					curr_data->best_vref = vref;
					ctrl->rxvref[channel][0][byte] = vref;
				} else if (curr_data->sum_bits == curr_data->best_sum) {
					curr_data->best_vref = vref;
				}
			}
		}
	}
	printk(BIOS_DEBUG, "\n\nHi-Lo (XOR):");
	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		printk(BIOS_DEBUG, "\n  C%u:", channel);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			struct vref_train_data *const curr_data = &vref_data[channel][byte];
			const uint8_t bit_xor = curr_data->high_mask ^ curr_data->low_mask;
			printk(BIOS_DEBUG, "\t0x%02x", bit_xor);
			if (bit_xor == 0xff)
				continue;

			/* Report an error if any bit did not change */
			status = RAMINIT_STATUS_SAMP_OFFSET_FAILURE;
		}
	}
	if (status)
		printk(BIOS_ERR, "\nUnexpected bit error in Vref offset training\n");

	printk(BIOS_DEBUG, "\n\nRdVref:");
	change_1d_margin_multicast(ctrl, RdV, 0, 0, false, REG_FILE_USE_RANK);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		printk(BIOS_DEBUG, "\n  C%u:", channel);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			struct vref_train_data *const curr_data = &vref_data[channel][byte];
			const int8_t vref_width =
				curr_data->best_vref - ctrl->rxvref[channel][0][byte];

			/*
			 * Step size for Rx Vref in DATA_OFFSET_TRAIN is about 3.9 mV
			 * whereas Rx Vref step size in RX_TRAIN_RANK is about 7.8 mV
			 */
			int8_t vref = ctrl->rxvref[channel][0][byte] + vref_width / 2;
			if (vref < 0)
				vref--;
			else
				vref++;

			for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				ctrl->rxvref[channel][rank][byte] = vref / 2;
				update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
			}
			printk(BIOS_DEBUG, "\t% 4d", ctrl->rxvref[channel][0][byte]);
		}
	}
	printk(BIOS_DEBUG, "\n\n");
	return status;
}

/**
 * LPDDR has an additional bit for DQS per each byte.
 *
 * TODO: The DQS value must be written into Data Control 2.
 */
#define NUM_OFFSET_TRAIN_BITS	(NUM_BITS + 1)

#define PLOT_CH_SPACE		"  "

struct samp_train_data {
	uint8_t first_zero;
	uint8_t last_one;
};

static void train_samp_offset(struct sysinfo *ctrl)
{
	const uint8_t max_train_bits = ctrl->lpddr ? NUM_OFFSET_TRAIN_BITS : NUM_BITS;

	struct samp_train_data samp_data[NUM_CHANNELS][NUM_LANES][NUM_OFFSET_TRAIN_BITS] = {0};

	printk(BIOS_DEBUG, "Channel ");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		printk(BIOS_DEBUG, "%u ", channel); /* Same length as PLOT_CH_SPACE */
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
			printk(BIOS_DEBUG, "        %s ", ctrl->lpddr ? " " : "");
	}
	printk(BIOS_DEBUG, "\nByte    ");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
			printk(BIOS_DEBUG, "%u       %s ", byte, ctrl->lpddr ? " " : "");

		printk(BIOS_DEBUG, PLOT_CH_SPACE);
	}
	printk(SAMP_OFFSET_PLOT, "\nBits    ");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
			printk(SAMP_OFFSET_PLOT, "01234567%s ", ctrl->lpddr ? "S" : "");

		printk(SAMP_OFFSET_PLOT, PLOT_CH_SPACE);
	}
	printk(SAMP_OFFSET_PLOT, "\n SAmp\n");
	for (uint8_t samp_offset = 1; samp_offset <= 15; samp_offset++) {
		printk(SAMP_OFFSET_PLOT, "% 5d\t", samp_offset);

		uint32_t rx_offset_vdq = 0;
		for (uint8_t bit = 0; bit < NUM_BITS; bit++) {
			rx_offset_vdq += samp_offset << (4 * bit);
		}
		mchbar_write32(DDR_DATA_RX_OFFSET_VDQ, rx_offset_vdq);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			/* Propagate delay values (without a read command) */
			union ddr_data_control_0_reg data_control_0 = {
				.raw = ctrl->dq_control_0[channel],
			};
			data_control_0.read_rf_rd      = 1;
			data_control_0.read_rf_wr      = 0;
			data_control_0.read_rf_rank    = 0;
			data_control_0.force_odt_on    = 1;
			data_control_0.samp_train_mode = 1;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
			udelay(1);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				const uint32_t feedback =
					get_data_train_feedback(channel, byte);

				for (uint8_t bit = 0; bit < max_train_bits; bit++) {
					struct samp_train_data *const curr_data =
							&samp_data[channel][byte][bit];
					const bool result = feedback & BIT(bit);
					if (result) {
						curr_data->last_one = samp_offset;
					} else if (curr_data->first_zero == 0) {
						curr_data->first_zero = samp_offset;
					}
					printk(SAMP_OFFSET_PLOT, result ? "." : "#");
				}
				printk(SAMP_OFFSET_PLOT, " ");
			}
			printk(SAMP_OFFSET_PLOT, PLOT_CH_SPACE);
			data_control_0.samp_train_mode = 0;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		}
		printk(SAMP_OFFSET_PLOT, "\n");
	}
	printk(BIOS_DEBUG, "\nBitSAmp ");
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			uint32_t rx_offset_vdq = 0;
			for (uint8_t bit = 0; bit < max_train_bits; bit++) {
				struct samp_train_data *const curr_data =
						&samp_data[channel][byte][bit];

				uint8_t vref = curr_data->first_zero + curr_data->last_one;
				vref = clamp_u8(0, vref / 2, 15);
				/*
				 * Check for saturation conditions to make sure
				 * we are as close as possible to Vdd/2 (750 mV).
				 */
				if (curr_data->first_zero == 0)
					vref = 15;
				if (curr_data->last_one == 0)
					vref = 0;

				ctrl->rxdqvrefpb[channel][0][byte][bit].center = vref;
				rx_offset_vdq += vref & 0xf << (4 * bit);
				printk(BIOS_DEBUG, "%x", vref);
			}
			mchbar_write32(RX_OFFSET_VDQ(channel, byte), rx_offset_vdq);
			printk(BIOS_DEBUG, " ");
			download_regfile(ctrl, channel, 1, 0, REG_FILE_USE_RANK, 0, 1, 0);
		}
		printk(BIOS_DEBUG, PLOT_CH_SPACE);
	}
	printk(BIOS_DEBUG, "\n");
}

enum raminit_status train_sense_amp_offset(struct sysinfo *ctrl)
{
	printk(BIOS_DEBUG, "Stage 1: Vref offset training\n");
	const enum raminit_status status = train_vref_offset(ctrl);

	printk(BIOS_DEBUG, "Stage 2: Samp offset training\n");
	train_samp_offset(ctrl);

	/* Clean up after test */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), ctrl->dq_control_0[channel]);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
			mchbar_write32(DQ_CONTROL_2(channel, byte),
				ctrl->dq_control_2[channel][byte]);
	}
	io_reset();
	return status;
}
