/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_ROMSTAGE_H_
#define _BAYTRAIL_ROMSTAGE_H_

#include <device/dram/ddr3.h>

#define NUM_CHANNELS 2

enum dram_type {
	DRAM_TYPE_DDR3,
	DRAM_TYPE_DDR3L,
	DRAM_TYPE_LPDDR3,
};

enum cpu_odt {
	CPU_ODT_DEFAULT = 0,
	CPU_ODT_60 = 60,
	CPU_ODT_80 = 80,
	CPU_ODT_100 = 100,
	CPU_ODT_120 = 120,
	CPU_ODT_150 = 150,
};

enum dram_odt {
	DRAM_ODT_DEFAULT = 0,
	DRAM_ODT_60 = 60,
	DRAM_ODT_120 = 120,
};

enum spd_src {
	SPD_SRC_NONE,
	SPD_SRC_SMBUS,
	SPD_SRC_MEM,
};

struct spd_cfg {
	enum spd_src src;
	u8 addr;
	spd_ddr3_raw_data data;
	struct dimm_attr_ddr3_st dimm;
};

struct memory_init_params {
	enum dram_type dram_type;
	bool dram_is_slotted;
	/*
	 * The below ODT settings are only honored when !dram_is_slotted.
	 * Additionally, weaker_odt_settings being non-zero causes
	 * cpu_odt_value to not be honored as weaker_odt_settings have a
	 * special training path.
	 */
	bool weaker_odt_settings;
	/* Allowed settings: 60, 80, 100, 120, and 150. */
	enum cpu_odt cpu_odt_value;
	/* Allowed settings: 60 and 120. */
	enum dram_odt dram_odt_value;
	/* SPD configurations for each channel. */
	struct spd_cfg spd_cfgs[NUM_CHANNELS];
};

void mainboard_memory_init_params(struct memory_init_params *memory_init_params);

void raminit(struct memory_init_params *memory_init_params, int prev_sleep_state);
void gfx_init(void);
void punit_init(void);

#endif /* _BAYTRAIL_ROMSTAGE_H_ */
