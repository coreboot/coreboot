/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <lib.h>
#include <northbridge/intel/haswell/haswell.h>
#include <string.h>
#include <types.h>

#include "raminit_native.h"

static void program_misc_control(struct sysinfo *ctrl)
{
	if (!is_hsw_ult())
		return;

	const union ddr_scram_misc_control_reg ddr_scram_misc_ctrl = {
		.ddr_no_ch_interleave = !ctrl->dq_pins_interleaved,
		.lpddr_mode           = ctrl->lpddr,
		.cke_mapping_ch0      = ctrl->lpddr ? ctrl->lpddr_cke_rank_map[0] : 0,
		.cke_mapping_ch1      = ctrl->lpddr ? ctrl->lpddr_cke_rank_map[1] : 0,
	};
	mchbar_write32(DDR_SCRAM_MISC_CONTROL, ddr_scram_misc_ctrl.raw);
}

static void program_mrc_revision(void)
{
	mchbar_write32(MRC_REVISION, 0x01090000);	/* MRC 1.9.0 Build 0 */
}

static void program_ranks_used(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write8(MC_INIT_STATE_ch(channel), ctrl->rankmap[channel]);
		if (!does_ch_exist(ctrl, channel)) {
			mchbar_write32(DDR_CLK_ch_RANKS_USED(channel), 0);
			mchbar_write32(DDR_CTL_ch_CTL_RANKS_USED(channel), 0);
			mchbar_write32(DDR_CKE_ch_CTL_RANKS_USED(channel), 0);
			continue;
		}
		uint32_t clk_ranks_used = ctrl->rankmap[channel];
		if (ctrl->lpddr) {
			/* With LPDDR, the clock usage goes by group instead */
			clk_ranks_used = 0;
			for (uint8_t group = 0; group < NUM_GROUPS; group++) {
				if (ctrl->dq_byte_map[channel][CT_ITERATION_CLOCK][group])
					clk_ranks_used |= BIT(group);
			}
		}
		mchbar_write32(DDR_CLK_ch_RANKS_USED(channel), clk_ranks_used);

		uint32_t ctl_ranks_used = ctrl->rankmap[channel];
		if (is_hsw_ult()) {
			/* Set ODT disable bits */
			/** TODO: May need to do this after JEDEC reset/init **/
			if (ctrl->lpddr && ctrl->lpddr_dram_odt)
				ctl_ranks_used |= 2 << 4;	/* ODT is used on rank 0 */
			else
				ctl_ranks_used |= 3 << 4;
		}
		mchbar_write32(DDR_CTL_ch_CTL_RANKS_USED(channel), ctl_ranks_used);

		uint32_t cke_ranks_used = ctrl->rankmap[channel];
		if (ctrl->lpddr) {
			/* Use CKE-to-rank mapping for LPDDR */
			const uint8_t cke_rank_map = ctrl->lpddr_cke_rank_map[channel];
			cke_ranks_used = 0;
			for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
				/* ULT only has 2 ranks per channel */
				if (rank >= 2)
					break;

				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				for (uint8_t cke = 0; cke < 4; cke++) {
					if (rank == ((cke_rank_map >> cke) & 1))
						cke_ranks_used |= BIT(cke);
				}
			}
		}
		mchbar_write32(DDR_CKE_ch_CTL_RANKS_USED(channel), cke_ranks_used);
	}
}

static const uint8_t rxb_trad[2][5][4] = {
	{	/* Vdd low */
		/* 1067 MT/s,    1333 MT/s,    1600 MT/s,    1867 MT/s,    2133 MT/s, */
		{4, 3, 3, 2}, {4, 4, 3, 2}, {5, 4, 3, 3}, {5, 4, 4, 3}, {5, 4, 4, 3},
	},
	{	/* Vdd hi */
		/* 1067 MT/s,    1333 MT/s,    1600 MT/s,    1867 MT/s,    2133 MT/s, */
		{4, 3, 3, 2}, {4, 4, 3, 2}, {5, 4, 3, 3}, {5, 4, 4, 3}, {4, 4, 3, 3},
	},
};

static const uint8_t rxb_ultx[2][3][4] = {
	{	/* Vdd low */
		/* 1067 MT/s,    1333 MT/s,    1600 MT/s, */
		{5, 6, 6, 5}, {5, 6, 6, 5}, {4, 6, 6, 6},
	},
	{	/* Vdd hi */
		/* 1067 MT/s,    1333 MT/s,    1600 MT/s, */
		{7, 6, 6, 5}, {7, 6, 6, 5}, {7, 6, 6, 6},
	},
};

