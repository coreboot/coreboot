/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <lib.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

static enum nri_status run_memory_scrub(struct sysinfo *ctrl, const uint8_t chanmask)
{
	mchbar_write32(REUT_GLOBAL_CTL, (union reut_seq_ctl_reg) {
		.start_test    = 1,
		.clear_errors  = 1,
		.stop_on_error = 1,
	}.raw);
	const uint8_t ch_error = wait_for_reut(ctrl, chanmask);
	if (ch_error & chanmask) {
		printk(BIOS_ERR, "REUT found channel errors during scrub: 0x%x\n", ch_error);
		return NRI_STATUS_SCRUB_ERROR;
	}
	return NRI_STATUS_SUCCESS;
}

static bool should_scrub_memory(const struct sysinfo *ctrl)
{
	/* Never scrub memory in S3 resume */
	if (ctrl->bootmode == BOOTMODE_S3)
		return false;

	/* ECC requires scrub to initialise parity */
	return ctrl->is_ecc;
}

enum nri_status ecc_scrub(struct sysinfo *ctrl)
{
	if (!should_scrub_memory(ctrl))
		return NRI_STATUS_SUCCESS;

	printk(BIOS_DEBUG, "Performing ECC scrub\n");

	/* Scrub with zeros */
	const uint8_t patterns[1] = { 0 };
	const uint8_t pat_mask[1] = { 0 };
	write_wdb_fixed_pat(ctrl, patterns, pat_mask, ARRAY_SIZE(pat_mask), 0);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		const union reut_seq_cfg_reg reut_seq_cfg = {
			.stop_base_seq_on_wrap_trigger = 1,
			.address_update_rate_mode      = 1,
			.global_control                = 1,
			.initialization_mode           = REUT_MODE_TEST,
			.subsequence_start_pointer     = 0,
			.subsequence_end_pointer       = 0,
			.start_test_delay              = 2,
		};
		mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
		/* Not actually infinite, we stop after wrap triggers */
		program_loop_count(ctrl, channel, LOOPCOUNT_INFINITE);
		/* TODO: Subsequence encoding differs for HSW A0 */
		const union reut_subseq_ctl_reg subseq_a = {
			.num_cachelines_with_scale     = 0,
			.subseq_type                   = SUBSEQ_B_WR,
		};
		mchbar_write32(REUT_ch_SUBSEQ_x_CTL(channel, 0), subseq_a.raw);
		mchbar_write32(REUT_ch_WDB_CL_CTRL(channel), (union reut_pat_wdb_cl_ctrl_reg) {
			.inc_scale = 1,
		}.raw);
	}
	struct reut_box reut_addr = {
		.bank = {
			.wrap_trigger = true,
		},
		.row = {
			.inc_val = 1,
		},
		.col = {
			.inc_val = 1,
		},
	};
	enum nri_status status = 0;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		uint8_t chanmask = 0;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
			chanmask |= select_reut_ranks(ctrl, channel, BIT(rank));

		if (!chanmask)
			continue;

		printk(BIOS_INFO, "Scrubbing rank %u\n", rank);
		const uint8_t slot = rank / 2;
		for (uint8_t bank = 0; bank < 8; bank++) {
			reut_addr.bank.start = bank;
			reut_addr.bank.stop = bank;
			for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
				if (!(BIT(channel) & chanmask))
					continue;

				struct raminit_dimm_info *dimm = &ctrl->dimms[channel][slot];
				const uint32_t row_size = BIT(dimm->data.row_bits);
				const uint32_t col_size = BIT(dimm->data.col_bits);
				reut_addr.row.stop = row_size - 1;
				reut_addr.col.stop = col_size - WDB_CACHE_LINE_SIZE;
				reut_addr.row.inc_rate = col_size / WDB_CACHE_LINE_SIZE;
				program_seq_addr(channel, &reut_addr, false);
			}
			status = run_memory_scrub(ctrl, chanmask);
			if (status) {
				printk(BIOS_ERR, "Rank %u bank %u scrub error!\n", rank, bank);
				break;
			}
		}
		if (status)
			break;
	}
	/* Return to normal operation mode */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
		reut_set_normal_mode(channel);

	if (status)
		printk(BIOS_ERR, "*** Memory scrub error! ***\n");

	return status;
}

enum nri_status alias_check(struct sysinfo *ctrl)
{
	/* MRC skips Alias Check for HSW A0, probably because subseq bitfields differ */
	if (ctrl->stepping == STEPPING_A0)
		return NRI_STATUS_SUCCESS;

	/*
	 * Check if the SDRAM Addressing * Primary Bus Width == SDRAM capacity.
	 * If not, report an alias and exit. TODO: Implement this someday?
	 */

