/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ALDERLAKE_MEMINIT_H_
#define _SOC_ALDERLAKE_MEMINIT_H_

#include <stddef.h>
#include <stdint.h>
#include <fsp/soc_binding.h>

#define BYTES_PER_CHANNEL	2
#define BITS_PER_BYTE		8
#define DQS_PER_CHANNEL		2

/* 64-bit Channel identification */
enum {
	DDR_CH0,
	DDR_CH1,
	DDR_CH2,
	DDR_CH3,
	DDR_CH4,
	DDR_CH5,
	DDR_CH6,
	DDR_CH7,
	DDR_NUM_CHANNELS
};
/* Number of memory DIMM slots available on Alderlake board */
#define NUM_DIMM_SLOT		16

struct spd_by_pointer {
	size_t spd_data_len;
	uintptr_t spd_data_ptr;
};

enum mem_info_read_type {
	NOT_EXISTING,	/* No memory in this slot */
	READ_SMBUS,	/* Read on-module spd by SMBUS. */
	READ_SPD_CBFS,	/* Find SPD file in CBFS. */
	READ_SPD_MEMPTR	/* Find SPD data from pointer. */
};

struct spd_info {
	enum mem_info_read_type read_type;
	union spd_data_by {
		/* To read on-module SPD when read_type is READ_SMBUS. */
		uint8_t spd_smbus_address[NUM_DIMM_SLOT];

		/* To identify SPD file when read_type is READ_SPD_CBFS. */
		int spd_index;

		/* To find SPD data when read_type is READ_SPD_MEMPTR. */
		struct spd_by_pointer spd_data_ptr_info;
	} spd_spec;
};

/* Board-specific memory configuration information */
struct mb_cfg {
	/* DQ mapping */
	uint8_t dq_map[DDR_NUM_CHANNELS][BYTES_PER_CHANNEL * BITS_PER_BYTE];

	/*
	 * DQS CPU<>DRAM map.  Each array entry represents a
	 * mapping of a dq bit on the CPU to the bit it's connected to on
	 * the memory part.  The array index represents the dqs bit number
	 * on the memory part, and the values in the array represent which
	 * pin on the CPU that DRAM pin connects to.
	 */
	uint8_t dqs_map[DDR_NUM_CHANNELS][DQS_PER_CHANNEL];

	/*
	 * Rcomp resistor values.  These values represent the resistance in
	 * ohms of the three rcomp resistors attached to the DDR_COMP_0,
	 * DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
	 */
	uint16_t rcomp_resistor[3];

	/* Rcomp target values. */
	uint16_t rcomp_targets[5];

	/*
	 * Dqs Pins Interleaved Setting. Enable/Disable Control
	 * TRUE = enable, FALSE = disable
	 */
	bool dq_pins_interleaved;

	/*
	 * Early Command Training Enable/Disable Control
	 * TRUE = enable, FALSE = disable
	 */
	bool ect;

	/* Board type */
	uint8_t UserBd;
};

/*
 * Initialize default memory configurations for Alder Lake.
 */

void memcfg_init(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *board_cfg,
			const struct spd_info *spd_info, bool half_populated);

#endif /* _SOC_ALDERLAKE_MEMINIT_H_ */