uint8_t get_rx_bias(const struct sysinfo *ctrl)
{
	const bool is_ult = is_hsw_ult();
	const bool vddhi  = ctrl->vdd_mv > 1350;
	const uint8_t max_rxf = is_ult ? ARRAY_SIZE(rxb_ultx[0]) : ARRAY_SIZE(rxb_trad[0]);
	const uint8_t ref_clk = ctrl->base_freq == 133 ? 4 : 6;
	const uint8_t rx_f    = clamp_s8(0, ctrl->multiplier - ref_clk, max_rxf - 1);
	const uint8_t rx_cb   = mchbar_read32(DDR_CLK_CB_STATUS) & 0x3;
	if (is_ult)
		return rxb_ultx[vddhi][rx_f][rx_cb];
	else
		return rxb_trad[vddhi][rx_f][rx_cb];
}

static void program_ddr_data(struct sysinfo *ctrl, const bool dis_odt_static, const bool vddhi)
{
	const bool is_ult = is_hsw_ult();

	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!does_rank_exist(ctrl, rank))
			continue;

		const union ddr_data_rx_train_rank_reg rx_train = {
			.rcven = 64,
			.dqs_p = 32,
			.dqs_n = 32,
		};
		mchbar_write32(DDR_DATA_RX_TRAIN_RANK(rank), rx_train.raw);
		mchbar_write32(DDR_DATA_RX_PER_BIT_RANK(rank), 0x88888888);

		const union ddr_data_tx_train_rank_reg tx_train = {
			.tx_eq     = TXEQFULLDRV | 11,
			.dq_delay  = 96,
			.dqs_delay = 64,
		};
		mchbar_write32(DDR_DATA_TX_TRAIN_RANK(rank), tx_train.raw);
		mchbar_write32(DDR_DATA_TX_PER_BIT_RANK(rank), 0x88888888);

		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
				ctrl->tx_dq[channel][rank][byte] = tx_train.dq_delay;
				ctrl->txdqs[channel][rank][byte] = tx_train.dqs_delay;
				ctrl->tx_eq[channel][rank][byte] = tx_train.tx_eq;

				ctrl->rcven[channel][rank][byte]  = rx_train.rcven;
				ctrl->rxdqsp[channel][rank][byte] = rx_train.dqs_p;
				ctrl->rxdqsn[channel][rank][byte] = rx_train.dqs_n;
				ctrl->rx_eq[channel][rank][byte]  = rx_train.rx_eq;
			}
		}
	}
	mchbar_write32(DDR_DATA_TX_XTALK, 0);
	mchbar_write32(DDR_DATA_RX_OFFSET_VDQ, 0x88888888);
	mchbar_write32(DDR_DATA_OFFSET_TRAIN, 0);
	mchbar_write32(DDR_DATA_OFFSET_COMP, 0);

	const union ddr_data_control_0_reg data_control_0 = {
		.internal_clocks_on = !is_ult,
		.data_vccddq_hi     = vddhi,
		.disable_odt_static = dis_odt_static,
		.lpddr_mode         = ctrl->lpddr,
		.odt_samp_extend_en = ctrl->lpddr,
		.early_rleak_en     = ctrl->lpddr && ctrl->stepping >= STEPPING_C0,
	};
	mchbar_write32(DDR_DATA_CONTROL_0, data_control_0.raw);

	const union ddr_data_control_1_reg data_control_1 = {
		.dll_mask             = 1,
		.rx_bias_ctl          = get_rx_bias(ctrl),
		.odt_delay            = -2,
		.odt_duration         = 7,
		.sense_amp_delay      = -2,
		.sense_amp_duration   = 7,
	};
	mchbar_write32(DDR_DATA_CONTROL_1, data_control_1.raw);

	clear_data_offset_train_all(ctrl);

	/* Stagger byte turn-on to reduce dI/dT */
	const uint8_t byte_stagger[] = { 0, 4, 1, 5, 2, 6, 3, 7, 8 };
	const uint8_t latency = 2 * ctrl->tAA - 6;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		union ddr_data_control_2_reg data_control_2 = {
			.raw = 0,
		};
		if (is_ult) {
			data_control_2.rx_dqs_amp_offset = 8;
			data_control_2.rx_clk_stg_num    = 0x1f;
			data_control_2.leaker_comp       = ctrl->lpddr ? 3 : 0;
		}
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			const uint8_t stg = latency * byte_stagger[byte] / ctrl->lanes;
			data_control_2.rx_stagger_ctl = stg & 0x1f;
			mchbar_write32(DQ_CONTROL_2(channel, byte), data_control_2.raw);
			ctrl->data_offset_comp[channel][byte].raw = 0;
			ctrl->dq_control_1[channel][byte] = data_control_1.raw;
			ctrl->dq_control_2[channel][byte] = data_control_2.raw;
		}
		ctrl->dq_control_0[channel] = data_control_0.raw;
	}
}

