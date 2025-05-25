/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <lib.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>
#include <types.h>

#include "raminit_native.h"

static void set_cadb_patterns(const uint8_t channel, const uint16_t seeds[NUM_CADB_MUX_SEEDS])
{
	for (uint8_t i = 0; i < NUM_CADB_MUX_SEEDS; i++)
		mchbar_write32(REUT_ch_PAT_CADB_MUX_x(channel, i), seeds[i]);
}

static void setup_cadb(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t vic_spread,
	const uint8_t vic_bit)
{
	const bool lmn_en = false;

	/*
	 * Currently, always start writing at CADB row 0.
	 * Could add a start point parameter in the future.
	 */
	mchbar_write8(REUT_ch_PAT_CADB_WRITE_PTR(channel), 0);
	const uint8_t num_cadb_rows = 8;
	for (uint8_t row = 0; row < num_cadb_rows; row++) {
		const uint8_t lfsr0 = (row >> 0) & 1;
		const uint8_t lfsr1 = (row >> 1) & 1;
		uint64_t reg64 = 0;
		for (uint8_t bit = 0; bit < 22; bit++) {
			uint8_t bremap;
			if (bit >= 19) {
				/* (bremap in 40 .. 42) => CADB data control */
				bremap = bit + 21;
			} else if (bit >= 16) {
				/* (bremap in 24 .. 26) => CADB data bank */
				bremap = bit + 8;
			} else {
				/* (bremap in  0 .. 15) => CADB data address */
				bremap = bit;
			}
			const uint8_t fine = bit % vic_spread;
			reg64 |= ((uint64_t)(fine == vic_bit ? lfsr0 : lfsr1)) << bremap;
		}
		/*
		 * Write row. CADB pointer is auto incremented after every write. This must be
		 * a single 64-bit write, otherwise the CADB pointer will auto-increment twice.
		 */
		mchbar_write64(REUT_ch_PAT_CADB_PROG(channel), reg64);
	}
	const union reut_pat_cadb_mux_ctrl_reg cadb_mux_ctrl = {
		.mux_0_ctrl = lmn_en ? REUT_MUX_LMN : REUT_MUX_LFSR,
		.mux_1_ctrl = REUT_MUX_LFSR,
		.mux_2_ctrl = REUT_MUX_LFSR,
	};
	mchbar_write32(REUT_ch_PAT_CADB_MUX_CTRL(channel), cadb_mux_ctrl.raw);
	const union reut_pat_cl_mux_lmn_reg cadb_cl_mux_lmn = {
		.en_sweep_freq = 1,
		.l_counter     = 1,
		.m_counter     = 1,
		.n_counter     = 6,
	};
	mchbar_write32(REUT_ch_PAT_CADB_CL_MUX_LMN(channel), cadb_cl_mux_lmn.raw);
	const uint16_t cadb_mux_seeds[NUM_CADB_MUX_SEEDS] = { 0x0ea1, 0xbeef, 0xdead };
	set_cadb_patterns(channel, cadb_mux_seeds);
}

static uint32_t calc_rate(const uint32_t rate, const uint32_t lim, const uint8_t scale_bit)
{
	return rate > lim ? log2_ceil(rate - 1) : BIT(scale_bit) | rate;
}

