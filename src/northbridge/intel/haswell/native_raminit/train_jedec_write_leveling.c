/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"
#include "ranges.h"

#define JWLC_PLOT	RAM_DEBUG
#define JWRL_PLOT	RAM_DEBUG

static void reset_dram_dll(struct sysinfo *ctrl, const uint8_t channel, const uint8_t rank)
{
	const uint16_t mr0reg = ctrl->mr0[channel][rank / 2];
	reut_issue_mrs(ctrl, channel, BIT(rank), 0, mr0reg | MR0_DLL_RESET);
}

static void program_wdb_pattern(struct sysinfo *ctrl, const bool invert)
{
	/* Pattern to keep DQ-DQS simple but detect any failures. Same as NHM/WSM. */
	const uint8_t pat[4][2] = {
		{ 0x00, 0xff },
		{ 0xff, 0x00 },
		{ 0xc3, 0x3c },
		{ 0x3c, 0xc3 },
	};
	const uint8_t pmask[2][8] = {
		{ 0, 0, 1, 1, 1, 1, 0, 0 },
		{ 1, 1, 0, 0, 0, 0, 1, 1 },
	};
	for (uint8_t s = 0; s < ARRAY_SIZE(pat); s++)
		write_wdb_fixed_pat(ctrl, pat[s], pmask[invert], ARRAY_SIZE(pmask[invert]), s);
}

static int16_t set_add_delay(uint32_t *add_delay, uint8_t rank, int8_t target_off)
{
	const uint8_t shift = rank * 2;
	if (target_off > MAX_ADD_DELAY) {
		*add_delay &= ~(3 << shift);
		*add_delay |= MAX_ADD_DELAY << shift;
		return 128 * (target_off - MAX_ADD_DELAY);
	} else if (target_off < 0) {
		*add_delay &= ~(3 << shift);
		*add_delay |= 0 << shift;
		return 128 * target_off;
	} else {
		*add_delay &= ~(3 << shift);
		*add_delay |= target_off << shift;
		return 0;
	}
}

static enum raminit_status train_jedec_write_leveling_cleanup(struct sysinfo *ctrl)
{
	const struct reut_box reut_addr = {
		.col = {
			.start   = 0,
			.stop    = 1023,
			.inc_val = 1,
		},
	};
	const struct wdb_pat wdb_pattern = {
		.start_ptr  = 0,
		.stop_ptr   = 3,
		.inc_rate   = 1,
		.dq_pattern = BASIC_VA,
	};
	const int8_t offsets[] = { 0, 1, -1, 2, 3 };
	const int8_t dq_offsets[] = { 0, -10, 10, -5, 5, -15, 15 };
	const uint8_t dq_offset_max = ARRAY_SIZE(dq_offsets);

	/* Set LFSR seeds to be sequential */
	program_wdb_lfsr(ctrl, true);
	setup_io_test(
		ctrl,
		ctrl->chanmap,
		PAT_WR_RD,
		2,
		4,
		&reut_addr,
		NSOE,
		&wdb_pattern,
		0,
		0);

	const union reut_pat_wdb_cl_mux_cfg_reg reut_wdb_cl_mux_cfg = {
		.mux_0_control       = REUT_MUX_BTBUFFER,
		.mux_1_control       = REUT_MUX_BTBUFFER,
		.mux_2_control       = REUT_MUX_BTBUFFER,
		.ecc_data_source_sel = 1,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		mchbar_write32(REUT_ch_PAT_WDB_CL_MUX_CFG(channel), reut_wdb_cl_mux_cfg.raw);
	}

	int8_t byte_off[NUM_CHANNELS][NUM_LANES] = { 0 };
	uint32_t add_delay[NUM_CHANNELS] = { 0 };
	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	bool invert = false;
	const uint16_t valid_byte_mask = BIT(ctrl->lanes) - 1;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		uint8_t chanmask = 0;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
			chanmask |= select_reut_ranks(ctrl, channel, BIT(rank));

		if (!chanmask)
			continue;

