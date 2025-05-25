/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
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

static void update_data_offset_train(
	struct sysinfo *ctrl,
	const uint8_t param,
	const uint8_t en_multicast,
	const uint8_t channel_in,
	const uint8_t rank,
	const uint8_t byte_in,
	const bool update_ctrl,
	const enum regfile_mode regfile,
	const uint32_t value)
{
	bool is_rd = false;
	bool is_wr = false;
	switch (param) {
	case RdT:
	case RdV:
	case RcvEna:
		is_rd = true;
		break;
	case WrT:
	case WrDqsT:
		is_wr = true;
		break;
	default:
		die("%s: Invalid margin parameter %u\n", __func__, param);
	}
	if (en_multicast) {
		mchbar_write32(DDR_DATA_OFFSET_TRAIN, value);
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			download_regfile(ctrl, channel, true, rank, regfile, 0, is_rd, is_wr);
			if (update_ctrl) {
				for (uint8_t byte = 0; byte < ctrl->lanes; byte++)
					ctrl->data_offset_train[channel][byte] = value;
			}
		}
	} else {
		mchbar_write32(DDR_DATA_OFFSET_TRAIN_ch_b(channel_in, byte_in), value);
		download_regfile(ctrl, channel_in, false, rank, regfile, byte_in, is_rd, is_wr);
		if (update_ctrl)
			ctrl->data_offset_train[channel_in][byte_in] = value;
	}
}

static uint32_t get_max_margin(const enum margin_parameter param)
{
	switch (param) {
	case RcvEna:
	case RdT:
	case WrT:
	case WrDqsT:
		return MAX_POSSIBLE_TIME;
	case RdV:
		return MAX_POSSIBLE_VREF;
	default:
		die("%s: Invalid margin parameter %u\n", __func__, param);
	}
}

void change_margin(
	struct sysinfo *ctrl,
	const enum margin_parameter param,
	const int32_t value0,
	const bool en_multicast,
	const uint8_t channel,
	const uint8_t rank,
	const uint8_t byte,
	const bool update_ctrl,
	const enum regfile_mode regfile)
{
	/** FIXME: Remove this **/
	if (rank == 0xff)
		die("%s: rank is 0xff\n", __func__);

	if (!en_multicast && !does_ch_exist(ctrl, channel))
		die("%s: Tried to change margin of empty channel %u\n", __func__, channel);

	const uint32_t max_value = get_max_margin(param);
	const int32_t v0 = clamp_s32(-max_value, value0, max_value);

	union ddr_data_offset_train_reg ddr_data_offset_train = {
		.raw = en_multicast ? 0 : ctrl->data_offset_train[channel][byte],
	};
	bool update_offset_train = false;
	switch (param) {
	case RcvEna:
		ddr_data_offset_train.rcven = v0;
		update_offset_train = true;
		break;
	case RdT:
		ddr_data_offset_train.rx_dqs = v0;
		update_offset_train = true;
		break;
	case WrT:
		ddr_data_offset_train.tx_dq = v0;
		update_offset_train = true;
		break;
	case WrDqsT:
		ddr_data_offset_train.tx_dqs = v0;
		update_offset_train = true;
		break;
	case RdV:
		ddr_data_offset_train.vref = v0;
		update_offset_train = true;
		break;
	default:
		die("%s: Invalid margin parameter %u\n", __func__, param);
	}
	if (update_offset_train) {
		update_data_offset_train(
			ctrl,
			param,
			en_multicast,
			channel,
			rank,
			byte,
			update_ctrl,
			regfile,
			ddr_data_offset_train.raw);
	}
}

void change_1d_margin_multicast(
	struct sysinfo *ctrl,
	const enum margin_parameter param,
	const int32_t value0,
	const uint8_t rank,
	const bool update_ctrl,
	const enum regfile_mode regfile)
{
	change_margin(ctrl, param, value0, true, 0, rank, 0, update_ctrl, regfile);
}