void program_seq_addr(
	const uint8_t channel,
	const struct reut_box *reut_addr,
	const bool log_seq_addr)
{
	const int loglevel = log_seq_addr ? BIOS_ERR : BIOS_NEVER;
	const uint32_t div = 8;
	union reut_seq_base_addr_reg reut_seq_addr_start = {
		.col_addr  = reut_addr->col.start / div,
		.row_addr  = reut_addr->row.start,
		.bank_addr = reut_addr->bank.start,
		.rank_addr = reut_addr->rank.start,
	};
	mchbar_write64(REUT_ch_SEQ_ADDR_START(channel), reut_seq_addr_start.raw);
	reut_seq_addr_start.raw = mchbar_read64(REUT_ch_SEQ_ADDR_START(channel));
	printk(loglevel, "\tStart column:     %u\n", reut_seq_addr_start.col_addr);
	printk(loglevel, "\tStart row:        %u\n", reut_seq_addr_start.row_addr);
	printk(loglevel, "\tStart bank:       %u\n", reut_seq_addr_start.bank_addr);
	printk(loglevel, "\tStart rank:       %u\n", reut_seq_addr_start.rank_addr);
	printk(loglevel, "\n");

	union reut_seq_base_addr_reg reut_seq_addr_stop = {
		.col_addr  = reut_addr->col.stop / div,
		.row_addr  = reut_addr->row.stop,
		.bank_addr = reut_addr->bank.stop,
		.rank_addr = reut_addr->rank.stop,
	};
	mchbar_write64(REUT_ch_SEQ_ADDR_WRAP(channel), reut_seq_addr_stop.raw);
	reut_seq_addr_stop.raw = mchbar_read64(REUT_ch_SEQ_ADDR_WRAP(channel));
	printk(loglevel, "\tStop column:      %u\n", reut_seq_addr_stop.col_addr);
	printk(loglevel, "\tStop row:         %u\n", reut_seq_addr_stop.row_addr);
	printk(loglevel, "\tStop bank:        %u\n", reut_seq_addr_stop.bank_addr);
	printk(loglevel, "\tStop rank:        %u\n", reut_seq_addr_stop.rank_addr);
	printk(loglevel, "\n");

	union reut_seq_misc_ctl_reg reut_seq_misc_ctl = {
		.col_wrap_trigger_en  = reut_addr->col.wrap_trigger,
		.row_wrap_trigger_en  = reut_addr->row.wrap_trigger,
		.bank_wrap_trigger_en = reut_addr->bank.wrap_trigger,
		.rank_wrap_trigger_en = reut_addr->rank.wrap_trigger,
	};
	mchbar_write32(REUT_ch_SEQ_MISC_CTL(channel), reut_seq_misc_ctl.raw);
	printk(loglevel, "\tWrap column:      %u\n", reut_addr->col.wrap_trigger);
	printk(loglevel, "\tWrap row:         %u\n", reut_addr->row.wrap_trigger);
	printk(loglevel, "\tWrap bank:        %u\n", reut_addr->bank.wrap_trigger);
	printk(loglevel, "\tWrap rank:        %u\n", reut_addr->rank.wrap_trigger);
	printk(loglevel, "\n");

	union reut_seq_addr_inc_ctl_reg reut_seq_addr_inc_ctl = {
		.col_addr_update  = calc_rate(reut_addr->col.inc_rate,  31, 7),
		.row_addr_update  = calc_rate(reut_addr->row.inc_rate,  15, 5),
		.bank_addr_update = calc_rate(reut_addr->bank.inc_rate, 31, 7),
		.rank_addr_update = calc_rate(reut_addr->rank.inc_rate, 31, 7),
		.col_addr_increment  = reut_addr->col.inc_val,
		.row_addr_increment  = reut_addr->row.inc_val,
		.bank_addr_increment = reut_addr->bank.inc_val,
		.rank_addr_increment = reut_addr->rank.inc_val,
	};
	printk(loglevel, "\tUpdRate column:   %u\n", reut_addr->col.inc_rate);
	printk(loglevel, "\tUpdRate row:      %u\n", reut_addr->row.inc_rate);
	printk(loglevel, "\tUpdRate bank:     %u\n", reut_addr->bank.inc_rate);
	printk(loglevel, "\tUpdRate rank:     %u\n", reut_addr->rank.inc_rate);
	printk(loglevel, "\n");
	printk(loglevel, "\tUpdRateCR column: %u\n", reut_seq_addr_inc_ctl.col_addr_update);
	printk(loglevel, "\tUpdRateCR row:    %u\n", reut_seq_addr_inc_ctl.row_addr_update);
	printk(loglevel, "\tUpdRateCR bank:   %u\n", reut_seq_addr_inc_ctl.bank_addr_update);
	printk(loglevel, "\tUpdRateCR rank:   %u\n", reut_seq_addr_inc_ctl.rank_addr_update);
	printk(loglevel, "\n");
	printk(loglevel, "\tUpdInc column:    %u\n", reut_seq_addr_inc_ctl.col_addr_increment);
	printk(loglevel, "\tUpdInc row:       %u\n", reut_seq_addr_inc_ctl.row_addr_increment);
	printk(loglevel, "\tUpdInc bank:      %u\n", reut_seq_addr_inc_ctl.bank_addr_increment);
	printk(loglevel, "\tUpdInc rank:      %u\n", reut_seq_addr_inc_ctl.rank_addr_increment);
	printk(loglevel, "\n");
	mchbar_write64(REUT_ch_SEQ_ADDR_INC_CTL(channel), reut_seq_addr_inc_ctl.raw);
}