static void program_vsshi_control(struct sysinfo *ctrl, const uint16_t vsshi_mv)
{
	const uint32_t vsshi_control_reg = is_hsw_ult() ? 0x366c : 0x306c;
	const union ddr_comp_vsshi_control_reg ddr_vsshi_control = {
		.vsshi_target    = (vsshi_mv * 192) / ctrl->vdd_mv - 20,
		.hi_bw_divider   = 1,
		.lo_bw_divider   = 1,
		.bw_error        = 2,
		.panic_driver_en = 1,
		.panic_voltage   = 24 / 8, /* Voltage in 8mV steps */
		.gain_boost      = 1,
	};
	mchbar_write32(vsshi_control_reg, ddr_vsshi_control.raw);
	mchbar_write32(DDR_COMP_VSSHI_CONTROL, ddr_vsshi_control.raw);
}

static void calc_vt_slope_code(const uint16_t slope, uint8_t *best_a, uint8_t *best_b)
{
	const int16_t coding[] = {0, -125, -62, -31, 250, 125, 62, 31};
	*best_a = 0;
	*best_b = 0;
	int16_t best_err = slope;
	for (uint8_t b = 0; b < ARRAY_SIZE(coding); b++) {
		for (uint8_t a = b; a < ARRAY_SIZE(coding); a++) {
			int16_t	error = slope - (coding[a] + coding[b]);
			if (error < 0)
				error = -error;

			if (error < best_err) {
				best_err = error;
				*best_a = a;
				*best_b = b;
			}
		}
	}
}

static void program_dimm_vref(struct sysinfo *ctrl, const uint16_t vccio_mv, const bool vddhi)
{
	const bool is_ult = is_hsw_ult();

	/* Static values for ULT */
	uint8_t vt_slope_a = 4;
	uint8_t vt_slope_b = 0;
	if (!is_ult) {
		/* On non-ULT, compute best slope code */
		const uint16_t vt_slope = 1500 * vccio_mv / ctrl->vdd_mv - 1000;
		calc_vt_slope_code(vt_slope, &vt_slope_a, &vt_slope_b);
	}
	const union ddr_data_vref_control_reg ddr_vref_control = {
		.hi_bw_divider  = is_ult ? 0 : 3,
		.lo_bw_divider  = 3,
		.sample_divider = is_ult ? 1 : 3,
		.slow_bw_error  = 1,
		.hi_bw_enable   = 1,
		.vt_slope_b     = vt_slope_b,
		.vt_slope_a     = vt_slope_a,
		.vt_offset      = 0,
	};
	mchbar_write32(is_ult ? 0xf68 : 0xf6c, ddr_vref_control.raw); /* Use CH1 byte 7 */

	const union ddr_data_vref_adjust_reg ddr_vref_adjust = {
		.en_dimm_vref_ca  = 1,
		.en_dimm_vref_ch0 = 1,
		.en_dimm_vref_ch1 = 1,
		.vccddq_hi_qnnn_h = vddhi,
		.hi_z_timer_ctrl  = 3,
	};
	ctrl->dimm_vref = ddr_vref_adjust;
	mchbar_write32(DDR_DATA_VREF_ADJUST, ddr_vref_adjust.raw);
}

static uint32_t pi_code(const uint32_t code)
{
	return code << 21 | code << 14 | code << 7 | code << 0;
}