		printk(BIOS_DEBUG, "Rank %u\n", rank);
		printk(JWLC_PLOT, "Channel");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(JWLC_PLOT, "\t\t%u\t", channel);
		}
		printk(JWLC_PLOT, "\nByte\t");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(JWLC_PLOT, "\t");
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(JWLC_PLOT, "%u ", byte);
		}
		printk(JWLC_PLOT, "\nDelay DqOffset");
		bool done = false;
		int8_t byte_sum[NUM_CHANNELS] = { 0 };
		uint16_t byte_pass[NUM_CHANNELS] = { 0 };
		for (uint8_t off = 0; off < ARRAY_SIZE(offsets); off++) {
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				const int16_t global_byte_off =
					set_add_delay(&add_delay[channel], rank, offsets[off]);
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					update_txt(ctrl, channel, rank, byte, TXT_DQDQS_OFF,
						global_byte_off);
				}
				mchbar_write32(SC_WR_ADD_DELAY_ch(channel),
						add_delay[channel]);
			}
			/* Reset FIFOs and DRAM DLL (Micron workaround) */
			if (!ctrl->lpddr) {
				io_reset();
				for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
					if (!rank_in_ch(ctrl, rank, channel))
						continue;

					reset_dram_dll(ctrl, channel, rank);
				}
				udelay(1);
			}
			for (uint8_t dq_offset = 0; dq_offset < dq_offset_max; dq_offset++) {
				printk(JWLC_PLOT, "\n% 3d\t% 3d",
					offsets[off], dq_offsets[dq_offset]);
				change_1d_margin_multicast(
					ctrl,
					WrT,
					dq_offsets[dq_offset],
					rank,
					false,
					REG_FILE_USE_RANK);

				/*
				 * Re-program the WDB pattern. Change the pattern
				 * for the next test to avoid false pass issues.
				 */
				program_wdb_pattern(ctrl, invert);
				invert = !invert;
				run_io_test(ctrl, chanmask, BASIC_VA, true);
				done = true;
				for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
					if (!rank_in_ch(ctrl, rank, channel))
						continue;

					printk(JWLC_PLOT, "\t");
					uint16_t result = get_byte_group_errors(channel);
					result &= valid_byte_mask;

					/* Skip bytes that have failed or already passed */
					const uint16_t skip_me = result | byte_pass[channel];
					for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
						const bool pass = result & BIT(byte);
						printk(JWLC_PLOT, pass ? "# " : ". ");
						if (skip_me & BIT(byte))
							continue;

						byte_pass[channel] |= BIT(byte);
						byte_off[channel][byte] = offsets[off];
						byte_sum[channel] += offsets[off];
					}
					if (byte_pass[channel] != valid_byte_mask)
						done = false;
				}
				if (done)
					break;
			}
			if (done)
				break;
		}
		printk(BIOS_DEBUG, "\n\n");
		if (!done) {
			printk(BIOS_ERR, "JWLC: Could not find a pass for all bytes\n");
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				printk(BIOS_ERR, "Channel %u, rank %u fail:", channel, rank);
				const uint16_t passing_mask = byte_pass[channel];
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					if (BIT(byte) & passing_mask)
						continue;

					printk(BIOS_ERR, " %u", byte);
				}
				printk(BIOS_ERR, "\n");
			}
			status = RAMINIT_STATUS_JWRL_FAILURE;
			break;
		}
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			/* Refine target offset to make sure it works for all bytes */
			int8_t target_off = DIV_ROUND_CLOSEST(byte_sum[channel], ctrl->lanes);
			int16_t global_byte_off = 0;
			uint8_t all_good_loops = 0;
			bool all_good = 0;
			while (!all_good) {
				global_byte_off =
					set_add_delay(&add_delay[channel], rank, target_off);
				all_good = true;
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					int16_t local_offset;
					local_offset = byte_off[channel][byte] - target_off;
					local_offset = local_offset * 128 + global_byte_off;
					const uint16_t tx_dq = ctrl->tx_dq[channel][rank][byte];
					if (tx_dq + local_offset >= (512 - 64)) {
						all_good = false;
						all_good_loops++;
						target_off++;
						break;
					}
					const uint16_t txdqs = ctrl->tx_dq[channel][rank][byte];
					if (txdqs + local_offset < 96) {
						all_good = false;
						all_good_loops++;
						target_off--;
						break;
					}
				}
				/* Avoid an infinite loop */
				if (all_good_loops > 3)
					break;
			}
			if (!all_good) {
				printk(BIOS_ERR, "JWLC: Target offset refining failed\n");
				status = RAMINIT_STATUS_JWRL_FAILURE;
				break;
			}
			printk(BIOS_DEBUG, "C%u.R%u:  Offset\tFinalEdge\n", channel, rank);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				int16_t local_offset;
				local_offset = byte_off[channel][byte] - target_off;
				local_offset = local_offset * 128 + global_byte_off;
				ctrl->tx_dq[channel][rank][byte] += local_offset;
				ctrl->txdqs[channel][rank][byte] += local_offset;
				update_txt(ctrl, channel, rank, byte, TXT_RESTORE, 0);
				printk(BIOS_DEBUG, "  B%u:   %d\t%d\n", byte, local_offset,
					ctrl->txdqs[channel][rank][byte]);
			}
			mchbar_write32(SC_WR_ADD_DELAY_ch(channel), add_delay[channel]);
			if (!ctrl->lpddr) {
				reset_dram_dll(ctrl, channel, rank);
				udelay(1);
			}
			printk(BIOS_DEBUG, "\n");
		}
		printk(BIOS_DEBUG, "\n");
	}

	/* Restore WDB after test */
	write_wdb_va_pat(ctrl, 0, BASIC_VA_PAT_SPREAD_8, 8, 0);
	program_wdb_lfsr(ctrl, false);
	mchbar_write32(DDR_DATA_OFFSET_TRAIN, 0);

	/** TODO: Do full JEDEC init instead? **/
	io_reset();
	return status;
}

