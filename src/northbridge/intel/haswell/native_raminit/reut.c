/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>
#include <types.h>

#include "raminit_native.h"

enum {
	CADB_CMD_MRS = 0,
	CADB_CMD_REF = 1,
	CADB_CMD_PRE = 2,
	CADB_CMD_ACT = 3,
	CADB_CMD_WR  = 4,
	CADB_CMD_RD  = 5,
	CADB_CMD_ZQ  = 6,
	CADB_CMD_NOP = 7,
};

/*
 * DDR3 rank mirror swaps the following pins: A3<->A4, A5<->A6, A7<->A8, BA0<->BA1
 *
 * Note that the swapped bits are contiguous. We can use some XOR magic to swap the bits.
 * Address lanes are at bits 0..15 and bank selects are at bits 24..26 on the REUT register.
 */
#define MIRROR_BITS	(BIT(24) | BIT(7) | BIT(5) | BIT(3))
static uint64_t cadb_prog_rank_mirror(const uint64_t cadb_prog)
{
	/* First XOR: find which pairs of bits are different (need swapping) */
	const uint64_t tmp64 = (cadb_prog ^ (cadb_prog >> 1)) & MIRROR_BITS;

	/* Second XOR: invert the pairs of bits that have different values */
	return cadb_prog ^ (tmp64 | tmp64 << 1);
}

static enum raminit_status reut_write_cadb_cmd(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t cmd,
	const uint8_t bank,
	const uint16_t valarr[NUM_SLOTRANKS],
	const uint8_t delay)
{
	union mcscheds_dft_misc_reg dft_misc = {
		.raw = mchbar_read32(MCSCHEDS_DFT_MISC),
	};
	dft_misc.ddr_qualifier = 0;
	mchbar_write32(MCSCHEDS_DFT_MISC, dft_misc.raw);

	/* Pointer will be dynamically incremented after a write to CADB_PROG register */
	mchbar_write8(REUT_ch_PAT_CADB_WRITE_PTR(channel), 0);

	uint8_t count = 0;
	for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
		if (!(ctrl->rankmap[channel] & BIT(rank) & rankmask))
			continue;

		union reut_pat_cadb_prog_reg reut_cadb_prog = {
			.addr = valarr[rank],
			.bank = bank,
			.cs   = ~BIT(rank), /* CS is active low */
			.cmd  = cmd,
			.cke  = 0xf,
		};
		if (ctrl->rank_mirrored[channel] & BIT(rank))
			reut_cadb_prog.raw = cadb_prog_rank_mirror(reut_cadb_prog.raw);

		mchbar_write64(REUT_ch_PAT_CADB_PROG(channel), reut_cadb_prog.raw);
		count++;
	}
	if (!count) {
		printk(BIOS_ERR, "%s: rankmask is invalid\n", __func__);
		return RAMINIT_STATUS_UNSPECIFIED_ERROR;	/** FIXME: Is this needed? **/
	}
	const union reut_pat_cadb_mrs_reg reut_cadb_mrs = {
		.delay_gap = delay ? delay : 3,
		.end_ptr   = count - 1,
	};
	mchbar_write32(REUT_ch_PAT_CADB_MRS(channel), reut_cadb_mrs.raw);

	const uint32_t reut_seq_cfg_save = mchbar_read32(REUT_ch_SEQ_CFG(channel));
	union reut_seq_cfg_reg reut_seq_cfg = {
		.raw = reut_seq_cfg_save,
	};
	reut_seq_cfg.global_control = 0;
	reut_seq_cfg.initialization_mode = REUT_MODE_MRS;
	mchbar_write32(REUT_ch_SEQ_CFG(channel), reut_seq_cfg.raw);
	mchbar_write32(REUT_ch_SEQ_CTL(channel), (union reut_seq_ctl_reg) {
		.start_test   = 1,
		.clear_errors = 1,
	}.raw);
	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	union reut_global_err_reg reut_global_err;
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 100);
	do {
		reut_global_err.raw = mchbar_read32(REUT_GLOBAL_ERR);
		if (reut_global_err.ch_error & BIT(channel)) {
			printk(BIOS_ERR, "Unexpected REUT error for channel %u\n", channel);
			status = RAMINIT_STATUS_REUT_ERROR;
			break;
		}
		if (stopwatch_expired(&timer)) {
			printk(BIOS_ERR, "%s: REUT timed out!\n", __func__);
			status = RAMINIT_STATUS_POLL_TIMEOUT;
			break;
		}
	} while (!(reut_global_err.ch_test_done & BIT(channel)));
	mchbar_write32(REUT_ch_SEQ_CTL(channel), (union reut_seq_ctl_reg) {
		.clear_errors = 1,
	}.raw);
	mchbar_write32(REUT_ch_SEQ_CFG(channel), reut_seq_cfg_save);
	return status;
}

static enum raminit_status reut_write_cadb_cmd_all(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t cmd,
	const uint8_t bank,
	const uint16_t val,
	const uint8_t delay)
{
	const uint16_t valarr[NUM_SLOTRANKS] = { val, val, val, val };
	return reut_write_cadb_cmd(ctrl, channel, rankmask, cmd, bank, valarr, delay);
}

void reut_issue_mrs(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rankmask,
	const uint8_t mr,
	const uint16_t val)
{
	reut_write_cadb_cmd_all(ctrl, channel, rankmask, CADB_CMD_MRS, mr, val, 0);
}

void reut_issue_mrs_all(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t mr,
	const uint16_t val[NUM_SLOTS])
{
	const uint16_t valarr[NUM_SLOTRANKS] = { val[0], val[0], val[1], val[1] };
	reut_write_cadb_cmd(ctrl, channel, 0xf, CADB_CMD_MRS, mr, valarr, 0);
}

enum raminit_status reut_issue_zq(struct sysinfo *ctrl, uint8_t chanmask, uint8_t zq_type)
{
	/** TODO: Issuing ZQ commands differs for LPDDR **/
	if (ctrl->lpddr)
		die("%s: LPDDR not yet supported in ZQ calibration\n", __func__);

	__maybe_unused uint8_t opcode; /* NOTE: Only used for LPDDR */
	uint16_t zq = 0;
	switch (zq_type) {
	case ZQ_INIT:
		zq = BIT(10);
		opcode = 0xff;
		break;
	case ZQ_LONG:
		zq = BIT(10);
		opcode = 0xab;
		break;
	case ZQ_SHORT:
		opcode = 0x56;
		break;
	case ZQ_RESET:
		opcode = 0xc3;
		break;
	default:
		die("%s: ZQ type %u is invalid\n", __func__, zq_type);
	}

	/* ZQCS on single-channel needs a longer delay */
	const uint8_t delay = zq_type == ZQ_SHORT && (!ctrl->dpc[0] || !ctrl->dpc[1]) ? 7 : 1;
	enum raminit_status status = RAMINIT_STATUS_SUCCESS;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!(BIT(channel) & chanmask) || !does_ch_exist(ctrl, channel))
			continue;

		status = reut_write_cadb_cmd_all(ctrl, channel, 0xf, CADB_CMD_ZQ, 0, zq, delay);
		if (status)
			break;
	}

	/* Wait a bit after ZQ INIT and ZQCL commands */
	if (zq)
		udelay(1);

	return status;
}