static void program_ddr_ca(struct sysinfo *ctrl, const bool vddhi)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		const union ddr_clk_controls_reg ddr_clk_controls = {
			.dll_mask   = 1,
			.vccddq_hi  = vddhi,
			.lpddr_mode = ctrl->lpddr,
		};
		mchbar_write32(DDR_CLK_ch_CONTROLS(channel), ddr_clk_controls.raw);

		const union ddr_cmd_controls_reg ddr_cmd_controls = {
			.dll_mask         = 1,
			.vccddq_hi        = vddhi,
			.lpddr_mode       = ctrl->lpddr,
			.early_weak_drive = 3,
			.cmd_tx_eq        = 1,
		};
		mchbar_write32(DDR_CMD_ch_CONTROLS(channel), ddr_cmd_controls.raw);

		const union ddr_cke_ctl_controls_reg ddr_cke_controls = {
			.dll_mask         = 1,
			.vccddq_hi        = vddhi,
			.lpddr_mode       = ctrl->lpddr,
			.early_weak_drive = 3,
			.cmd_tx_eq        = 1,
			.ctl_tx_eq        = 1,
			.ctl_sr_drv       = 2,
		};
		mchbar_write32(DDR_CKE_ch_CTL_CONTROLS(channel), ddr_cke_controls.raw);

		const union ddr_cke_ctl_controls_reg ddr_ctl_controls = {
			.dll_mask       = 1,
			.vccddq_hi      = vddhi,
			.lpddr_mode     = ctrl->lpddr,
			.ctl_tx_eq      = 1,
			.ctl_sr_drv     = 2,
			.la_drv_en_ovrd = 1,	/* Must be set on ULT */
		};
		mchbar_write32(DDR_CTL_ch_CTL_CONTROLS(channel), ddr_ctl_controls.raw);

		const uint8_t cmd_pi = ctrl->lpddr ? 96 : 64;
		mchbar_write32(DDR_CMD_ch_PI_CODING(channel), pi_code(cmd_pi));
		mchbar_write32(DDR_CKE_ch_CMD_PI_CODING(channel), pi_code(cmd_pi));
		mchbar_write32(DDR_CKE_CTL_ch_CTL_PI_CODING(channel), pi_code(64));
		mchbar_write32(DDR_CLK_ch_PI_CODING(channel), pi_code(64));

		mchbar_write32(DDR_CMD_ch_COMP_OFFSET(channel), 0);
		mchbar_write32(DDR_CLK_ch_COMP_OFFSET(channel), 0);
		mchbar_write32(DDR_CKE_CTL_ch_CTL_COMP_OFFSET(channel), 0);

		for (uint8_t group = 0; group < NUM_GROUPS; group++) {
			ctrl->cke_cmd_pi_code[channel][group] = cmd_pi;
			ctrl->cmd_north_pi_code[channel][group] = cmd_pi;
			ctrl->cmd_south_pi_code[channel][group] = cmd_pi;
		}
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			ctrl->clk_pi_code[channel][rank] = 64;
			ctrl->ctl_pi_code[channel][rank] = 64;
		}
	}
}

enum {
	RCOMP_RD_ODT = 0,
	RCOMP_WR_DS_DQ,
	RCOMP_WR_DS_CMD,
	RCOMP_WR_DS_CTL,
	RCOMP_WR_DS_CLK,
	RCOMP_MAX_CODES,
};

struct rcomp_info {
	uint8_t resistor;
	uint8_t sz_steps;
	uint8_t target_r;
	int8_t result;
};