/*
 * Early steppings take exponential (base 2) loopcount values,
 * but later steppings take linear loopcount values elsewhere.
 * Address the differences in register offset and format here.
 */
void program_loop_count(const struct sysinfo *ctrl, const uint8_t channel, const uint8_t lc_exp)
{
	if (ctrl->stepping >= STEPPING_C0) {
		const uint32_t loopcount = lc_exp >= 32 ? 0 : BIT(lc_exp);
		mchbar_write32(HSW_REUT_ch_SEQ_LOOP_COUNT(channel), loopcount);
	} else {
		const uint8_t loopcount = lc_exp >= 32 ? 0 : lc_exp + 1;
		union reut_seq_cfg_reg reut_seq_cfg = {
			.raw = mchbar_read64(REUT_ch_SEQ_CFG(channel)),
		};
		reut_seq_cfg.early_steppings_loop_count = loopcount;
		mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
	}
}

/* REUT subsequence types (B = Base, O = Offset) */
enum {
	SUBSEQ_B_RD	= 0,
	SUBSEQ_B_WR	= 1,
	SUBSEQ_B_RD_WR	= 2,
	SUBSEQ_B_WR_RD	= 3,
	SUBSEQ_O_RD	= 4,
	SUBSEQ_O_WR	= 5,
};

static void write_subseq(uint8_t ch, uint8_t idx, union reut_subseq_ctl_reg ssq, uint8_t type)
{
	ssq.subseq_type = type;
	mchbar_write32(REUT_ch_SUBSEQ_x_CTL(ch, idx), ssq.raw);
}

static void program_subseq(
	struct sysinfo *const ctrl,
	const uint8_t channel,
	const enum reut_cmd_pat cmd_pat,
	const union reut_subseq_ctl_reg ss_a,
	const union reut_subseq_ctl_reg ss_b)
{
	switch (cmd_pat) {
	case PAT_WR_RD_TA:
		write_subseq(channel, 0, ss_a, SUBSEQ_B_WR);
		for (uint8_t i = 1; i < 7; i++) {
			write_subseq(channel, i, ss_b, SUBSEQ_B_RD_WR);
		}
		write_subseq(channel, 7, ss_a, SUBSEQ_B_RD);
		break;
	case PAT_RD_WR_TA:
		write_subseq(channel, 0, ss_b, SUBSEQ_B_WR_RD);
		break;
	case PAT_ODT_TA:
		write_subseq(channel, 0, ss_a, SUBSEQ_B_WR);
		write_subseq(channel, 1, ss_b, SUBSEQ_B_RD_WR);
		write_subseq(channel, 2, ss_a, SUBSEQ_B_RD);
		write_subseq(channel, 3, ss_b, SUBSEQ_B_WR_RD);
		break;
	default:
		write_subseq(channel, 0, ss_a, SUBSEQ_B_WR);
		write_subseq(channel, 1, ss_a, SUBSEQ_B_RD);
		break;
	}
}

static uint8_t encode_num_cl(const uint16_t number_of_cachelines)
{
	/* Low 7 bits are the value, high bit means exponential/linear value */
	if (number_of_cachelines > 127) {
		return log2_ceil(number_of_cachelines);	/* Assume exponential/pow2 number */
	} else {
		return number_of_cachelines | BIT(7);	/* Small, treat as linear number */
	}
}