uint32_t update_comp_global_offset(
	struct sysinfo *ctrl,
	const enum global_comp_offset param,
	const int32_t offset,
	const uint8_t update_ctrl)
{
	union ddr_comp_ctl_0_reg ddr_comp_ctl_0 = ctrl->comp_ctl_0;
	union ddr_comp_ctl_1_reg ddr_comp_ctl_1 = ctrl->comp_ctl_1;
	switch (param) {
	case RdOdt:
		/* Disable fixed ODT offset before changing this param */
		ddr_comp_ctl_0.fixed_odt_offset = 0;
		ddr_comp_ctl_0.dq_odt_vref = offset;
		break;
	case WrDS:
		ddr_comp_ctl_0.dq_drv_vref = offset;
		break;
	case WrDSCmd:
		ddr_comp_ctl_0.cmd_drv_vref = offset;
		break;
	case WrDSCtl:
		ddr_comp_ctl_0.ctl_drv_vref = offset;
		break;
	case WrDSClk:
		ddr_comp_ctl_0.clk_drv_vref = offset;
		break;
	case SCompDq:
		ddr_comp_ctl_1.dq_scomp = offset;
		break;
	case SCompCmd:
		ddr_comp_ctl_1.cmd_scomp = offset;
		break;
	case SCompCtl:
		ddr_comp_ctl_1.ctl_scomp = offset;
		break;
	case SCompClk:
		ddr_comp_ctl_1.clk_scomp = offset;
		break;
	case DisOdtStatic:
		ctrl->comp_ctl_0.disable_odt_static = !!offset;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			union ddr_data_control_0_reg data_control_0 = {
				.raw = ctrl->dq_control_0[channel],
			};
			data_control_0.disable_odt_static = !!offset;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel), data_control_0.raw);
			if (update_ctrl) {
				ctrl->dq_control_0[channel] = data_control_0.raw;
			}
		}
		break;
	default:
		die("%s: Invalid global comp offset %d\n", __func__, param);
	}
	mchbar_write32(DDR_COMP_CTL_0, ddr_comp_ctl_0.raw);
	mchbar_write32(DDR_COMP_CTL_1, ddr_comp_ctl_1.raw);
	if (update_ctrl) {
		ctrl->comp_ctl_0 = ddr_comp_ctl_0;
		ctrl->comp_ctl_1 = ddr_comp_ctl_1;
	}
	force_rcomp_and_wait_us(8);
	const union ddr_data_comp_1_reg ddr_data_comp_1 = {
		.raw = mchbar_read32(DDR_DATA_COMP_1),
	};
	const uint8_t rcomp_odt_up = ddr_data_comp_1.rcomp_odt_up;
	/* Check if we are close to saturation and try changing the static legs */
	if (param == RdOdt && (rcomp_odt_up < 16 || rcomp_odt_up > 48)) {
		const bool dis_odt_static = rcomp_odt_up < 16;

		/* Host always needs to be up-to-date with the static leg state */
		ddr_comp_ctl_0.disable_odt_static = dis_odt_static;
		for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
			if (!does_ch_exist(ctrl, channel))
				continue;

			union ddr_data_control_0_reg data_control_0 = {
				.raw = ctrl->dq_control_0[channel],
			};
			data_control_0.disable_odt_static = dis_odt_static;
			mchbar_write32(DDR_DATA_ch_CONTROL_0(channel),
					data_control_0.raw);
			ctrl->dq_control_0[channel] = data_control_0.raw;
		}
		mchbar_write32(DDR_COMP_CTL_0, ddr_comp_ctl_0.raw);
		ctrl->comp_ctl_0.disable_odt_static = dis_odt_static;
		force_rcomp_and_wait_us(8);
	}
	switch (param) {
	case RdOdt:
	case DisOdtStatic: {
		/* Re-enable fixed ODT offset after changing this param */
		const uint32_t delta = ddr_data_comp_1.rcomp_odt_down - ddr_data_comp_1.rcomp_odt_up;
		ddr_comp_ctl_0.odt_up_down_off  = delta;
		ddr_comp_ctl_0.fixed_odt_offset = 1;
		mchbar_write32(DDR_COMP_CTL_0, ddr_comp_ctl_0.raw);
		if (update_ctrl)
			ctrl->comp_ctl_0 = ddr_comp_ctl_0;

		return ddr_data_comp_1.rcomp_odt_up;
	}
	case WrDS:
	case SCompDq: {
		const union ddr_data_comp_0_reg data_comp_0 = {
			.raw = mchbar_read32(DDR_DATA_COMP_0),
		};
		return param == WrDS ? data_comp_0.rcomp_drv_up : data_comp_0.slew_rate_comp;
	}
	case WrDSCmd:
	case SCompCmd: {
		const union ddr_comp_cmd_comp_reg cmd_comp = {
			.raw = mchbar_read32(DDR_COMP_CMD_COMP),
		};
		return param == WrDSCmd ? cmd_comp.rcomp_drv_up : cmd_comp.slew_rate_comp;
	}
	case WrDSCtl:
	case SCompCtl: {
		const union ddr_comp_ctl_comp_reg ctl_comp = {
			.raw = mchbar_read32(DDR_COMP_CTL_COMP),
		};
		return param == WrDSCtl ? ctl_comp.rcomp_drv_up : ctl_comp.slew_rate_comp;
	}
	case WrDSClk:
	case SCompClk: {
		const union ddr_comp_clk_comp_reg clk_comp = {
			.raw = mchbar_read32(DDR_COMP_CLK_COMP),
		};
		return param == WrDSClk ? clk_comp.rcomp_drv_up : clk_comp.slew_rate_comp;
	}
	default:
		die("%s: Invalid global comp offset %d\n", __func__, param);
		return 0;
	}
}