static void program_rcomp_vref(struct sysinfo *ctrl, const bool dis_odt_static)
{
	const bool is_ult = is_hsw_ult();
	/*
	 * +-------------------------------+
	 * | Rcomp resistor values in ohms |
	 * +-----------+------+------+-----+
	 * | Ball name | Trad | ULTX | Use |
	 * +-----------+------+------+-----+
	 * | SM_RCOMP0 | 100  | 200  | CMD |
	 * | SM_RCOMP1 |  75  | 120  | DQ  |
	 * | SM_RCOMP2 | 100  | 100  | ODT |
	 * +-----------+------+------+-----+
	 */
	struct rcomp_info rcomp_cfg[RCOMP_MAX_CODES] = {
		[RCOMP_RD_ODT] = {
			.resistor = 50,
			.sz_steps = 96,
			.target_r = 50,
		},
		[RCOMP_WR_DS_DQ] = {
			.resistor = 25,
			.sz_steps = 64,
			.target_r = 33,
		},
		[RCOMP_WR_DS_CMD] = {
			.resistor = 20,
			.sz_steps = 64,
			.target_r = 20,
		},
		[RCOMP_WR_DS_CTL] = {
			.resistor = 20,
			.sz_steps = 64,
			.target_r = 20,
		},
		[RCOMP_WR_DS_CLK] = {
			.resistor = 25,
			.sz_steps = 64,
			.target_r = 29,
		},
	};
	if (is_ult) {
		rcomp_cfg[RCOMP_WR_DS_DQ].resistor = 40;
		rcomp_cfg[RCOMP_WR_DS_DQ].target_r = 40;
		rcomp_cfg[RCOMP_WR_DS_CLK].resistor = 40;
	} else if (ctrl->dpc[0] == 2 || ctrl->dpc[1] == 2) {
		rcomp_cfg[RCOMP_RD_ODT].target_r = 60;
	}
	for (uint8_t i = 0; i < RCOMP_MAX_CODES; i++) {
		struct rcomp_info *const r = &rcomp_cfg[i];
		const int32_t div = 2 * (r->resistor + r->target_r);
		assert(div);
		const int32_t vref = (r->sz_steps * (r->resistor - r->target_r)) / div;

		/* DqOdt is 5 bits wide, the other Rcomp targets are 4 bits wide */
		const int8_t comp_limit = i == RCOMP_RD_ODT ? 16 : 8;
		r->result = clamp_s32(-comp_limit, vref, comp_limit - 1);
	}
	const union ddr_comp_ctl_0_reg ddr_comp_ctl_0 = {
		.disable_odt_static = dis_odt_static,
		.dq_drv_vref        = rcomp_cfg[RCOMP_WR_DS_DQ].result,
		.dq_odt_vref        = rcomp_cfg[RCOMP_RD_ODT].result,
		.cmd_drv_vref       = rcomp_cfg[RCOMP_WR_DS_CMD].result,
		.ctl_drv_vref       = rcomp_cfg[RCOMP_WR_DS_CTL].result,
		.clk_drv_vref       = rcomp_cfg[RCOMP_WR_DS_CLK].result,
	};
	ctrl->comp_ctl_0 = ddr_comp_ctl_0;
	mchbar_write32(DDR_COMP_CTL_0, ctrl->comp_ctl_0.raw);
}

enum {
	SCOMP_DQ = 0,
	SCOMP_CMD,
	SCOMP_CTL,
	SCOMP_CLK,
	SCOMP_MAX_CODES,
};

static void program_slew_rates(struct sysinfo *ctrl, const bool vddhi)
{
	const uint8_t min_cycle_delay[SCOMP_MAX_CODES] = { 46, 70, 70, 46 };
	uint8_t buffer_stage_delay_ps[SCOMP_MAX_CODES] = { 59, 53, 53, 53 };
	uint16_t comp_slew_rate_codes[SCOMP_MAX_CODES];

	/* CMD Slew Rate = 1.8 for 2N */
	if (ctrl->tCMD == 2)
		buffer_stage_delay_ps[SCOMP_CMD] = 89;

	/* CMD Slew Rate = 4 V/ns for double-pumped CMD bus */
	if (ctrl->lpddr)
		buffer_stage_delay_ps[SCOMP_CMD] = 63;

	for (uint8_t i = 0; i < SCOMP_MAX_CODES; i++) {
		uint16_t stages = DIV_ROUND_CLOSEST(ctrl->qclkps, buffer_stage_delay_ps[i]);
		if (stages < 5)
			stages = 5;

		bool dll_pc = buffer_stage_delay_ps[i] < min_cycle_delay[i] || stages > 16;

		/* Lock DLL... */
		if (dll_pc)
			comp_slew_rate_codes[i] = stages / 2 - 1;        /* to a phase */
		else
			comp_slew_rate_codes[i] = (stages - 1) | BIT(4); /* to a cycle */
	}
	union ddr_comp_ctl_1_reg ddr_comp_ctl_1 = {
		.dq_scomp       = comp_slew_rate_codes[SCOMP_DQ],
		.cmd_scomp      = comp_slew_rate_codes[SCOMP_CMD],
		.ctl_scomp      = comp_slew_rate_codes[SCOMP_CTL],
		.clk_scomp      = comp_slew_rate_codes[SCOMP_CLK],
		.vccddq_hi      = vddhi,
	};
	ctrl->comp_ctl_1 = ddr_comp_ctl_1;
	mchbar_write32(DDR_COMP_CTL_1, ctrl->comp_ctl_1.raw);
}