	uint32_t mad_dimm_ch[NUM_CHANNELS] = {};
	if (ctrl->is_ecc) {
		/* Disable ECC, since ECC scrub has yet to occur */
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			mad_dimm_ch[channel] = mchbar_read32(MAD_DIMM(channel));
			union mad_dimm_reg mad_dimm = { .raw = mad_dimm_ch[channel] };
			mad_dimm.ecc_io_on = 0;
			mad_dimm.ecc_logic_on = 0;
			mchbar_write32(MAD_DIMM(channel), mad_dimm.raw);
		}
	}
	setup_wdb(ctrl);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		const union reut_seq_cfg_reg reut_seq_cfg = {
			.global_control            = 1,
			.initialization_mode       = REUT_MODE_TEST,
			.subsequence_start_pointer = 0,
			.subsequence_end_pointer   = 1,
			.start_test_delay          = 2,
		};
		mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
		program_loop_count(ctrl, channel, 0);	/* Loop once */
		mchbar_write32(REUT_ch_WDB_CL_CTRL(channel), (union reut_pat_wdb_cl_ctrl_reg) {
			.inc_scale = 1,
		}.raw);
		const union reut_pat_wdb_cl_mux_cfg_reg pat_wdb_cl_mux_cfg = {
			.mux_0_control = REUT_MUX_LFSR,
			.mux_1_control = REUT_MUX_LFSR,
			.mux_2_control = REUT_MUX_LFSR,
			.ecc_data_source_sel = 1,
		};
		mchbar_write32(REUT_ch_PAT_WDB_CL_MUX_CFG(channel), pat_wdb_cl_mux_cfg.raw);
	}
	struct reut_box reut_addr = {
		.bank = {
			.start   = 1,
			.stop    = 1,
		},
		.row = {
			.inc_val = 1024,
		},
		.col = {
			.inc_val = 32,
		},
	};
	enum nri_status status = 0;
	const uint16_t col_inc_val_unaligned = 1024 / 4;
	for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
		const uint8_t rank = slot + slot;
		uint8_t chanmask = 0;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			chanmask |= select_reut_ranks(ctrl, channel, BIT(rank));
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			struct raminit_dimm_info *const dimm = &ctrl->dimms[channel][slot];
			/*
			 * Since we're counting cache lines and won't wrap the row address, set
			 * row_stop to row_size - 1 to avoid resetting the current address.
			 * Column must wrap. Wrap occurs on the increment that is after writing
			 * to that address. Thus, we set wrap to be the last accessed column.
			 */
			const uint32_t row_size = BIT(dimm->data.row_bits);
			const uint32_t col_size = BIT(dimm->data.col_bits);

			/* Should be >= 4 */
			const uint32_t writes_per_page = col_size / col_inc_val_unaligned;
			reut_addr.col.stop = col_size - col_inc_val_unaligned;
			reut_addr.row.stop = row_size - 1;
			reut_addr.row.inc_rate = writes_per_page - 1;
			program_seq_addr(channel, &reut_addr, false);

			/*
			 * Instead of matching wrap addresses, we will stop on 1 less cache
			 * line write from the top. This works because when aliasing occurs,
			 * the physical addressing size must double for row/col.
			 */
			uint32_t num_cl = log2_ceil(row_size / BIT(10) * writes_per_page - 1);

			/* TODO: Subsequence encoding differs for HSW A0 */
			const union reut_subseq_ctl_reg subseq_a = {
				.num_cachelines_with_scale     = num_cl,
				.subseq_type                   = SUBSEQ_B_WR,
				.reset_curr_base_addr_to_start = 0,
			};
			const union reut_subseq_ctl_reg subseq_b = {
				.num_cachelines_with_scale     = num_cl,
				.subseq_type                   = SUBSEQ_B_RD,
				.reset_curr_base_addr_to_start = 1,
			};
			mchbar_write32(REUT_ch_SUBSEQ_x_CTL(channel, 0), subseq_a.raw);
			mchbar_write32(REUT_ch_SUBSEQ_x_CTL(channel, 1), subseq_b.raw);
		}
		if (!chanmask)
			continue;

		printk(BIOS_DEBUG, "Testing DIMM %u\n", slot);
		status = run_memory_scrub(ctrl, chanmask);
		if (status)
			break;
	}
	if (ctrl->is_ecc) {
		/* Re-enable ECC logic, must scrub afterwards! */
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			mchbar_write32(MAD_DIMM(channel), mad_dimm_ch[channel]);
		}
		/* Wait 4 usec after enabling the ECC IO, needed by HW */
		udelay(4);
	}

	/* Return to normal operation mode */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
		reut_set_normal_mode(channel);

	if (status)
		printk(BIOS_DEBUG, "*** Alias detected! See REUT error above ***\n");

	return status;
}
