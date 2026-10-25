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
