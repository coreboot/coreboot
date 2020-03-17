/*
 * This file is part of the coreboot project.
 *
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

#ifndef _SOC_CANNONLAKE_MEMCFG_INIT_H_
#define _SOC_CANNONLAKE_MEMCFG_INIT_H_

#include <stddef.h>
#include <stdint.h>
#include <fsp/soc_binding.h>

/* Number of dq bits controlled per dqs */
#define DQ_BITS_PER_DQS 8

/* Number of memory DIMM slots available on Cannonlake board */
#define NUM_DIMM_SLOT 4

/*
 * Number of memory packages, where a "package" represents a 64-bit solution.
 */
#define DDR_NUM_PACKAGES 2

/* 64-bit Channel identification */
enum {
	DDR_CH0,
	DDR_CH1,
	DDR_NUM_CHANNELS
};

struct spd_by_pointer {
	size_t spd_data_len;
	uintptr_t spd_data_ptr;
};

enum mem_info_read_type {
	NOT_EXISTING,	/* No memory in this slot */
	READ_SMBUS,	/* Read on-module spd by SMBUS. */
	READ_SPD_CBFS,	/* Find spd file in CBFS. */
	READ_SPD_MEMPTR /* Find spd data from pointer. */
};

struct spd_info {
	enum mem_info_read_type read_type;
	union spd_data_by {
		/* To read on-module spd when read_type is READ_SMBUS. */
		uint8_t spd_smbus_address;

		/* To identify spd file when read_type is READ_SPD_CBFS. */
		int spd_index;

		/* To find spd data when read_type is READ_SPD_MEMPTR. */
		struct spd_by_pointer spd_data_ptr_info;
	} spd_spec;
};

/* Board-specific memory dq mapping information */
struct cnl_mb_cfg {
	/* Parameters required to access SPD for CH0D0/CH0D1/CH1D0/CH1D1. */
	struct spd_info spd[NUM_DIMM_SLOT];

	/*
	 * For each channel, there are 6 sets of DQ byte mappings,
	 * where each set has a package 0 and a package 1 value (package 0
	 * represents the first 64-bit lpddr4 chip combination, and package 1
	 * represents the second 64-bit lpddr4 chip combination).
	 * The first three sets are for CLK, CMD, and CTL.
	 * The fsp package actually expects 6 sets, even though the last 3 sets
	 * are not used in CNL.
	 * We let the meminit_lpddr4() routine take care of clearing the
	 * unused fields for the caller.
	 * Note that dq_map is only used by LPDDR; it does not need to be
	 * initialized for designs using DDR4.
	 */
	uint8_t dq_map[DDR_NUM_CHANNELS][6][DDR_NUM_PACKAGES];

	/*
	 * DQS CPU<>DRAM map Ch0 and Ch1.  Each array entry represents a
	 * mapping of a dq bit on the CPU to the bit it's connected to on
	 * the memory part.  The array index represents the dqs bit number
	 * on the memory part, and the values in the array represent which
	 * pin on the CPU that DRAM pin connects to.
	 * dqs_map is only used by LPDDR; same comments apply as for dq_map
	 * above.
	 */
	uint8_t dqs_map[DDR_NUM_CHANNELS][DQ_BITS_PER_DQS];

	/*
	 * Rcomp resistor values.  These values represent the resistance in
	 * ohms of the three rcomp resistors attached to the DDR_COMP_0,
	 * DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
	 */
	uint16_t rcomp_resistor[3];

	/*
	 * Rcomp target values.  These will typically be the following
	 * values for Cannon Lake : { 80, 40, 40, 40, 30 }
	 */
	uint16_t rcomp_targets[5];

	/*
	 * Indicates whether memory is interleaved.
	 * Set to 1 for an interleaved design,
	 * set to 0 for non-interleaved design.
	 */
	uint8_t dq_pins_interleaved;

	/*
	 * VREF_CA configuration.
	 * Set to 0 VREF_CA goes to both CH_A and CH_B,
	 * set to 1 VREF_CA goes to CH_A and VREF_DQ_A goes to CH_B,
	 * set to 2 VREF_CA goes to CH_A and VREF_DQ_B goes to CH_B.
	 */
	uint8_t vref_ca_config;

	/* Early Command Training Enabled */
	uint8_t ect;
};

/*
 * Initialize default memory configurations for CannonLake.
 */
void cannonlake_memcfg_init(FSP_M_CONFIG *mem_cfg,
			    const struct cnl_mb_cfg *cnl_cfg);

#endif /* _SOC_CANNONLAKE_MEMCFG_INIT_H_ */
