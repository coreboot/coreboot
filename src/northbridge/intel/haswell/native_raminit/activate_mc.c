/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>
#include <types.h>

#include "raminit_native.h"

static void update_internal_clocks_on(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		bool clocks_on = false;
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			const union ddr_data_control_1_reg data_control_1 = {
				.raw = ctrl->dq_control_1[channel][byte],
			};
			const int8_t o_on = data_control_1.odt_delay;
			const int8_t s_on = data_control_1.sense_amp_delay;
			const int8_t o_off = data_control_1.odt_duration;
			const int8_t s_off = data_control_1.sense_amp_duration;
			if (o_on + o_off >= 7 || s_on + s_off >= 7) {
				clocks_on = true;
				break;
			}
		}
		union ddr_data_control_0_reg data_control_0 = {
			.raw = ctrl->dq_control_0[channel],
		};
		data_control_0.internal_clocks_on = clocks_on;
		ctrl->dq_control_0[channel] = data_control_0.raw;
		mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
	}
}

/* Switch off unused segments of the SDLL to save power */
static void update_sdll_length(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			uint8_t max_pi = 0;
			for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
				if (!rank_in_ch(ctrl, rank, channel))
					continue;

				const uint8_t rx_dqs_p = ctrl->rxdqsp[channel][rank][byte];
				const uint8_t rx_dqs_n = ctrl->rxdqsn[channel][rank][byte];
				max_pi = MAX(max_pi, MAX(rx_dqs_p, rx_dqs_n));
			}
			/* Update SDLL length for power savings */
			union ddr_data_control_1_reg data_control_1 = {
				.raw = ctrl->dq_control_1[channel][byte],
			};
			/* Calculate which segments to turn off */
			data_control_1.sdll_segment_disable = (7 - (max_pi >> 3)) & ~1;
			ctrl->dq_control_1[channel][byte] = data_control_1.raw;
			mchbar_write32(DQ_CONTROL_1(channel, byte), data_control_1.raw);
		}
	}
}

static void set_rx_clk_stg_num(struct sysinfo *ctrl, const uint8_t channel)
{
	const uint8_t rcven_drift = ctrl->lpddr ? DIV_ROUND_UP(tDQSCK_DRIFT, ctrl->qclkps) : 1;
	uint8_t max_rcven = 0;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!rank_in_ch(ctrl, rank, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
			max_rcven = MAX(max_rcven, ctrl->rcven[channel][rank][byte] / 64);
	}
	const union ddr_data_control_1_reg ddr_data_control_1 = {
		.raw = ctrl->dq_control_1[channel][0],
	};
	const bool lpddr_long_odt = ddr_data_control_1.lpddr_long_odt_en;
	const uint8_t rcven_turnoff = max_rcven + 18 + 2 * rcven_drift + lpddr_long_odt;
	const union ddr_data_control_0_reg ddr_data_control_0 = {
		.raw = ctrl->dq_control_0[channel],
	};
	for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
		union ddr_data_control_2_reg ddr_data_control_2 = {
			.raw = ctrl->dq_control_2[channel][byte],
		};
		if (ddr_data_control_0.odt_samp_extend_en) {
			if (ddr_data_control_2.rx_clk_stg_num < rcven_turnoff)
				ddr_data_control_2.rx_clk_stg_num = rcven_turnoff;
		} else {
			const int8_t o_on = ddr_data_control_1.odt_delay;
			const int8_t o_off = ddr_data_control_1.odt_duration;
			ddr_data_control_2.rx_clk_stg_num = MAX(17, o_on + o_off + 14);
		}
		ctrl->dq_control_2[channel][byte] = ddr_data_control_2.raw;
		mchbar_write32(DQ_CONTROL_2(channel, byte), ddr_data_control_2.raw);
	}
}

#define SELF_REFRESH_IDLE_COUNT 0x200

static void enter_sr(void)
{
	mchbar_write32(PM_SREF_CONFIG, SELF_REFRESH_IDLE_COUNT | BIT(16));
	udelay(1);
}

enum power_down_mode {
	PDM_NO_PD	= 0,
	PDM_APD		= 1,
	PDM_PPD		= 2,
	PDM_PPD_DLL_OFF	= 6,
};

static void power_down_config(struct sysinfo *ctrl)
{
	const enum power_down_mode pd_mode = ctrl->lpddr ? PDM_PPD : PDM_PPD_DLL_OFF;
	mchbar_write32(PM_PDWN_CONFIG, pd_mode << 12 | 0x40);
}

static void train_power_modes_post(struct sysinfo *ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		/* Adjust tCPDED and tPRPDEN */
		if (ctrl->mem_clock_mhz >= 933)
			ctrl->tc_bankrank_d[channel].tCPDED = 2;

		if (ctrl->mem_clock_mhz >= 1066)
			ctrl->tc_bankrank_d[channel].tPRPDEN = 2;

		mchbar_write32(TC_BANK_RANK_D_ch(channel), ctrl->tc_bankrank_d[channel].raw);
	}
	power_down_config(ctrl);
	mchbar_write32(MCDECS_CBIT, BIT(30));	/* dis_msg_clk_gate */
}