static bool is_cpu_comp(const enum opt_param optparam)
{
	switch (optparam) {
	case OptWrDS:
	case OptRdOdt:
	case OptSComp:
	case OptTComp:
		return true;
	default:
		return false;
	}
}

static const uint8_t odt_map[6] = {0, 120, 60, 40, 30, 20};

void update_opt_param_offset(
	struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t ranks_in,
	const uint8_t byte,
	const enum opt_param optparam,
	const int16_t off_in,
	const bool update_ctrl)
{
	const uint8_t rankmask = ranks_in & ctrl->rankmap[channel];
	union ddr_data_offset_comp_reg data_offset_comp = ctrl->data_offset_comp[channel][byte];
	if (optparam == OptDefault) {
		mchbar_write32(DDR_DATA_OFFSET_COMP_ch_b(channel, byte), data_offset_comp.raw);
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask)) {
				continue;
			}
			update_rxt(ctrl, channel, rank, byte, RXT_RESTORE, 0);
			update_txt(ctrl, channel, rank, byte, TXT_RESTORE, 0);
		}
		mchbar_write32(DQ_CONTROL_1(channel, byte), ctrl->dq_control_1[channel][byte]);
		return;
	}
	if (is_cpu_comp(optparam)) {
		int16_t off;
		if (optparam == OptWrDS) {
			off = clamp_s16(-32, off_in, 31);
		} else {
			off = clamp_s16(-16, off_in, 15);
		}
		if (optparam == OptWrDS) {
			data_offset_comp.drv_up   = off;
			data_offset_comp.drv_down = off;
		} else if (optparam == OptRdOdt) {
			data_offset_comp.odt_up   = off;
			data_offset_comp.odt_down = off;
		} else if (optparam == OptTComp) {
			data_offset_comp.t_clk_out = off;
		} else if (optparam == OptSComp) {
			data_offset_comp.slew_rate = off;
		}
		mchbar_write32(DDR_DATA_OFFSET_COMP_ch_b(channel, byte), data_offset_comp.raw);
		if (update_ctrl) {
			ctrl->data_offset_comp[channel][byte] = data_offset_comp;
		}
		union ddr_scram_misc_control_reg misc_control = ctrl->misc_control_0;
		misc_control.force_comp_update = 1;
		mchbar_write32(DDR_SCRAM_MISC_CONTROL, misc_control.raw);
	}
	if (optparam == OptTxEq) {
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask)) {
				continue;
			}
			/*
			 * TxEq[5:4] = Emphasize   = [3, 6, 9, 12] legs
			 * TxEq[3:0] = Deemphasize = [0 .. 11, 4 * Rsvd] legs
			 */
			/* Use 12 emphasis legs (not trained) */
			const uint8_t off_code = clamp_s16(0, off_in, 11) | TXEQFULLDRV;
			update_txt(ctrl, channel, rank, byte, TXT_TX_EQ, off_code);
			if (update_ctrl) {
				ctrl->tx_eq[channel][rank][byte] = off_code;
			}
		}
	}
	if (optparam == OptRxEq) {
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask)) {
				continue;
			}
			/*
			 * RxEQ[4:0] CR Decoding (pF/kOhm)
			 *            [2:0]
			 *  [4:3]     0        1        2        3        4        5-7
			 *     0      0.5/.02  0.5/1.0  0.5/.50  0.5/.25  0.5/.12  rsvd
			 *     1      1.0/.02  1.0/1.0  1.0/.50  1.0/.25  1.0/.12  rsvd
			 *     2      1.5/.02  1.5/1.0  1.5/.50  1.5/.25  1.5/.12  rsvd
			 *     3      2.0/.02  2.0/1.0  2.0/.50  2.0/.25  2.0/.12  rsvd
			 * Sweep = 0-19        [4:3] = (Sweep / 5)  [2:0] = (Sweep % 5)
			 */
			const int16_t off = clamp_s16(0, off_in, 19);
			const uint8_t value = ((off / 5) << 3) + (off % 5);
			update_rxt(ctrl, channel, rank, byte, RXT_RX_EQ, value);
			if (update_ctrl) {
				ctrl->rx_eq[channel][rank][byte] = value;
			}
		}
	}
	if (optparam == OptRxBias) {
		/*
		 * Mapping: 0: 0.44   1: 0.66   2: 0.88   3: 1.00
		 *          4: 1.33   5: 1.66   6: 2.00   7: 2.33
		 */
		union ddr_data_control_1_reg data_control_1 = {
			.raw = ctrl->dq_control_1[channel][byte],
		};
		data_control_1.rx_bias_ctl = clamp_s16(0, off_in, 7);
		mchbar_write32(DQ_CONTROL_1(channel, byte), data_control_1.raw);
		if (update_ctrl) {
			ctrl->dq_control_1[channel][byte] = data_control_1.raw;
		}
	}
	if (optparam == OptDimmRon) {
		if (ctrl->lpddr) {
			die("%s: LPDDR3 support missing\n", __func__);
		}
		/* DIMM Ron: 240/6, 240/7 Ohms */
		const uint16_t ron = !!off_in << 1;
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask)) {
				continue;
			}
			const uint16_t mr1reg = (ctrl->mr1[channel][rank] & ~BIT(1)) | ron;
			reut_issue_mrs(ctrl, channel, BIT(rank), 1, mr1reg);
			if (update_ctrl) {
				ctrl->mr1[channel][rank] = mr1reg;
			}
		}
	}
	if (optparam == OptDimmOdt || optparam == OptDimmOdtWr) {
		if (ctrl->lpddr) {
			die("%s: LPDDR3 support missing\n", __func__);
		}
		const uint16_t rtt_wr_mrs_encoding[] = { 0, 2, 1 };
		const uint16_t rtt_wr_mask = BIT(10) | BIT(9);
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++) {
			if (!rank_in_mask(rank, rankmask)) {
				continue;
			}
			uint8_t index = off_in;
			if (optparam == OptDimmOdt) {
				index >>= 4;
			}
			index = MIN(index, 2);
			const uint16_t rtt_wr = rtt_wr_mrs_encoding[index] << 9;
			uint16_t mr2reg = (ctrl->mr2[channel][rank] & ~rtt_wr_mask) | rtt_wr;
			reut_issue_mrs(ctrl, channel, BIT(rank), 2, mr2reg);
			if (update_ctrl) {
				ctrl->mr2[channel][rank] = mr2reg;
			}
			if (optparam == OptDimmOdtWr) {
				continue;
			}
			index = off_in;
			index &= 0xf;
			if (index >= ARRAY_SIZE(odt_map)) {
				printk(BIOS_ERR, "C%uR%u RTT NOM idx %u is too large\n",
					channel, rank, index);
				index = ARRAY_SIZE(odt_map) - 1;
			}
			const uint16_t rtt_nom = encode_ddr3_rttnom(odt_map[index]);
			uint16_t mr1reg = (ctrl->mr1[channel][rank] & ~RTTNOM_MASK) | rtt_nom;
			reut_issue_mrs(ctrl, channel, BIT(rank), 1, mr1reg);
			if (update_ctrl) {
				ctrl->mr1[channel][rank] = mr1reg;
			}
		}
	}
}