static uint32_t ln_x100(const uint32_t input_x100)
{
	uint32_t val = input_x100;
	uint32_t ret = 0;
	while (val > 271) {
		val = (val * 1000) / 2718;
		ret += 100;
	}
	return ret + (-16 * val * val + 11578 * val - 978860) / 10000;
}

static uint32_t compute_vsshi_vref(struct sysinfo *ctrl, const uint32_t vsshi_tgt, bool up)
{
	const uint32_t delta = 15;
	const uint32_t c_die_vsshi = 2000;
	const uint32_t r_cmd_ref = 100 * 10;
	const uint32_t offset = up ? 64 : 0;
	const uint32_t ln_vsshi = ln_x100((100 * vsshi_tgt) / (vsshi_tgt - delta));
	const uint32_t r_target = (ctrl->qclkps * 2000) / (c_die_vsshi * ln_vsshi);
	const uint32_t r_dividend = 128 * (up ? r_cmd_ref : r_target);
	return r_dividend / (r_cmd_ref + r_target) - offset;
}

static void program_vsshi(struct sysinfo *ctrl, const uint16_t vccio_mv, const uint16_t vsshi)
{
	const uint16_t vsshi_down = vsshi + 24; /* Panic threshold of 24 mV */
	const uint16_t vsshi_up = vccio_mv - vsshi_down;
	const union ddr_comp_vsshi_reg ddr_comp_vsshi = {
		.panic_drv_down_vref = compute_vsshi_vref(ctrl, vsshi_down, false),
		.panic_drv_up_vref   = compute_vsshi_vref(ctrl, vsshi_up, true),
		.vt_offset           = 128 * 450 / vccio_mv / 2,
		.vt_slope_a          = 4,
	};
	mchbar_write32(DDR_COMP_VSSHI, ddr_comp_vsshi.raw);
}

static void program_misc(struct sysinfo *ctrl)
{
	ctrl->misc_control_0.raw = mchbar_read32(DDR_SCRAM_MISC_CONTROL);
	ctrl->misc_control_0.weaklock_latency = 12;
	ctrl->misc_control_0.wl_sleep_cycles  =  5;
	ctrl->misc_control_0.wl_wake_cycles   =  2;
	mchbar_write32(DDR_SCRAM_MISC_CONTROL, ctrl->misc_control_0.raw);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		/* Keep scrambling disabled for training */
		mchbar_write32(DDR_SCRAMBLE_ch(channel), 0);
	}
}

/* Very weird, application-specific function */
static void override_comp(uint32_t value, uint32_t width, uint32_t shift, uint32_t offset)
{
	const uint32_t mask = (1 << width) - 1;
	uint32_t reg32 = mchbar_read32(offset);
	reg32 &= ~(mask << shift);
	reg32 |= (value << shift);
	mchbar_write32(offset, reg32);
}

static void program_ls_comp(struct sysinfo *ctrl)
{
	force_rcomp_and_wait_us(10);

	/* Override level shifter compensation */
	const uint32_t ls_comp = 2;
	override_comp(ls_comp, 3, 28, DDR_DATA_RCOMP_DATA_1);
	override_comp(ls_comp, 3, 24, DDR_CMD_COMP);
	override_comp(ls_comp, 3, 24, DDR_CKE_CTL_COMP);
	override_comp(ls_comp, 3, 23, DDR_CLK_COMP);
	override_comp(ls_comp, 3, 28, DDR_DATA_COMP_1);
	override_comp(ls_comp, 3, 24, DDR_COMP_CMD_COMP);
	override_comp(ls_comp, 4, 24, DDR_COMP_CTL_COMP);
	override_comp(ls_comp, 4, 23, DDR_COMP_CLK_COMP);
	override_comp(ls_comp, 3, 24, DDR_COMP_OVERRIDE);

	/* Manually update the COMP values */
	union ddr_scram_misc_control_reg ddr_scram_misc_ctrl = ctrl->misc_control_0;
	ddr_scram_misc_ctrl.force_comp_update = 1;
	mchbar_write32(DDR_SCRAM_MISC_CONTROL, ddr_scram_misc_ctrl.raw);

	/* Use a fixed offset between ODT Up/Dn */
	const union ddr_data_comp_1_reg data_comp_1 = {
		.raw = mchbar_read32(DDR_DATA_COMP_1),
	};
	const uint32_t odt_offset = data_comp_1.rcomp_odt_down - data_comp_1.rcomp_odt_up;
	ctrl->comp_ctl_0.odt_up_down_off  = odt_offset;
	ctrl->comp_ctl_0.fixed_odt_offset = 1;
	mchbar_write32(DDR_COMP_CTL_0, ctrl->comp_ctl_0.raw);
}

