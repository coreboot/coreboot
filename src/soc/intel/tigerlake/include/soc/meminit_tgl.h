/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _SOC_MEMINIT_TGL_H_
#define _SOC_MEMINIT_TGL_H_

#include <stddef.h>
#include <stdint.h>
#include <fsp/soc_binding.h>

#define BYTES_PER_CHANNEL	2
#define BITS_PER_BYTE		8
#define DQS_PER_CHANNEL		2
#define NUM_CHANNELS		8

struct spd_by_pointer {
	size_t spd_data_len;
	uintptr_t spd_data_ptr;
};

enum mem_info_read_type {
	NOT_EXISTING,	/* No memory in this channel */
	READ_SPD_CBFS,	/* Find spd file in CBFS. */
	READ_SPD_MEMPTR	/* Find spd data from pointer. */
};

struct spd_info {
	enum mem_info_read_type read_type;
	union spd_data_by {
		/* To identify spd file when read_type is READ_SPD_CBFS. */
		int spd_index;

		/* To find spd data when read_type is READ_SPD_MEMPTR. */
		struct spd_by_pointer spd_data_ptr_info;
	} spd_spec;
};

/* Board-specific memory configuration information */
struct mb_lpddr4x_cfg {
	/* DQ mapping */
	uint8_t dq_map[NUM_CHANNELS][BYTES_PER_CHANNEL * BITS_PER_BYTE];

	/*
	 * DQS CPU<>DRAM map.  Each array entry represents a
	 * mapping of a dq bit on the CPU to the bit it's connected to on
	 * the memory part.  The array index represents the dqs bit number
	 * on the memory part, and the values in the array represent which
	 * pin on the CPU that DRAM pin connects to.
	 */
	uint8_t dqs_map[NUM_CHANNELS][DQS_PER_CHANNEL];

	/*
	 * Early Command Training Enable/Disable Control
	 * 1 = enable, 0 = disable
	 */
	uint8_t ect;
};

/* Initialize default memory configurations for dimm0-only lpddr4x */
void meminit_lpddr4x_dimm0(FSP_M_CONFIG *mem_cfg,
			   const struct mb_lpddr4x_cfg *board_cfg,
			   const struct spd_info *spd,
			   bool half_populated);

#endif /* _SOC_MEMINIT_TGL_H_ */