static uint8_t compute_burst_end_odt_delay(const struct sysinfo *const ctrl)
{
	/* Must be disabled for LPDDR */
	if (ctrl->lpddr)
		return 0;

	const uint8_t beod = MIN(7, DIV_ROUND_CLOSEST(14300 * 20 / 100, ctrl->qclkps));
	if (beod < 3)
		return 0;

	if (beod < 4)
		return 4;

	return beod;
}

static void program_burst_end_odt_delay(struct sysinfo *ctrl)
{
	/* Program burst_end_odt_delay - it should be zero during training steps */
	const uint8_t beod = compute_burst_end_odt_delay(ctrl);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			union ddr_data_control_1_reg ddr_data_control_1 = {
				.raw = ctrl->dq_control_1[channel][byte],
			};
			ddr_data_control_1.burst_end_odt_delay = beod;
			ctrl->dq_control_1[channel][byte] = ddr_data_control_1.raw;
			mchbar_write32(DQ_CONTROL_1(channel, byte), ddr_data_control_1.raw);
		}
	}
}

/*
 * Return a random value to use for scrambler seeds. Try to use RDRAND
 * first and fall back to hardcoded values if RDRAND does not succeed.
 */
static uint16_t get_random_number(const uint8_t channel)
{
	/* The RDRAND instruction is only available 100k cycles after reset */
	for (size_t i = 0; i < 100000; i++) {
		uint32_t status;
		uint32_t random;
		/** TODO: Clean up asm **/
		__asm__ __volatile__(
			"\n\t .byte 0x0F, 0xC7, 0xF0"
			"\n\t movl %%eax, %0"
			"\n\t pushf"
			"\n\t pop %%eax"
			"\n\t movl %%eax, %1"
			: "=m"(random),
			  "=m"(status)
			: /* No inputs */
			: "eax", "cc");

		/* Only consider non-zero random values as valid */
		if (status & 1 && random)
			return random;
	}

	/* https://xkcd.com/221 */
	if (channel)
		return 0x28f4;
	else
		return 0x893e;
}

/* Work around "error: 'typeof' applied to a bit-field" */
static inline uint32_t max(const uint32_t a, const uint32_t b)
{
	return MAX(a, b);
}

enum raminit_status activate_mc(struct sysinfo *ctrl)
{
	const bool enable_scrambling = true;
	const bool enable_cmd_tristate = true;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		if (enable_scrambling && ctrl->stepping < STEPPING_C0) {
			/* Make sure tRDRD_(sr, dr, dd) are at least 6 for scrambler W/A */
			union tc_bank_rank_a_reg tc_bank_rank_a = {
				.raw = mchbar_read32(TC_BANK_RANK_A_ch(channel)),
			};
			tc_bank_rank_a.tRDRD_sr = max(tc_bank_rank_a.tRDRD_sr, 6);
			tc_bank_rank_a.tRDRD_dr = max(tc_bank_rank_a.tRDRD_dr, 6);
			tc_bank_rank_a.tRDRD_dd = max(tc_bank_rank_a.tRDRD_dd, 6);
			mchbar_write32(TC_BANK_RANK_A_ch(channel), tc_bank_rank_a.raw);
		}
		if (enable_scrambling) {
			const union ddr_scramble_reg ddr_scramble = {
				.scram_key = get_random_number(channel),
				.scram_en  = 1,
			};
			mchbar_write32(DDR_SCRAMBLE_ch(channel), ddr_scramble.raw);
		}
		if (ctrl->tCMD == 1) {
			/* If we are in 1N mode, enable and set command rate limit to 3 */
			union mcmain_command_rate_limit_reg cmd_rate_limit = {
				.raw = mchbar_read32(COMMAND_RATE_LIMIT_ch(channel)),
			};
			cmd_rate_limit.enable_cmd_limit = 1;
			cmd_rate_limit.cmd_rate_limit   = 3;
			mchbar_write32(COMMAND_RATE_LIMIT_ch(channel), cmd_rate_limit.raw);
		}
		if (enable_cmd_tristate) {
			/* Enable command tri-state at the end of training */
			union tc_bank_rank_a_reg tc_bank_rank_a = {
				.raw = mchbar_read32(TC_BANK_RANK_A_ch(channel)),
			};
			tc_bank_rank_a.cmd_3st_dis = 0;
			mchbar_write32(TC_BANK_RANK_A_ch(channel), tc_bank_rank_a.raw);
		}
		/* Set MC to normal mode and clean the ODT and CKE */
		mchbar_write32(REUT_ch_SEQ_CFG(channel), REUT_MODE_NOP << 12);
		/* Set again the rank occupancy */
		mchbar_write8(MC_INIT_STATE_ch(channel), ctrl->rankmap[channel]);
		if (ctrl->is_ecc) {
			/* Enable ECC I/O and logic */
			union mad_dimm_reg mad_dimm = {
				.raw = mchbar_read32(MAD_DIMM(channel)),
			};
			mad_dimm.ecc_mode = 3;
			mchbar_write32(MAD_DIMM(channel), mad_dimm.raw);
		}
	}

	if (!is_hsw_ult())
		update_internal_clocks_on(ctrl);

	update_sdll_length(ctrl);

	program_burst_end_odt_delay(ctrl);

	if (is_hsw_ult()) {
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			set_rx_clk_stg_num(ctrl, channel);
		}
		/** TODO: Program DDRPL_CR_DDR_TX_DELAY if Memory Trace is enabled **/
	}

	/* Enable periodic COMP */
	mchbar_write32(M_COMP, (union pcu_comp_reg) {
		.comp_interval = COMP_INT,
	}.raw);

	/* Enable the power mode before PCU starts working */
	train_power_modes_post(ctrl);

	/* Set idle timer and self refresh enable bits */
	enter_sr();

	/** FIXME: Do not hardcode power weights and RAPL settings **/
	mchbar_write32(0x5888, 0x00000d0d);
	mchbar_write32(0x5884, 0x00000004);	/* 58.2 pJ */

	mchbar_write32(0x58e0, 0);
	mchbar_write32(0x58e4, 0);

	mchbar_write32(0x5890, 0xffff);
	mchbar_write32(0x5894, 0xffff);
	mchbar_write32(0x5898, 0xffff);
	mchbar_write32(0x589c, 0xffff);
	mchbar_write32(0x58d0, 0xffff);
	mchbar_write32(0x58d4, 0xffff);
	mchbar_write32(0x58d8, 0xffff);
	mchbar_write32(0x58dc, 0xffff);

	/* Overwrite thermal parameters */
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write32(_MCMAIN_C(0x42ec, channel), 0x0000000f);
		mchbar_write32(_MCMAIN_C(0x42f0, channel), 0x00000009);
		mchbar_write32(_MCMAIN_C(0x42f4, channel), 0x00000093);
		mchbar_write32(_MCMAIN_C(0x42f8, channel), 0x00000087);
		mchbar_write32(_MCMAIN_C(0x42fc, channel), 0x000000de);

		/** TODO: Differs for LPDDR **/
		mchbar_write32(PM_THRT_CKE_MIN_ch(channel), 0x30);
	}
	mchbar_write32(PCU_DDR_PTM_CTL, 0x40);
	return RAMINIT_STATUS_SUCCESS;
}