static enum raminit_status verify_wl_width(const int32_t lwidth)
{
	if (lwidth <= 32) {
		/* Check if width is valid */
		printk(BIOS_ERR, "WrLevel: Width region (%d) too small\n", lwidth);
		return RAMINIT_STATUS_JWRL_FAILURE;
	}
	if (lwidth >= 96) {
		/* Since we're calibrating a phase, a too large region is a problem */
		printk(BIOS_ERR, "WrLevel: Width region (%d) too large\n", lwidth);
		return RAMINIT_STATUS_JWRL_FAILURE;
	}
	return 0;
}

enum raminit_status train_jedec_write_leveling(struct sysinfo *ctrl)
{
	/*
	 * Enabling WL mode causes DQS to toggle for 1024 QCLK.
	 * Wait for this to stop. Round up to nearest microsecond.
	 */
	const bool wl_long_delay = ctrl->lpddr;
	const uint32_t dqs_toggle_time = wl_long_delay ? 2048 : 1024;
	const uint32_t wait_time_us = DIV_ROUND_UP(ctrl->qclkps * dqs_toggle_time, 1000 * 1000);

	const uint16_t wl_start = 192;
	const uint16_t wl_stop  = 192 + 128;
	const uint16_t wl_step  = 2;

	/* Do not use cached MR values */
	const bool save_restore_mrs = ctrl->restore_mrs;
	ctrl->restore_mrs = 0;