void setup_io_test(
	struct sysinfo *ctrl,
	const uint8_t chanmask,
	const enum reut_cmd_pat cmd_pat,
	const uint16_t num_cl,
	const uint8_t lc,
	const struct reut_box *const reut_addr,
	const enum test_stop soe,
	const struct wdb_pat *const pat,
	const uint8_t en_cadb,
	const uint8_t subseq_wait)
{
	if (!chanmask) {
		printk(BIOS_ERR, "\n%s: chanmask is invalid\n", __func__);
		return;
	}

	/*
	 * Prepare variables needed for both channels.
	 * Check for the cases where this MUST be 1: when
	 * we manually walk through subseq ODT and TA Wr.
	 */
	uint8_t lc_exp = MAX(lc - log2_ceil(num_cl), 0);
	if (cmd_pat == PAT_WR_RD_TA || cmd_pat == PAT_ODT_TA)
		lc_exp = 0;

	const uint8_t num_cl_cr = encode_num_cl(num_cl);
	const uint8_t num_cl2_cr = encode_num_cl(2 * num_cl);

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!(chanmask & BIT(channel))) {
			union reut_seq_cfg_reg reut_seq_cfg = {
				.raw = mchbar_read64(REUT_ch_SEQ_CFG(channel)),
			};
			reut_seq_cfg.global_control = 0;
			mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
			continue;
		}

		/*
		 * Program CADB
		 */
		mchbar_write8(REUT_ch_MISC_PAT_CADB_CTRL(channel), !!en_cadb);
		if (en_cadb)
			setup_cadb(ctrl, channel, 7, 8);

		/*
		 * Program sequence
		 */
		uint8_t subseq_start = 0;
		uint8_t subseq_end   = 0;
		switch (cmd_pat) {
		case PAT_WR_RD:
			subseq_end = 1;
			break;
		case PAT_WR:
			break;
		case PAT_RD:
			subseq_start = 1;
			subseq_end   = 1;
			break;
		case PAT_RD_WR_TA:
			break;
		case PAT_WR_RD_TA:
			subseq_end = 7;
			break;
		case PAT_ODT_TA:
			subseq_end = 3;
			break;
		default:
			die("\n%s: Pattern type %u is invalid\n", __func__, cmd_pat);
		}
		const union reut_seq_cfg_reg reut_seq_cfg = {
			.global_control            = 1,
			.initialization_mode       = REUT_MODE_TEST,
			.subsequence_start_pointer = subseq_start,
			.subsequence_end_pointer   = subseq_end,
			.start_test_delay          = 2,
		};
		mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
		program_loop_count(ctrl, channel, lc_exp);
		mchbar_write32(REUT_ch_SEQ_CTL(channel), (union reut_seq_ctl_reg) {
			.clear_errors = 1,
		}.raw);

		/* Program subsequences */
		const union reut_subseq_ctl_reg subseq_a = {
			.num_cachelines_with_scale     = num_cl_cr,
			.subseq_wait                   = subseq_wait,
			.reset_curr_base_addr_to_start = 1,
		};
		const union reut_subseq_ctl_reg subseq_b = {
			.num_cachelines_with_scale     = num_cl2_cr,
			.subseq_wait                   = subseq_wait,
			.reset_curr_base_addr_to_start = 1,
		};

		program_subseq(ctrl, channel, cmd_pat, subseq_a, subseq_b);

		/* Program sequence address */
		program_seq_addr(channel, reut_addr, false);

		/* Program WDB */
		const bool is_linear = pat->inc_rate < 32;
		mchbar_write32(REUT_ch_WDB_CL_CTRL(channel), (union reut_pat_wdb_cl_ctrl_reg) {
			.start_ptr = pat->start_ptr,
			.end_ptr   = pat->stop_ptr,
			.inc_rate  = is_linear ? pat->inc_rate : log2_ceil(pat->inc_rate),
			.inc_scale = is_linear,
		}.raw);

		/* Enable LMN in LMN or CADB modes, used to create lots of supply noise */
		const bool use_lmn = pat->dq_pattern == LMN_VA || pat->dq_pattern == CADB;
		union reut_pat_wdb_cl_mux_cfg_reg pat_wdb_cl_mux_cfg = {
			.mux_0_control = use_lmn ? REUT_MUX_LMN : REUT_MUX_LFSR,
			.mux_1_control = REUT_MUX_LFSR,
			.mux_2_control = REUT_MUX_LFSR,
			.ecc_data_source_sel = 1,
		};

		/* Program LFSR save/restore, too complex unless everything is power of 2 */
		if (cmd_pat == PAT_ODT_TA || cmd_pat == PAT_WR_RD_TA) {
			pat_wdb_cl_mux_cfg.reload_lfsr_seed_rate = log2_ceil(num_cl) + 1;
			pat_wdb_cl_mux_cfg.save_lfsr_seed_rate   = 1;
		}
		mchbar_write32(REUT_ch_PAT_WDB_CL_MUX_CFG(channel), pat_wdb_cl_mux_cfg.raw);

		/* Inversion mask is not used */
		mchbar_write32(REUT_ch_PAT_WDB_INV(channel), 0);

		/* Program error checking */
		const union reut_err_ctl_reg reut_err_ctl = {
			.selective_err_enable_cacheline = 0xff,
			.selective_err_enable_chunk     = 0xff,
			.stop_on_error_control          = soe,
			.stop_on_nth_error              = 1,
		};
		mchbar_write32(REUT_ch_ERR_CONTROL(channel), reut_err_ctl.raw);
		mchbar_write64(REUT_ch_ERR_DATA_MASK(channel), 0);
		mchbar_write8(REUT_ch_ERR_ECC_MASK(channel), 0);
	}

	/* Always do a ZQ short before the beginning of a test */
	reut_issue_zq(ctrl, chanmask, ZQ_SHORT);
}