static void enable_2x_refresh(struct sysinfo *ctrl)
{
	if (!CONFIG(ENABLE_DDR_2X_REFRESH)) {
		return;
	}

	printk(BIOS_DEBUG, "Enabling 2x Refresh\n");
	const bool asr = ctrl->flags.asr;
	const bool lpddr = ctrl->lpddr;

	/* Mutually exclusive */
	assert(!asr || !lpddr);
	if (!asr) {
		uint32_t desired_cfg = 0;
		desired_cfg |= BIT(0);	/* Enable 2x refresh */
		desired_cfg |= BIT(31);	/* Lock */
		if (lpddr) {
			desired_cfg |= 4 << 1;	/* LPDDR MR4 1/2 tREFI */
		}
		uint32_t reg32 = pcode_mailbox_read(MAILBOX_BIOS_CMD_READ_DDR_2X_REFRESH);
		if (reg32 & BIT(31)) {
			/* Locked, check if programmed correctly */
			const uint32_t current_cfg = reg32;
			if (current_cfg != desired_cfg) {
				printk(BIOS_ERR,
					"Mailbox 2x Refresh locked with wrong values: %x != %x",
					current_cfg, desired_cfg);
			}
		} else {
			/* Not locked, program desired value */
			reg32 |= desired_cfg;
			if (pcode_mailbox_write(MAILBOX_BIOS_CMD_WRITE_DDR_2X_REFRESH, reg32)) {
				printk(BIOS_ERR, "Could not enable Mailbox 2x Refresh\n");
			}
		}
		if (!lpddr) {
			return;
		}
	}
	assert(asr || lpddr);
	uint16_t refi_reduction = 50;
	if (lpddr) {
		refi_reduction = 97;
		mchbar_clrbits32(PCU_DDR_PTM_CTL, 1 << 7); /* DISABLE_DRAM_TS */
	}
	/* tREFI is saved in cache, so only update it in cold boots */
	if (ctrl->bootmode == BOOTMODE_COLD) {
		ctrl->tREFI *= refi_reduction;
		ctrl->tREFI /= 100;
	}
}

static void set_pcu_ddr_voltage(const uint16_t vdd_mv)
{
	/** TODO: Handle other voltages? **/
	uint32_t pcu_ddr_voltage;
	switch (vdd_mv) {
	case 1200:
		pcu_ddr_voltage = 3;
		break;
	case 1350:
		pcu_ddr_voltage = 1;
		break;
	default:
	case 1500:
		pcu_ddr_voltage = 0;
		break;
	}
	/* Set bits 0..2 */
	mchbar_write32(PCU_DDR_VOLTAGE, pcu_ddr_voltage);
}

static void program_scheduler(struct sysinfo *ctrl)
{
	/*
	 * ZQ calibration needs to be serialized for LPDDR3. Otherwise,
	 * the processor issues LPDDR3 ZQ calibration in parallel when
	 * exiting Package C7 or deeper. This causes problems for dual
	 * and quad die packages since all ranks share the same ZQ pin.
	 *
	 * Erratum HSM94: LPDDR3 ZQ Calibration Following Deep Package
	 * C-state Exit May Lead to Unpredictable System Behavior
	 */
	const union mcscheds_cbit_reg mcscheds_cbit = {
		.dis_write_gap = 1,
		.dis_odt       = is_hsw_ult() && !(ctrl->lpddr && ctrl->lpddr_dram_odt),
		.serialize_zq  = ctrl->lpddr,
	};
	mchbar_write32(MCSCHEDS_CBIT, mcscheds_cbit.raw);
	mchbar_write32(MCMNTS_SC_WDBWM, 0x553c3038);
	if (ctrl->lpddr) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			union mcmain_command_rate_limit_reg cmd_rate_limit = {
				.raw = mchbar_read32(COMMAND_RATE_LIMIT_ch(channel)),
			};
			cmd_rate_limit.enable_cmd_limit = 1;
			cmd_rate_limit.cmd_rate_limit   = 3;
			mchbar_write32(COMMAND_RATE_LIMIT_ch(channel), cmd_rate_limit.raw);
		}
	}
}

