/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

#define DDR3_RTTNOM(a, b, c) (((a) << 9) | ((b) << 6) | ((c) << 2))

uint16_t encode_ddr3_rttnom(const uint32_t rttnom)
{
	switch (rttnom) {
	case 0:		return DDR3_RTTNOM(0, 0, 0);	/* RttNom is disabled */
	case 20:	return DDR3_RTTNOM(1, 0, 0);	/* RZQ/12 */
	case 30:	return DDR3_RTTNOM(1, 0, 1);	/* RZQ/8 */
	case 40:	return DDR3_RTTNOM(0, 1, 1);	/* RZQ/6 */
	case 60:	return DDR3_RTTNOM(0, 0, 1);	/* RZQ/4 */
	case 120:	return DDR3_RTTNOM(0, 1, 0);	/* RZQ/2 */
	}
	printk(BIOS_ERR, "%s: Invalid rtt_nom value %u\n", __func__, rttnom);
	return 0;
}

static const uint8_t jedec_wr_t[12] = { 1, 2, 3, 4, 5, 5, 6, 6, 7, 7, 0, 0 };

static void ddr3_program_mr0(struct sysinfo *ctrl, const uint8_t dll_reset)
{
	assert(ctrl->tWR >= 5 && ctrl->tWR <= 16);
	assert(ctrl->tAA >= 4);
	const uint8_t jedec_cas = ctrl->tAA - 4;
	const union {
		struct __packed {
			uint16_t burst_length     : 2; // Bits  1:0
			uint16_t cas_latency_msb  : 1; // Bits  2:2
			uint16_t read_burst_type  : 1; // Bits  3:3
			uint16_t cas_latency_low  : 3; // Bits  6:4
			uint16_t test_mode        : 1; // Bits  7:7
			uint16_t dll_reset        : 1; // Bits  8:8
			uint16_t write_recovery   : 3; // Bits 11:9
			uint16_t precharge_pd_dll : 1; // Bits 12:12
			uint16_t                  : 3; // Bits 15:13
		};
		uint16_t raw;
	} mr0reg = {
		.burst_length     = 0,
		.cas_latency_msb  = !!(jedec_cas & BIT(3)),
		.read_burst_type  = 0,
		.cas_latency_low  = jedec_cas & 0x7,
		.dll_reset        = 1,
		.write_recovery   = jedec_wr_t[ctrl->tWR - 5],
		.precharge_pd_dll = 0,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			if (!rank_in_ch(ctrl, slot + slot, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr0[channel][slot] = mr0reg.raw;
		}
		reut_issue_mrs_all(ctrl, channel, 0, ctrl->mr0[channel]);
	}
}

void ddr3_program_mr1(struct sysinfo *ctrl, const uint8_t wl_mode, const uint8_t q_off)
{
	/*
	 * JESD79-3F (JEDEC DDR3 spec) refers to bit 0 of MR1 as 'DLL Enable'.
	 * However, its encoding is weird, and 'DLL Disable' makes more sense.
	 *
	 * Moreover, bit 5 is part of ODIC (Output Driver Impedance Control),
	 * but all encodings where MR1 bit 5 is 1 are reserved. Thus, omit it.
	 */
	union {
		struct __packed {
			uint16_t dll_disable      : 1; // Bits  0:0
			uint16_t od_impedance_ctl : 1; // Bits  1:1
			uint16_t odt_rtt_nom_low  : 1; // Bits  2:2
			uint16_t additive_latency : 2; // Bits  4:3
			uint16_t                  : 1; // Bits  5:5
			uint16_t odt_rtt_nom_mid  : 1; // Bits  6:6
			uint16_t write_level_mode : 1; // Bits  7:7
			uint16_t                  : 1; // Bits  8:8
			uint16_t odt_rtt_nom_high : 1; // Bits  9:9
			uint16_t                  : 1; // Bits 10:10
			uint16_t t_dqs            : 1; // Bits 11:11
			uint16_t q_off            : 1; // Bits 12:12
			uint16_t                  : 3; // Bits 15:13
		};
		uint16_t raw;
	} mr1reg = {
		.dll_disable      = 0,
		.od_impedance_ctl = 1,	/* RZQ/7 */
		.additive_latency = 0,
		.write_level_mode = wl_mode,
		.t_dqs            = 0,
		.q_off            = q_off,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		mr1reg.raw &= ~RTTNOM_MASK;
		mr1reg.raw |= encode_ddr3_rttnom(ctrl->dpc[channel] == 2 ? 60 : 0);
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			if (!rank_in_ch(ctrl, slot + slot, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr1[channel][slot] = mr1reg.raw;
		}
		reut_issue_mrs_all(ctrl, channel, 1, ctrl->mr1[channel]);
	}
}

enum {
	RTT_WR_OFF = 0,
	RTT_WR_60  = 1,
	RTT_WR_120 = 2,
};

static void ddr3_program_mr2(struct sysinfo *ctrl)
{
	assert(ctrl->tCWL >= 5);
	const bool dimm_srt = ctrl->flags.ext_temp_refresh && !ctrl->flags.asr;

	const union {
		struct __packed {
			uint16_t partial_array_sr  : 3; // Bits  0:2
			uint16_t cas_write_latency : 3; // Bits  5:3
			uint16_t auto_self_refresh : 1; // Bits  6:6
			uint16_t self_refresh_temp : 1; // Bits  7:7
			uint16_t                   : 1; // Bits  8:8
			uint16_t odt_rtt_wr        : 2; // Bits 10:9
			uint16_t                   : 5; // Bits 15:11
		};
		uint16_t raw;
	} mr2reg = {
		.partial_array_sr  = 0,
		.cas_write_latency = ctrl->tCWL - 5,
		.auto_self_refresh = ctrl->flags.asr,
		.self_refresh_temp = dimm_srt,
		.odt_rtt_wr        = is_hsw_ult() ? RTT_WR_120 : RTT_WR_60,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			if (!rank_in_ch(ctrl, slot + slot, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr2[channel][slot] = mr2reg.raw;
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

static void ddr3_program_mr3(struct sysinfo *ctrl, const uint8_t mpr_mode)
{
	const union {
		struct __packed {
			uint16_t mpr_loc  :  2; // Bits  1:0
			uint16_t mpr_mode :  1; // Bits  2:2
			uint16_t          : 13; // Bits 15:3
		};
		uint16_t raw;
	} mr3reg = {
		.mpr_loc  = 0,
		.mpr_mode = mpr_mode,
	};
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			if (!rank_in_ch(ctrl, slot + slot, channel))
				continue;

			if (!ctrl->restore_mrs)
				ctrl->mr3[channel][slot] = mr3reg.raw;
		}
		reut_issue_mrs_all(ctrl, channel, 3, ctrl->mr3[channel]);
	}
}

enum raminit_status ddr3_jedec_init(struct sysinfo *ctrl)
{
	ddr3_program_mr2(ctrl);
	ddr3_program_mr3(ctrl, 0);
	ddr3_program_mr1(ctrl, 0, 0);
	ddr3_program_mr0(ctrl, 1);
	return reut_issue_zq(ctrl, ctrl->chanmap, ZQ_INIT);
}

enum raminit_status exit_selfrefresh(struct sysinfo *ctrl)
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

	const enum raminit_status status = reut_issue_zq(ctrl, ctrl->chanmap, ZQ_LONG);
	if (status) {
		/* ZQCL errors don't seem to be a fatal problem here */
		printk(BIOS_ERR, "ZQ Long failed during S3 resume or warm reset flow\n");
	}
	return RAMINIT_STATUS_SUCCESS;
}