void setup_io_test_cadb(
	struct sysinfo *ctrl,
	const uint8_t chanmask,
	const uint8_t lc,
	const enum test_stop soe)
{
	const struct reut_box reut_addr = {
		.rank = {
			.start    = 0,
			.stop     = 0,
			.inc_rate = 32,
			.inc_val  = 1,
		},
		.bank = {
			.start    = 0,
			.stop     = 7,
			.inc_rate = 3,
			.inc_val  = 1,
		},
		.row = {
			.start    = 0,
			.stop     = 2047,
			.inc_rate = 3,
			.inc_val  = 73,
		},
		.col = {
			.start    = 0,
			.stop     = 1023,
			.inc_rate = 0,
			.inc_val  = 53,
		},
	};
	const struct wdb_pat pattern = {
		.start_ptr  = 0,
		.stop_ptr   = 9,
		.inc_rate   = 4,
		.dq_pattern = CADB,
	};
	setup_io_test(
		ctrl,
		chanmask,
		PAT_WR_RD,
		128,
		lc,
		&reut_addr,
		soe,
		&pattern,
		1,
		0);

	ctrl->dq_pat_lc = MAX(lc - 2 - 3, 0) + 1;
	ctrl->dq_pat = CADB;
}

void setup_io_test_basic_va(
	struct sysinfo *ctrl,
	const uint8_t chanmask,
	const uint8_t lc,
	const enum test_stop soe)
{
	const uint32_t spread = 8;
	const struct reut_box reut_addr = {
		.rank = {
			.start    = 0,
			.stop     = 0,
			.inc_rate = 32,
			.inc_val  = 1,
		},
		.col = {
			.start    = 0,
			.stop     = 1023,
			.inc_rate = 0,
			.inc_val  = 1,
		},
	};
	const struct wdb_pat pattern = {
		.start_ptr  = 0,
		.stop_ptr   = spread - 1,
		.inc_rate   = 4,
		.dq_pattern = BASIC_VA,
	};
	setup_io_test(
		ctrl,
		chanmask,
		PAT_WR_RD,
		128,
		lc,
		&reut_addr,
		soe,
		&pattern,
		0,
		0);

	ctrl->dq_pat_lc = MAX(lc - 8, 0) + 1;
	ctrl->dq_pat = BASIC_VA;
}

void setup_io_test_mpr(
	struct sysinfo *ctrl,
	const uint8_t chanmask,
	const uint8_t lc,
	const enum test_stop soe)
{
	const struct reut_box reut_addr_ddr = {
		.rank = {
			.start    = 0,
			.stop     = 0,
			.inc_rate = 32,
			.inc_val  = 1,
		},
		.col = {
			.start    = 0,
			.stop     = 1023,
			.inc_rate = 0,
			.inc_val  = 1,
		},
	};
	const struct reut_box reut_addr_lpddr = {
		.bank = {
			.start    = 4,
			.stop     = 4,
			.inc_rate = 0,
			.inc_val  = 0,
		},
	};
	const struct wdb_pat pattern = {
		.start_ptr  = 0,
		.stop_ptr   = 9,
		.inc_rate   = 4,
		.dq_pattern = BASIC_VA,
	};
	setup_io_test(
		ctrl,
		chanmask,
		PAT_RD,
		128,
		lc,
		ctrl->lpddr ? &reut_addr_lpddr : &reut_addr_ddr,
		soe,
		&pattern,
		0,
		0);

	ctrl->dq_pat_lc = 1;
	ctrl->dq_pat = BASIC_VA;
}

