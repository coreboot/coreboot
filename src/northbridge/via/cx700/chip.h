/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __VIA_CX700_CHIP_H__
#define __VIA_CX700_CHIP_H__

#define DIMM_SOCKETS 2

struct dram_cfg {
	uint8_t spd_addr[DIMM_SOCKETS];
	enum {
		MCLKO0,
		MCLKO1,
		MCLKO2,
		ALL_MCLKO
	} mem_clocks;
};

struct northbridge_via_cx700_config {
	struct dram_cfg dram_cfg;
};

#endif
