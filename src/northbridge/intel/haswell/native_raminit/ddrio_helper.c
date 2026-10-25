/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "raminit_native.h"

uint16_t ddrio_offset(
	const struct sysinfo *ctrl,
	const enum ddrio_reg reg,
	const uint8_t channel,
	const uint8_t byte)
{
	uint16_t offset = reg;
	if (channel >= NUM_CHANNELS) {
		offset = _DDRIO_C_R_B(reg, 0, 0, 0) + _BROADCAST_BASE;
	} else if (byte >= NUM_LANES) {
		offset = _DDRIO_C_R_B(reg, channel, 0, 0) + _PER_CH_BASE;
	} else {
		offset = _DDRIO_C_R_B(reg, channel, 0, byte);
	}
	if (ctrl->bdw)
		offset += 0xc;

	return offset;
}

void set_rx_offset_vdq_bcast(const struct sysinfo *ctrl, const uint32_t value)
{
	if (ctrl->bdw) {
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++)
			mchbar_write32(DDR_DATA_RX_OFFSET_VDQ_BDW(rank), value);
	} else {
		mchbar_write32(DDR_DATA_RX_OFFSET_VDQ_HSW, value);
	}
}

void set_rx_offset_vdq(
	const struct sysinfo *ctrl,
	const uint8_t channel,
	const uint8_t byte,
	const uint32_t value)
{
	if (ctrl->bdw) {
		for (uint8_t rank = 0; rank < NUM_SLOTRANKS; rank++)
			mchbar_write32(RX_OFFSET_VDQ_BDW(channel, rank, byte), value);
	} else {
		mchbar_write32(RX_OFFSET_VDQ_HSW(channel, byte), value);
	}
}