uint8_t select_reut_ranks(struct sysinfo *ctrl, const uint8_t channel, uint8_t rankmask)
{
	rankmask &= ctrl->rankmap[channel];

	uint8_t rank_count = 0;
	uint32_t rank_log_to_phys = 0;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_mask(rank, rankmask))
			continue;

		rank_log_to_phys |= rank << (4 * rank_count);
		rank_count++;
	}
	mchbar_write32(REUT_ch_RANK_LOG_TO_PHYS(channel), rank_log_to_phys);

	union reut_seq_cfg_reg reut_seq_cfg = {
		.raw = mchbar_read64(REUT_ch_SEQ_CFG(channel)),
	};
	if (!rank_count) {
		reut_seq_cfg.global_control = 0;
		mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
		return 0;
	}
	union reut_seq_base_addr_reg reut_seq_addr_stop = {
		.raw = mchbar_read64(REUT_ch_SEQ_ADDR_WRAP(channel)),
	};
	reut_seq_addr_stop.rank_addr = rank_count - 1;
	mchbar_write64(REUT_ch_SEQ_ADDR_WRAP(channel), reut_seq_addr_stop.raw);

	reut_seq_cfg.global_control = 1;
	mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
	return BIT(channel);
}

void run_mpr_io_test(const bool clear_errors)
{
	io_reset();
	mchbar_write32(REUT_GLOBAL_CTL, (union reut_seq_ctl_reg) {
		.start_test   = 1,
		.clear_errors = clear_errors,
	}.raw);
	tick_delay(2);
	io_reset();
	tick_delay(2);
	mchbar_write32(REUT_GLOBAL_CTL, (union reut_seq_ctl_reg) {
		.stop_test = 1,
	}.raw);
}

static uint8_t get_num_tests(const uint8_t dq_pat)
{
	switch (dq_pat) {
	case SEGMENT_WDB:	return 4;
	case CADB:		return 7;
	case TURN_AROUND_WR:	return 8;
	case TURN_AROUND_ODT:	return 4;
	case RD_RD_TA:		return 2;
	case RD_RD_TA_ALL:	return 8;
	default:		return 1;
	}
}

