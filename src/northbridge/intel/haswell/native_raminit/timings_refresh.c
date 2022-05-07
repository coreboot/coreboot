/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>

#include "raminit_native.h"

#define BL		8	/* Burst length */
#define tCCD		4
#define tRPRE		1
#define tWPRE		1
#define tDLLK		512

static bool is_sodimm(const enum spd_dimm_type_ddr3 type)
{
	return type == SPD_DDR3_DIMM_TYPE_SO_DIMM || type == SPD_DDR3_DIMM_TYPE_72B_SO_UDIMM;
}

static uint8_t get_odt_stretch(const struct sysinfo *const ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		/* Only stretch with 2 DIMMs per channel */
		if (ctrl->dpc[channel] != 2)
			continue;

		const struct raminit_dimm_info *dimms = ctrl->dimms[channel];

		/* Only stretch when using SO-DIMMs */
		if (!is_sodimm(dimms[0].data.dimm_type) || !is_sodimm(dimms[1].data.dimm_type))
			continue;

		/* Only stretch with mismatched card types */
		if (dimms[0].data.reference_card == dimms[1].data.reference_card)
			continue;

		/* Stretch if one SO-DIMM is card F */
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			if (dimms[slot].data.reference_card == 5)
				return 1;
		}
	}
	return 0;
}

static union tc_bank_reg make_tc_bank(struct sysinfo *const ctrl)
{
	return (union tc_bank_reg) {
		.tRCD      = ctrl->tRCD,
		.tRP       = ctrl->tRP,
		.tRAS      = ctrl->tRAS,
		.tRDPRE    = ctrl->tRTP,
		.tWRPRE    = 4 + ctrl->tCWL + ctrl->tWR,
		.tRRD      = ctrl->tRRD,
		.tRPab_ext = 0,	/** TODO: For LPDDR, this is ctrl->tRPab - ctrl->tRP **/
	};
}

static union tc_bank_rank_a_reg make_tc_bankrank_a(struct sysinfo *ctrl, uint8_t odt_stretch)
{
	/* Use 3N mode for DDR during training, but always use 1N mode for LPDDR */
	const uint32_t tCMD = ctrl->lpddr ? 0 : 3;
	const uint32_t tRDRD_drdd = BL / 2 + 1 + tRPRE + odt_stretch + !!ctrl->lpddr;

	return (union tc_bank_rank_a_reg) {
		.tCKE        = get_tCKE(ctrl->mem_clock_mhz, ctrl->lpddr),
		.tFAW        = ctrl->tFAW,
		.tRDRD_sr    = tCCD,
		.tRDRD_dr    = tRDRD_drdd,
		.tRDRD_dd    = tRDRD_drdd,
		.tRDPDEN     = ctrl->tAA + BL / 2 + 1,
		.cmd_3st_dis = 1,	/* Disable command tri-state before training */
		.cmd_stretch = tCMD,
	};
}

static union tc_bank_rank_b_reg make_tc_bankrank_b(struct sysinfo *const ctrl)
{
	const uint8_t tWRRD_drdd = ctrl->tCWL - ctrl->tAA + BL / 2 + 2 + tRPRE;
	const uint8_t tWRWR_drdd = BL / 2 + 2 + tWPRE;

	return (union tc_bank_rank_b_reg) {
		.tWRRD_sr = tCCD + ctrl->tCWL + ctrl->tWTR + 2,
		.tWRRD_dr = ctrl->lpddr ? 8 : tWRRD_drdd,
		.tWRRD_dd = ctrl->lpddr ? 8 : tWRRD_drdd,
		.tWRWR_sr = tCCD,
		.tWRWR_dr = tWRWR_drdd,
		.tWRWR_dd = tWRWR_drdd,
		.tWRPDEN  = ctrl->tWR + ctrl->tCWL + BL / 2,
		.dec_wrd  = ctrl->tCWL >= 6,
	};
}

static uint32_t get_tRDWR_sr(const struct sysinfo *ctrl)
{
	if (ctrl->lpddr) {
		const uint32_t tdqsck_max = DIV_ROUND_UP(5500, ctrl->qclkps * 2);
		return ctrl->tAA - ctrl->tCWL + tCCD + tWPRE + tdqsck_max + 1;
	} else {
		const bool fast_clock = ctrl->mem_clock_mhz > 666;
		return ctrl->tAA - ctrl->tCWL + tCCD + tWPRE + 2 + fast_clock;
	}
}

static union tc_bank_rank_c_reg make_tc_bankrank_c(struct sysinfo *ctrl, uint8_t odt_stretch)
{
	const uint32_t tRDWR_sr = get_tRDWR_sr(ctrl);
	const uint32_t tRDWR_drdd = tRDWR_sr + odt_stretch;

	return (union tc_bank_rank_c_reg) {
		.tXPDLL   = get_tXPDLL(ctrl->mem_clock_mhz),
		.tXP      = MAX(ctrl->tXP, 7),	/* Use a higher tXP for training */
		.tAONPD   = get_tAONPD(ctrl->mem_clock_mhz),
		.tRDWR_sr = tRDWR_sr,
		.tRDWR_dr = tRDWR_drdd,
		.tRDWR_dd = tRDWR_drdd,
	};
}

static union tc_bank_rank_d_reg make_tc_bankrank_d(struct sysinfo *ctrl, uint8_t odt_stretch)
{
	const uint32_t odt_rd_delay = ctrl->tAA - ctrl->tCWL;
	if (!ctrl->lpddr) {
		return (union tc_bank_rank_d_reg) {
			.tAA               = ctrl->tAA,
			.tCWL              = ctrl->tCWL,
			.tCPDED            = 1,
			.tPRPDEN           = 1,
			.odt_read_delay    = odt_rd_delay,
			.odt_read_duration = odt_stretch,
		};
	}

