/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_CANNONLAKE_LPDDR4_INIT_H_
#define _SOC_CANNONLAKE_LPDDR4_INIT_H_

#include <stddef.h>
#include <stdint.h>
#include <fsp/soc_binding.h>

/* Number of dq bits controlled per dqs */
#define DQ_BITS_PER_DQS 8

/*
 * Number of LPDDR4 packages, where a "package" represents a 64-bit solution.
 */
#define LP4_NUM_PACKAGES 2

/* 64-bit Channel identification */
enum {
	LP4_CH0,
	LP4_CH1,
	LP4_NUM_CHANNELS
};

struct spd_by_pointer {
	size_t spd_data_len;
	uintptr_t spd_data_ptr;
};

struct spd_info {
	bool spd_by_index;
	union spd_data_by {
		int spd_index;
		struct spd_by_pointer spd_data_ptr_info;
	} spd_spec;
};

/* Board-specific lpddr4 dq mapping information */
struct lpddr4_cfg {
	/*
	 * For each channel, there are 3 sets of DQ byte mappings,
	 * where each set has a package 0 and a package 1 value (package 0
	 * represents the first 64-bit lpddr4 chip combination, and package 1
	 * represents the second 64-bit lpddr4 chip combination).
	 * The first three sets are for CLK, CMD, and CTL.
	 * The fsp package actually expects 6 sets, but the last 3 sets are
	 * not used in CNL, so we only define the three sets that are used
	 * and let the meminit_lpddr4() routine take care of clearing the
	 * unused fields for the caller.
	 */
	const uint8_t dq_map[LP4_NUM_CHANNELS][3][LP4_NUM_PACKAGES];

	/*
	 * DQS CPU<>DRAM map Ch0 and Ch1.  Each array entry represents a
	 * mapping of a dq bit on the CPU to the bit it's connected to on
	 * the memory part.  The array index represents the dqs bit number
	 * on the memory part, and the values in the array represent which
	 * pin on the CPU that DRAM pin connects to.
	 */
	const uint8_t dqs_map[LP4_NUM_CHANNELS][DQ_BITS_PER_DQS];

	/*
	 * Rcomp resistor values.  These values represent the resistance in
	 * ohms of the three rcomp resistors attached to the DDR_COMP_0,
	 * DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
	 */
	const uint16_t rcomp_resistor[3];

	/*
	 * Rcomp target values.  These will typically be the following
	 * values for Cannon Lake : { 80, 40, 40, 40, 30 }
	 */
	const uint16_t rcomp_targets[5];

	/*
	 * Indicates whether memory is interleaved.
	 * Set to 1 for an interleaved design,
	 * set to 0 for non-interleaved design.
	 */
	const uint8_t dq_pins_interleaved;

	/* Early Command Training Enabled */
	const uint8_t ect;
};

/*
 * Initialize default LPDDR4 settings for CannonLake.
 */
void cannonlake_lpddr4_init(FSP_M_CONFIG *mem_cfg,
			const struct lpddr4_cfg *cnl_cfg,
			const struct spd_info *spd);

#endif /* _SOC_CANNONLAKE_LPDDR4_INIT_H_ */