uint8_t run_io_test(
	struct sysinfo *const ctrl,
	const uint8_t chanmask,
	const uint8_t dq_pat,
	const bool clear_errors)
{
	/* SEGMENT_WDB only runs 4 tests */
	const uint8_t segment_wdb_lc[4] = { 0, 0, 4, 2 };
	const union reut_pat_wdb_cl_ctrl_reg pat_wdb_cl[4] = {
		[0] = {
			.start_ptr =  0,
			.end_ptr   =  9,
			.inc_rate  = 25,
			.inc_scale = SCALE_LINEAR,
		},
		[1] = {
			.start_ptr =  0,
			.end_ptr   =  9,
			.inc_rate  = 25,
			.inc_scale = SCALE_LINEAR,
		},
		[2] = {
			.start_ptr = 10,
			.end_ptr   = 63,
			.inc_rate  = 19,
			.inc_scale = SCALE_LINEAR,
		},
		[3] = {
			.start_ptr = 10,
			.end_ptr   = 63,
			.inc_rate  = 10,
			.inc_scale = SCALE_LINEAR,
		},
	};
	const bool is_turnaround = dq_pat == RD_RD_TA || dq_pat == RD_RD_TA_ALL;
	const uint8_t num_tests = get_num_tests(dq_pat);
	union tc_bank_rank_a_reg tc_bank_rank_a[NUM_CHANNELS] = { 0 };
	if (is_turnaround) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(chanmask & BIT(channel)))
				continue;

			tc_bank_rank_a[channel].raw = ctrl->tc_bankrank_a[channel].raw;
		}
	}
	for (uint8_t t = 0; t < num_tests; t++) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(chanmask & BIT(channel)))
				continue;

			if (dq_pat == SEGMENT_WDB) {
				mchbar_write32(REUT_ch_WDB_CL_CTRL(channel), pat_wdb_cl[t].raw);
				/*
				 * Skip programming LFSR save/restore. Too complex
				 * unless power of 2. Program desired loopcount.
				 */
				const uint8_t pat_lc = ctrl->dq_pat_lc + segment_wdb_lc[t];
				program_loop_count(ctrl, channel, pat_lc);
			} else if (dq_pat == CADB) {
				setup_cadb(ctrl, channel, num_tests, t);
			} else if (dq_pat == TURN_AROUND_WR || dq_pat == TURN_AROUND_ODT) {
				union reut_seq_cfg_reg reut_seq_cfg = {
					.raw = mchbar_read64(REUT_ch_SEQ_CFG(channel)),
				};
				reut_seq_cfg.subsequence_start_pointer = t;
				reut_seq_cfg.subsequence_end_pointer   = t;
				mchbar_write64(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
				union reut_seq_addr_inc_ctl_reg addr_inc_ctl = {
					.raw = mchbar_read64(REUT_ch_SEQ_ADDR_INC_CTL(channel)),
				};
				uint8_t ta_inc_rate = 1;
				if (dq_pat == TURN_AROUND_WR && (t == 0 || t == 7))
					ta_inc_rate = 0;
				else if (dq_pat == TURN_AROUND_ODT && (t == 0 || t == 2))
					ta_inc_rate = 0;

				/* Program increment rate as linear value */
				addr_inc_ctl.rank_addr_update = BIT(7) | ta_inc_rate;
				addr_inc_ctl.col_addr_update  = BIT(7) | ta_inc_rate;
				mchbar_write64(REUT_ch_SEQ_ADDR_INC_CTL(channel),
						addr_inc_ctl.raw);
			} else if (dq_pat == RD_RD_TA) {
				tc_bank_rank_a[channel].tRDRD_sr = (t == 0) ? 4 : 5;
				mchbar_write32(TC_BANK_RANK_A_ch(channel),
						tc_bank_rank_a[channel].raw);
			} else if (dq_pat == RD_RD_TA_ALL) {
				/*
				 * Program tRDRD for SR and DR. Run 8 tests, covering
				 * tRDRD_sr = 4, 5, 6, 7 and tRDRD_dr = min, +1, +2, +3
				 */
				const uint32_t tRDRD_dr = ctrl->tc_bankrank_a[channel].tRDRD_dr;
				tc_bank_rank_a[channel].tRDRD_sr = (t % 4) + 4;
				tc_bank_rank_a[channel].tRDRD_dr = (t % 4) + tRDRD_dr;
				mchbar_write32(TC_BANK_RANK_A_ch(channel),
						tc_bank_rank_a[channel].raw);

				/* Program linear rank increment rate */
				union reut_seq_addr_inc_ctl_reg addr_inc_ctl = {
					.raw = mchbar_read64(REUT_ch_SEQ_ADDR_INC_CTL(channel)),
				};
				addr_inc_ctl.rank_addr_update = BIT(7) | (t / 4) ? 0 : 31;
				mchbar_write64(REUT_ch_SEQ_ADDR_INC_CTL(channel),
						addr_inc_ctl.raw);
			}
		}
		bool test_soe = false;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(chanmask & BIT(channel)))
				continue;

			const union reut_err_ctl_reg reut_err_ctl = {
				.raw = mchbar_read32(REUT_ch_ERR_CONTROL(channel)),
			};
			const uint8_t soe = reut_err_ctl.stop_on_error_control;
			if (soe != NSOE) {
				test_soe = true;
				break;
			}
		}
		io_reset();
		mchbar_write32(REUT_GLOBAL_CTL, (union reut_seq_ctl_reg) {
			.start_test   = 1,
			.clear_errors = clear_errors && t == 0,
		}.raw);
		struct mono_time prev, curr;
		timer_monotonic_get(&prev);
		union reut_global_err_reg global_err;
		do {
			global_err.raw = mchbar_read32(REUT_GLOBAL_ERR);
			/** TODO: Clean up this mess **/
			timer_monotonic_get(&curr);
			if (mono_time_diff_microseconds(&prev, &curr) > 1000 * 1000) {
				mchbar_write32(REUT_GLOBAL_CTL, (union reut_seq_ctl_reg) {
					.stop_test = 1,
				}.raw);
				printk(BIOS_ERR, "REUT timed out, ch_done: %x\n",
					global_err.ch_test_done);
				break;
			}
		} while ((global_err.ch_test_done & chanmask) != chanmask);
		if (test_soe && global_err.ch_error & chanmask)
			break;
	}
	if (is_turnaround) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!(chanmask & BIT(channel)))
				continue;

			mchbar_write32(TC_BANK_RANK_A_ch(channel),
				ctrl->tc_bankrank_a[channel].raw);
		}
	}
	return ((union reut_global_err_reg)mchbar_read32(REUT_GLOBAL_ERR)).ch_error;
}