static void mc_lockdown(void)
{
	/* Lock memory controller registers */
	mchbar_write32(MC_LOCK, 0x8f);

	/* MPCOHTRK_GDXC_OCLA_ADDRESS_HI_LOCK is set when programming the memory map */

	/* Lock memory map registers */
	pci_or_config16(HOST_BRIDGE, GGC,         1 <<  0);
	pci_or_config32(HOST_BRIDGE, DPR,         1 <<  0);
	pci_or_config32(HOST_BRIDGE, MESEG_LIMIT, 1 << 10);
	pci_or_config32(HOST_BRIDGE, REMAPBASE,   1 <<  0);
	pci_or_config32(HOST_BRIDGE, REMAPLIMIT,  1 <<  0);
	pci_or_config32(HOST_BRIDGE, TOM,         1 <<  0);
	pci_or_config32(HOST_BRIDGE, TOUUD,       1 <<  0);
	pci_or_config32(HOST_BRIDGE, BDSM,        1 <<  0);
	pci_or_config32(HOST_BRIDGE, BGSM,        1 <<  0);
	pci_or_config32(HOST_BRIDGE, TOLUD,       1 <<  0);
}

enum raminit_status raminit_done(struct sysinfo *ctrl)
{
	union mc_init_state_g_reg mc_init_state_g = {
		.raw = mchbar_read32(MC_INIT_STATE_G),
	};
	mc_init_state_g.refresh_enable = 1;
	mc_init_state_g.pu_mrc_done    = 1;
	mc_init_state_g.mrc_done       = 1;
	mchbar_write32(MC_INIT_STATE_G, mc_init_state_g.raw);

	/* Lock the memory controller to enable normal operation */
	mc_lockdown();

	/* Poll for mc_init_done_ack to make sure memory initialization is complete */
	printk(BIOS_DEBUG, "Waiting for mc_init_done acknowledgement... ");

	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 2000);
	do {
		mc_init_state_g.raw = mchbar_read32(MC_INIT_STATE_G);

		/* DRAM will NOT work without the acknowledgement. There is no hope. */
		if (stopwatch_expired(&timer))
			die("\nTimed out waiting for mc_init_done acknowledgement\n");

	} while (mc_init_state_g.mc_init_done_ack == 0);
	printk(BIOS_DEBUG, "DONE!\n");

	/* Provide some data for the graphics driver. Yes, it's hardcoded. */
	mchbar_write32(SSKPD + 0, 0x05a2404f);
	mchbar_write32(SSKPD + 4, 0x140000a0);
	return RAMINIT_STATUS_SUCCESS;
}