	/* tCWL has 1 extra clock because of tDQSS, subtract it here */
	const uint32_t tCWL_lpddr = ctrl->tCWL - 1;
	const uint32_t odt_wr_delay = tCWL_lpddr + DIV_ROUND_UP(3500, ctrl->qclkps * 2);
	const uint32_t odt_wr_duration = DIV_ROUND_UP(3500 - 1750, ctrl->qclkps * 2) + 1;

	return (union tc_bank_rank_d_reg) {
		.tAA                = ctrl->tAA,
		.tCWL               = tCWL_lpddr,
		.tCPDED             = 2,	/* Required by JEDEC LPDDR3 spec */
		.tPRPDEN            = 1,
		.odt_read_delay     = odt_rd_delay,
		.odt_read_duration  = odt_stretch,
		.odt_write_delay    = odt_wr_delay,
		.odt_write_duration = odt_wr_duration,
		.odt_always_rank_0  = ctrl->lpddr_dram_odt
	};
}

/* ZQCS period values, in (tREFI * 128) units */
#define ZQCS_PERIOD_DDR3	128	/* tREFI * 128 = 7.8 us * 128 = 1ms */
#define ZQCS_PERIOD_LPDDR3	256	/* tREFI * 128 = 3.9 us * 128 = 0.5ms */

static uint32_t make_tc_zqcal(const struct sysinfo *const ctrl)
{
	const uint32_t zqcs_period = ctrl->lpddr ? ZQCS_PERIOD_LPDDR3 : ZQCS_PERIOD_DDR3;
	const uint32_t tZQCS = get_tZQCS(ctrl->mem_clock_mhz, ctrl->lpddr);
	return tZQCS << (is_hsw_ult() ? 10 : 8) | zqcs_period;
}

static union tc_rftp_reg make_tc_rftp(const struct sysinfo *const ctrl)
{
	/*
	 * The tREFIx9 field should be programmed to minimum of 8.9 * tREFI (to allow
	 * for possible delays from ZQ or isoc) and tRASmax (70us) divided by 1024.
	 */
	return (union tc_rftp_reg) {
		.tREFI   = ctrl->tREFI,
		.tRFC    = ctrl->tRFC,
		.tREFIx9 = ctrl->tREFI * 89 / 10240,
	};
}

static union tc_srftp_reg make_tc_srftp(const struct sysinfo *const ctrl)
{
	return (union tc_srftp_reg) {
		.tXSDLL     = tDLLK,
		.tXS_offset = get_tXS_offset(ctrl->mem_clock_mhz),
		.tZQOPER    = get_tZQOPER(ctrl->mem_clock_mhz, ctrl->lpddr),
		.tMOD       = get_tMOD(ctrl->mem_clock_mhz) - 8,
	};
}

void configure_timings(struct sysinfo *ctrl)
{
	if (ctrl->lpddr)
		die("%s: Missing support for LPDDR\n", __func__);

	const uint8_t odt_stretch = get_odt_stretch(ctrl);
	const union tc_bank_reg tc_bank = make_tc_bank(ctrl);
	const union tc_bank_rank_a_reg tc_bank_rank_a = make_tc_bankrank_a(ctrl, odt_stretch);
	const union tc_bank_rank_b_reg tc_bank_rank_b = make_tc_bankrank_b(ctrl);
	const union tc_bank_rank_c_reg tc_bank_rank_c = make_tc_bankrank_c(ctrl, odt_stretch);
	const union tc_bank_rank_d_reg tc_bank_rank_d = make_tc_bankrank_d(ctrl, odt_stretch);

	const uint8_t wr_delay = tc_bank_rank_b.dec_wrd + 1;
	uint8_t sc_wr_add_delay = 0;
	sc_wr_add_delay |= wr_delay << 0;
	sc_wr_add_delay |= wr_delay << 2;
	sc_wr_add_delay |= wr_delay << 4;
	sc_wr_add_delay |= wr_delay << 6;

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		ctrl->tc_bank[channel] = tc_bank;
		ctrl->tc_bankrank_a[channel] = tc_bank_rank_a;
		ctrl->tc_bankrank_b[channel] = tc_bank_rank_b;
		ctrl->tc_bankrank_c[channel] = tc_bank_rank_c;
		ctrl->tc_bankrank_d[channel] = tc_bank_rank_d;

		mchbar_write32(TC_BANK_ch(channel), ctrl->tc_bank[channel].raw);
		mchbar_write32(TC_BANK_RANK_A_ch(channel), ctrl->tc_bankrank_a[channel].raw);
		mchbar_write32(TC_BANK_RANK_B_ch(channel), ctrl->tc_bankrank_b[channel].raw);
		mchbar_write32(TC_BANK_RANK_C_ch(channel), ctrl->tc_bankrank_c[channel].raw);
		mchbar_write32(TC_BANK_RANK_D_ch(channel), ctrl->tc_bankrank_d[channel].raw);
		mchbar_write8(SC_WR_ADD_DELAY_ch(channel), sc_wr_add_delay);
	}
}

void configure_refresh(struct sysinfo *ctrl)
{
	const union tc_srftp_reg tc_srftp = make_tc_srftp(ctrl);
	const union tc_rftp_reg  tc_rftp  = make_tc_rftp(ctrl);
	const uint32_t tc_zqcal = make_tc_zqcal(ctrl);

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		mchbar_setbits32(TC_RFP_ch(channel), 0xff);
		mchbar_write32(TC_RFTP_ch(channel),  tc_rftp.raw);
		mchbar_write32(TC_SRFTP_ch(channel), tc_srftp.raw);
		mchbar_write32(TC_ZQCAL_ch(channel), tc_zqcal);
	}
}
