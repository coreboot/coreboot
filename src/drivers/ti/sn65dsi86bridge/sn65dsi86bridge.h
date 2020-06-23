/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __TI_SN65DSI86BRIDGE_H
#define __TI_SN65DSI86BRIDGE_H

#include <edid.h>

enum dp_pll_clk_src {
	SN65_SEL_12MHZ = 0x0,
	SN65_SEL_19MHZ = 0x1,
	SN65_SEL_26MHZ = 0x2,
	SN65_SEL_27MHZ = 0x3,
	SN65_SEL_38MHZ = 0x4,
};

void sn65dsi86_bridge_init(uint8_t bus, uint8_t chip, enum dp_pll_clk_src ref_clk);
void sn65dsi86_bridge_configure(uint8_t bus, uint8_t chip,
				struct edid *edid, uint32_t num_of_lines,
				uint32_t dsi_bpp);
enum cb_err sn65dsi86_bridge_read_edid(uint8_t bus, uint8_t chip, struct edid *out);

#endif