	/* Propagate delay values (without a write command) */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		/* Propagate delay values from rank 0 to prevent assertion failures in RTL */
		union ddr_data_control_0_reg data_control_0 = {
			.raw = ctrl->dq_control_0[channel],
		};
		data_control_0.read_rf_rd   = 0;
		data_control_0.read_rf_wr   = 1;
		data_control_0.read_rf_rank = 0;
		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			union ddr_data_control_2_reg data_control_2 = {
				.raw = ctrl->dq_control_2[channel][byte],
			};
			data_control_2.force_bias_on = 1;
			data_control_2.force_rx_on   = 0;
			data_control_2.wl_long_delay = wl_long_delay;
			mchbar_write32(DQ_CONTROL_2(channel, byte), data_control_2.raw);
		}
	}

	if (ctrl->lpddr)
		die("%s: Missing LPDDR support\n", __func__);

	if (!ctrl->lpddr)
		ddr3_program_mr1(ctrl, 0, 1);

	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	struct phase_train_data region_data[NUM_CHANNELS][NUM_LANES] = { 0 };
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!does_rank_exist(ctrl, rank))
			continue;

		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			/** TODO: Differs for LPDDR **/
			uint16_t mr1reg = ctrl->mr1[channel][rank / 2];
			mr1reg &= ~MR1_QOFF_ENABLE;
			mr1reg |= MR1_WL_ENABLE;
			if (is_hsw_ult()) {
				mr1reg &= ~RTTNOM_MASK;
				mr1reg |= encode_ddr3_rttnom(120);
			} else if (ctrl->dpc[channel] == 2) {
				mr1reg &= ~RTTNOM_MASK;
				mr1reg |= encode_ddr3_rttnom(60);
			}
			reut_issue_mrs(ctrl, channel, BIT(rank), 1, mr1reg);

			/* Assert ODT for myself */
			uint8_t odt_matrix = BIT(rank);
			if (ctrl->dpc[channel] == 2) {
				/* Assert ODT for non-target DIMM */
				const uint8_t other_dimm = ((rank + 2) / 2) & 1;
				odt_matrix |= BIT(2 * other_dimm);
			}

			union reut_misc_odt_ctrl_reg reut_misc_odt_ctrl = {
				.raw = 0,
			};
			if (ctrl->lpddr) {
				/* Only one ODT pin for ULT */
				reut_misc_odt_ctrl.odt_on       = 1;
				reut_misc_odt_ctrl.odt_override = 1;
			} else if (!is_hsw_ult()) {
				reut_misc_odt_ctrl.odt_on       = odt_matrix;
				reut_misc_odt_ctrl.odt_override = 0xf;
			}
			mchbar_write32(REUT_ch_MISC_ODT_CTRL(channel), reut_misc_odt_ctrl.raw);
		}
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			/*
			 * Enable write leveling mode in DDR and propagate delay
			 * values (without a write command). Stay in WL mode.
			 */
			union ddr_data_control_0_reg data_control_0 = {
				.raw = ctrl->dq_control_0[channel],
			};
			data_control_0.wl_training_mode = 1;
			data_control_0.tx_pi_on         = 1;
			data_control_0.read_rf_rd       = 0;
			data_control_0.read_rf_wr       = 1;
			data_control_0.read_rf_rank     = rank;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		}
		printk(BIOS_DEBUG, "\nRank %u\n", rank);
		printk(JWRL_PLOT, "Channel\t");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(JWRL_PLOT, "%u", channel);
			if (channel > 0)
				continue;

			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(JWRL_PLOT, "\t");
		}
		printk(JWRL_PLOT, "\nByte");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
				printk(JWRL_PLOT, "\t%u", byte);
		}
		printk(JWRL_PLOT, "\nWlDelay");
		for (uint16_t wl_delay = wl_start; wl_delay < wl_stop; wl_delay += wl_step) {
			printk(JWRL_PLOT, "\n %3u:", wl_delay);
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					update_txt(ctrl, channel, rank, byte, TXT_TXDQS,
						wl_delay);
				}
			}
			/* Wait for the first burst to finish */
			if (wl_delay == wl_start)
				udelay(wait_time_us);

			io_reset();
			udelay(wait_time_us);
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
					const uint32_t feedback =
						get_data_train_feedback(channel, byte);
					const bool pass = (feedback & 0x1ff) >= 16;
					printk(JWRL_PLOT, "\t%c%u", pass ? '.' : '#', feedback);
					phase_record_pass(
						&region_data[channel][byte],
						pass,
						wl_delay,
						wl_start,
						wl_step);
				}
			}
		}
		printk(JWRL_PLOT, "\n");
		printk(BIOS_DEBUG, "\n\tInitSt\tInitEn\tCurrSt\tCurrEn\tLargSt\tLargEn\n");
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			printk(BIOS_DEBUG, "C%u\n", channel);
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				struct phase_train_data *data = &region_data[channel][byte];

				phase_append_initial_to_current(data, wl_start, wl_step);
				printk(BIOS_DEBUG, "   B%u:\t%d\t%d\t%d\t%d\t%d\t%d\n",
					byte,
					data->initial.start,
					data->initial.end,
					data->current.start,
					data->current.end,
					data->largest.start,
					data->largest.end);
			}
		}

		/*
		 * Clean up after test. Very coarsely adjust for
		 * any cycle errors. Program values for TxDQS.
		 */
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			/* Clear ODT before MRS (JEDEC spec) */
			mchbar_write32(REUT_ch_MISC_ODT_CTRL(channel), 0);

			/** TODO: Differs for LPDDR **/
			const uint16_t mr1reg = ctrl->mr1[channel][rank / 2] | MR1_QOFF_ENABLE;
			reut_issue_mrs(ctrl, channel, BIT(rank), 1, mr1reg);

			printk(BIOS_DEBUG, "\nC%u.R%u:  LftEdge Width\n", channel, rank);
			const bool rank_x16 = ctrl->dimms[channel][rank / 2].data.width == 16;
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				struct phase_train_data *data = &region_data[channel][byte];
				const int32_t lwidth = range_width(data->largest);
				int32_t tx_start = data->largest.start;
				printk(BIOS_DEBUG, "  B%u:   %d\t%d\n", byte, tx_start, lwidth);
				status = verify_wl_width(lwidth);
				if (status) {
					printk(BIOS_ERR,
						"WrLevel problems on channel %u, byte %u\n",
						channel, byte);
					goto clean_up;
				}

				/* Align byte pairs if DIMM is x16 */
				if (rank_x16 && (byte & 1)) {
					const struct phase_train_data *const ref_data =
							&region_data[channel][byte - 1];

					if (tx_start > ref_data->largest.start + 64)
						tx_start -= 128;

					if (tx_start < ref_data->largest.start - 64)
						tx_start += 128;
				}

				/* Fix for b4618067 - need to add 1 QCLK to DQS PI */
				if (is_hsw_ult())
					tx_start += 64;

				assert(tx_start >= 0);
				ctrl->txdqs[channel][rank][byte] = tx_start;
				ctrl->tx_dq[channel][rank][byte] = tx_start + 32;
				update_txt(ctrl, channel, rank, byte, TXT_RESTORE, 0);
			}
		}
		printk(BIOS_DEBUG, "\n");
	}

clean_up:
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), ctrl->dq_control_0[channel]);
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			mchbar_write32(DQ_CONTROL_2(channel, byte),
				ctrl->dq_control_2[channel][byte]);
		}
	}
	if (!ctrl->lpddr)
		ddr3_program_mr1(ctrl, 0, 0);

	ctrl->restore_mrs = save_restore_mrs;

	if (status)
		return status;

	/** TODO: If this step fails and dec_wrd is set, clear it and try again **/
	return train_jedec_write_leveling_cleanup(ctrl);
}
