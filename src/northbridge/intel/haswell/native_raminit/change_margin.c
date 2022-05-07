/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>

#include "raminit_native.h"

void update_rxt(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rank,
	const uint8_t byte,
	const enum rxt_subfield subfield,
	const int32_t value)
{
	union ddr_data_rx_train_rank_reg rxt = {
		.rcven =  ctrl->rcven[channel][rank][byte],
		.dqs_p = ctrl->rxdqsp[channel][rank][byte],
		.rx_eq =  ctrl->rx_eq[channel][rank][byte],
		.dqs_n = ctrl->rxdqsn[channel][rank][byte],
		.vref  = ctrl->rxvref[channel][rank][byte],
	};
	int32_t new_value;
	switch (subfield) {
	case RXT_RCVEN:
		new_value = clamp_s32(0, value, 511);
		rxt.rcven = new_value;
		break;
	case RXT_RXDQS_P:
		new_value = clamp_s32(0, value, 63);
		rxt.dqs_p = new_value;
		break;
	case RXT_RX_EQ:
		new_value = clamp_s32(0, value, 31);
		rxt.rx_eq = new_value;
		break;
	case RXT_RXDQS_N:
		new_value = clamp_s32(0, value, 63);
		rxt.dqs_n = new_value;
		break;
	case RXT_RX_VREF:
		new_value = clamp_s32(-32, value, 31);
		rxt.vref = new_value;
		break;
	case RXT_RXDQS_BOTH:
		new_value = clamp_s32(0, value, 63);
		rxt.dqs_p = new_value;
		rxt.dqs_n = new_value;
		break;
	case RXT_RESTORE:
		new_value = value;
		break;
	default:
		die("%s: Unhandled subfield index %u\n", __func__, subfield);
	}

	if (new_value != value) {
		printk(BIOS_ERR, "%s: Overflow for subfield %u: %d ---> %d\n",
			__func__, subfield, value, new_value);
	}
	mchbar_write32(RX_TRAIN_ch_r_b(channel, rank, byte), rxt.raw);
	download_regfile(ctrl, channel, false, rank, REG_FILE_USE_RANK, byte, true, false);
}

void update_txt(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t rank,
	const uint8_t byte,
	const enum txt_subfield subfield,
	const int32_t value)
{
	union ddr_data_tx_train_rank_reg txt = {
		.dq_delay  = ctrl->tx_dq[channel][rank][byte],
		.dqs_delay = ctrl->txdqs[channel][rank][byte],
		.tx_eq     = ctrl->tx_eq[channel][rank][byte],
	};
	int32_t new_value;
	switch (subfield) {
	case TXT_TX_DQ:
		new_value = clamp_s32(0, value, 511);
		txt.dq_delay = new_value;
		break;
	case TXT_TXDQS:
		new_value = clamp_s32(0, value, 511);
		txt.dqs_delay = new_value;
		break;
	case TXT_TX_EQ:
		new_value = clamp_s32(0, value, 63);
		txt.tx_eq = new_value;
		break;
	case TXT_DQDQS_OFF:
		new_value = value;
		txt.dqs_delay += new_value;
		txt.dq_delay  += new_value;
		break;
	case TXT_RESTORE:
		new_value = value;
		break;
	default:
		die("%s: Unhandled subfield index %u\n", __func__, subfield);
	}
	if (new_value != value) {
		printk(BIOS_ERR, "%s: Overflow for subfield %u: %d ---> %d\n",
			__func__, subfield, value, new_value);
	}
	mchbar_write32(TX_TRAIN_ch_r_b(channel, rank, byte), txt.raw);
	download_regfile(ctrl, channel, false, rank, REG_FILE_USE_RANK, byte, false, true);
}

void download_regfile(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const bool multicast,
	const uint8_t rank,
	const enum regfile_mode regfile,
	const uint8_t byte,
	const bool read_rf_rd,
	const bool read_rf_wr)
{
	union reut_seq_base_addr_reg reut_seq_base_addr;
	switch (regfile) {
	case REG_FILE_USE_START:
		reut_seq_base_addr.raw = mchbar_read64(REUT_ch_SEQ_ADDR_START(channel));
		break;
	case REG_FILE_USE_CURRENT:
		reut_seq_base_addr.raw = mchbar_read64(REUT_ch_SEQ_ADDR_CURRENT(channel));
		break;
	case REG_FILE_USE_RANK:
		reut_seq_base_addr.raw = 0;
		if (rank >= NUM_SLOTRANKS)
			die("%s: bad rank %u\n", __func__, rank);
		break;
	default:
		die("%s: Invalid regfile param %u\n", __func__, regfile);
	}
	uint8_t phys_rank = rank;
	if (reut_seq_base_addr.raw != 0) {
		/* Map REUT logical rank to physical rank */
		const uint32_t log_to_phys = mchbar_read32(REUT_ch_RANK_LOG_TO_PHYS(channel));
		phys_rank = log_to_phys >> (reut_seq_base_addr.rank_addr * 4) & 0x3;
	}
	uint32_t reg = multicast ? DDR_DATA_ch_CONTROL_0(channel) : DQ_CONTROL_0(channel, byte);
	union ddr_data_control_0_reg ddr_data_control_0 = {
		.raw = mchbar_read32(reg),
	};
	ddr_data_control_0.read_rf_rd   = read_rf_rd;
	ddr_data_control_0.read_rf_wr   = read_rf_wr;
	ddr_data_control_0.read_rf_rank = phys_rank;
	mchbar_write32(reg, ddr_data_control_0.raw);
}