static uint8_t biggest_channel(const struct sysinfo *const ctrl)
{
	_Static_assert(NUM_CHANNELS == 2, "Code assumes exactly two channels");
	return !!(ctrl->channel_size_mb[0] < ctrl->channel_size_mb[1]);
}

static void dram_zones(struct sysinfo *ctrl)
{
	/** TODO: Activate channel hash here, if enabled **/
	const uint8_t biggest = biggest_channel(ctrl);
	const uint8_t smaller = !biggest;

	/** TODO: Use stacked mode if Memory Trace is enabled **/
	const union mad_chnl_reg mad_channel = {
		.ch_a       = biggest,
		.ch_b       = smaller,
		.ch_c       = 2,
		.lpddr_mode = ctrl->lpddr,
	};
	mchbar_write32(MAD_CHNL, mad_channel.raw);

	const uint8_t channel_b_zone_size = ctrl->channel_size_mb[smaller] / 256;
	const union mad_zr_reg mad_zr = {
		.ch_b_double = channel_b_zone_size * 2,
		.ch_b_single = channel_b_zone_size,
	};
	mchbar_write32(MAD_ZR, mad_zr.raw);
}

static uint8_t biggest_dimm(const struct raminit_dimm_info *dimms)
{
	_Static_assert(NUM_SLOTS <= 2, "Code assumes at most two DIMMs per channel.");
	if (NUM_SLOTS == 1)
		return 0;

	return !!(dimms[0].data.size_mb < dimms[1].data.size_mb);
}

static void dram_dimm_mapping(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel)) {
			const union mad_dimm_reg mad_dimm = {
				.rank_interleave = 1,
				.enh_interleave  = 1,
			};
			mchbar_write32(MAD_DIMM(channel), mad_dimm.raw);
			continue;
		}
		const uint8_t biggest = biggest_dimm(ctrl->dimms[channel]);
		const uint8_t smaller = !biggest;
		const struct dimm_attr_ddr3_st *dimm_a = &ctrl->dimms[channel][biggest].data;
		const struct dimm_attr_ddr3_st *dimm_b = &ctrl->dimms[channel][smaller].data;
		union mad_dimm_reg mad_dimm = {
			.dimm_a_size     = dimm_a->size_mb / 256,
			.dimm_b_size     = dimm_b->size_mb / 256,
			.dimm_a_sel      = biggest,
			.dimm_a_ranks    = dimm_a->ranks == 2,
			.dimm_b_ranks    = dimm_b->ranks == 2,
			.dimm_a_width    = dimm_a->width == 16,
			.dimm_b_width    = dimm_b->width == 16,
			.rank_interleave = 1,
			.enh_interleave  = 1,
			.ecc_mode        = 0,	/* Do not enable ECC yet */
		};
		if (is_hsw_ult())
			mad_dimm.dimm_b_width = mad_dimm.dimm_a_width;

		mchbar_write32(MAD_DIMM(channel), mad_dimm.raw);
		if (ctrl->lpddr)
			die("%s: Missing LPDDR support (LPDDR_MR_PARAMS)\n", __func__);
	}
}

enum raminit_status configure_mc(struct sysinfo *ctrl)
{
	const uint16_t vccio_mv = 1000;
	const uint16_t vsshi_mv = ctrl->vdd_mv - 950;
	const bool dis_odt_static = is_hsw_ult(); /* Disable static ODT legs on ULT */
	const bool vddhi = ctrl->vdd_mv > 1350;

	program_misc_control(ctrl);
	program_mrc_revision();
	program_ranks_used(ctrl);
	program_ddr_data(ctrl, dis_odt_static, vddhi);
	program_vsshi_control(ctrl, vsshi_mv);
	program_dimm_vref(ctrl, vccio_mv, vddhi);
	program_ddr_ca(ctrl, vddhi);
	program_rcomp_vref(ctrl, dis_odt_static);
	program_slew_rates(ctrl, vddhi);
	program_vsshi(ctrl, vccio_mv, vsshi_mv);
	program_misc(ctrl);
	program_ls_comp(ctrl);
	enable_2x_refresh(ctrl);
	set_pcu_ddr_voltage(ctrl->vdd_mv);
	configure_timings(ctrl);
	configure_refresh(ctrl);
	program_scheduler(ctrl);
	dram_zones(ctrl);
	dram_dimm_mapping(ctrl);

	return RAMINIT_STATUS_SUCCESS;
}
