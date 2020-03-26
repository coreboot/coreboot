/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _SOC_TIGERLAKE_MEMINIT_H_
#define _SOC_TIGERLAKE_MEMINIT_H_

#include <stddef.h>
#include <stdint.h>
#include <fsp/soc_binding.h>

#define BITS_PER_BYTE			8

#define LPDDR4X_CHANNELS		8
#define LPDDR4X_BYTES_PER_CHANNEL	2

enum mem_topology {
	MEMORY_DOWN,	/* Supports reading SPD from CBFS or in-memory pointer. */
};

enum md_spd_loc {
	 /* Read SPD from pointer provided to memory location. */
	SPD_MEMPTR,
	/* Read SPD using index into spd.bin in CBFS.  */
	SPD_CBFS,
};

struct spd_info {
	enum mem_topology topology;

	/* SPD info for Memory down topology */
	enum md_spd_loc md_spd_loc;
	union {
		/* Used for SPD_CBFS */
		uint8_t cbfs_index;

		struct {
			/* Used for SPD_MEMPTR */
			uintptr_t data_ptr;
			size_t data_len;
		};
	};
};

/* Board-specific memory configuration information */
struct lpddr4x_cfg {
	/*
	 * DQ CPU<>DRAM map:
	 * LPDDR4x memory interface has 2 DQs per channel. Each DQ consists of 8 bits(1
	 * byte). Thus, dq_map is represented as DDR[7-0]_DQ[1-0][7:0], where
	 * DDR[7-0] : LPDDR4x channel #
	 * DQ[1-0]  : DQ # within the channel
	 * [7:0]    : Bits within the DQ
	 *
	 * Index of the array represents DQ pin# on the CPU, whereas value in
	 * the array represents DQ pin# on the memory part.
	 */
	uint8_t dq_map[LPDDR4X_CHANNELS][LPDDR4X_BYTES_PER_CHANNEL][BITS_PER_BYTE];

	/*
	 * DQS CPU<>DRAM map:
	 * LPDDR4x memory interface has 2 DQS pairs(P/N) per channel. Thus, dqs_map is
	 * represented as DDR[7-0]_DQS[1:0], where
	 * DDR[7-0]  : LPDDR4x channel #
	 * DQS[1-0]  : DQS # within the channel
	 *
	 * Index of the array represents DQS pin# on the CPU, whereas value in
	 * the array represents DQ pin# on the memory part.
	 */
	uint8_t dqs_map[LPDDR4X_CHANNELS][LPDDR4X_BYTES_PER_CHANNEL];

	/*
	 * Early Command Training Enable/Disable Control
	 * 1 = enable, 0 = disable
	 */
	uint8_t ect;
};

void meminit_lpddr4x(FSP_M_CONFIG *mem_cfg, const struct lpddr4x_cfg *board_cfg,
		const struct spd_info *spd, bool half_populated);

#endif /* _SOC_TIGERLAKE_MEMINIT_H_ */
