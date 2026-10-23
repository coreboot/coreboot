/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

#define DDR3_RTT_NOM(a, b, c)	(((a) << 9) | ((b) << 6) | ((c) << 2))
#define DDR3_RTT_NOM_MASK	DDR3_RTT_NOM(1, 1, 1)

static uint16_t ddr3_encode_rtt_nom(const unsigned int rtt_nom)
{
	switch (rtt_nom) {
	case 0:		return DDR3_RTT_NOM(0, 0, 0);	/* RttNom is disabled */
	case 20:	return DDR3_RTT_NOM(1, 0, 0);	/* RZQ/12 */
	case 30:	return DDR3_RTT_NOM(1, 0, 1);	/* RZQ/8 */
	case 40:	return DDR3_RTT_NOM(0, 1, 1);	/* RZQ/6 */
	case 60:	return DDR3_RTT_NOM(0, 0, 1);	/* RZQ/4 */
	case 120:	return DDR3_RTT_NOM(0, 1, 0);	/* RZQ/2 */
	}
	printk(BIOS_ERR, "%s: Invalid rtt_nom value %u\n", __func__, rtt_nom);
	return 0;
}

void ddr3_update_rtt_nom(uint16_t *mr1reg, unsigned int rtt_nom)
{
	*mr1reg &= ~DDR3_RTT_NOM_MASK;
	*mr1reg |= ddr3_encode_rtt_nom(rtt_nom);
}

#define DDR3_RTT_WR(a, b)	(((a) << 10) | ((b) << 9))
#define DDR3_RTT_WR_MASK	DDR3_RTT_WR(1, 1)

static uint16_t ddr3_encode_rtt_wr(const unsigned int rtt_wr)
{
	switch (rtt_wr) {
	case 0:		return DDR3_RTT_WR(0, 0);	/* RttWr is disabled */
	case 60:	return DDR3_RTT_WR(0, 1);	/* RZQ/4 */
	case 120:	return DDR3_RTT_WR(1, 0);	/* RZQ/2 */
	}
	printk(BIOS_ERR, "%s: Invalid rtt_wr value %u\n", __func__, rtt_wr);
	return 0;
}

void ddr3_update_rtt_wr(uint16_t *mr2reg, unsigned int rtt_wr)
{
	*mr2reg &= ~DDR3_RTT_WR_MASK;
	*mr2reg |= ddr3_encode_rtt_wr(rtt_wr);
}

static void ddr3_program_mr0(struct sysinfo *ctrl)
{
	assert(ctrl->tWR >= 5 && ctrl->tWR <= 16);
	assert(ctrl->tAA >= 4);
	const uint8_t jedec_tWR[12] = { 1, 2, 3, 4, 5, 5, 6, 6, 7, 7, 0, 0 };
	const uint8_t jedec_cas = ctrl->tAA - 4;

	const union ddr3_mr_0 mr0reg = {
		.burst_length     = 0,
		.cas_latency_msb  = !!(jedec_cas & BIT(3)),
		.read_burst_type  = 0,
		.cas_latency_low  = jedec_cas & 0x7,
		.dll_reset        = 1,
		.write_recovery   = jedec_tWR[ctrl->tWR - 5],
		.precharge_pd_dll = 0,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr0[channel][rank] = mr0reg.raw;
		}
		reut_issue_mrs_all(ctrl, channel, 0, ctrl->mr0[channel]);
	}
}

static void ddr3_program_mr1(struct sysinfo *ctrl)
{
	union ddr3_mr_1 mr1reg = {
		.dll_disable      = 0,
		.od_impedance_ctl = 1, /* 0: RZQ/6, 1: RZQ/7 */
		.additive_latency = 0,
		.wr_leveling_mode = 0,
		.t_dqs            = 0,
		.q_off_enable     = 0,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		ddr3_update_rtt_nom(&mr1reg.raw, ctrl->dpc[channel] == 2 ? 60 : 0);
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr1[channel][rank] = mr1reg.raw;
		}
		reut_issue_mrs_all(ctrl, channel, 1, ctrl->mr1[channel]);
	}
}

static void ddr3_program_mr2(struct sysinfo *ctrl)
{
	assert(ctrl->tCWL >= 5);
	const bool dimm_srt = ctrl->flags.ext_temp_refresh && !ctrl->flags.asr;

	union ddr3_mr_2 mr2reg = {
		.partial_array_sr  = 0,
		.cas_write_latency = ctrl->tCWL - 5,
		.auto_self_refresh = ctrl->flags.asr,
		.self_refresh_temp = dimm_srt,
	};
	ddr3_update_rtt_wr(&mr2reg.raw, is_hsw_ult() ? 120 : 60);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr2[channel][rank] = mr2reg.raw;
		}
		/* MR2 shadow register is similar but not identical to MR2 */
		if (!ctrl->restore_mrs) {
			union tc_mr2_shadow_reg tc_mr2_shadow = {
				.raw = mr2reg.raw & 0x073f,
			};
			for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
				if (!rank_in_ch(ctrl, slot + slot, channel))
					continue;

				if (dimm_srt)
					tc_mr2_shadow.srt_available |= BIT(slot);

				if (ctrl->rank_mirrored[channel] & BIT(slot + slot + 1))
					tc_mr2_shadow.addr_bit_swizzle |= BIT(slot);
			}
			mchbar_write32(TC_MR2_SHADOW_ch(channel), tc_mr2_shadow.raw);
		}
		reut_issue_mrs_all(ctrl, channel, 2, ctrl->mr2[channel]);
	}
}

static void ddr3_program_mr3(struct sysinfo *ctrl)
{
	const union ddr3_mr_3 mr3reg = {
		.mpr_loc  = 0,
		.mpr_mode = 0,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr3[channel][rank] = mr3reg.raw;
		}
		reut_issue_mrs_all(ctrl, channel, 3, ctrl->mr3[channel]);
	}
}

enum nri_status ddr3_jedec_init(struct sysinfo *ctrl)
{
	ddr3_program_mr2(ctrl);
	ddr3_program_mr3(ctrl);
	ddr3_program_mr1(ctrl);
	ddr3_program_mr0(ctrl);
	return reut_issue_zq(ctrl, ctrl->chanmap, ZQ_INIT);
}

enum nri_status exit_selfrefresh(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		/* Fields in ctrl aren't populated on a warm boot */
		union ddr_data_control_0_reg data_control_0 = {
			.raw = mchbar_read32(DQ_CONTROL_0(channel, 0)),
		};
		data_control_0.read_rf_rd = 1;
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_ch(ctrl, rank, channel))
				continue;

			data_control_0.read_rf_rank = rank;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
		}
	}

	/* Time needed to stabilize the DCLK (~6 us) */
	udelay(6);

	/* Pull the DIMMs out of self refresh by asserting CKE high */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		const union reut_misc_cke_ctrl_reg reut_misc_cke_ctrl = {
			.cke_on = ctrl->rankmap[channel],
		};
		mchbar_write32(REUT_ch_MISC_CKE_CTRL(channel), reut_misc_cke_ctrl.raw);
	}
	mchbar_write32(REUT_MISC_ODT_CTRL, 0);

	const enum nri_status status = reut_issue_zq(ctrl, ctrl->chanmap, ZQ_LONG);
	if (status) {
		/* ZQCL errors don't seem to be a fatal problem here */
		printk(BIOS_ERR, "ZQ Long failed during S3 resume or warm reset flow\n");
	}
	return NRI_STATUS_SUCCESS;
}
