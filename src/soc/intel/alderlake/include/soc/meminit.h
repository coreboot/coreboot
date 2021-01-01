/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ALDERLAKE_MEMINIT_H_
#define _SOC_ALDERLAKE_MEMINIT_H_

#include <stddef.h>
#include <stdint.h>
#include <types.h>
#include <fsp/soc_binding.h>
#include <intelblocks/meminit.h>

enum mem_type {
	MEM_TYPE_DDR4,
	MEM_TYPE_DDR5,
	MEM_TYPE_LP4X,
	MEM_TYPE_LP5X,
};

struct mem_ddr_config {
	/* Dqs Pins Interleaved Setting. Enable/Disable Control */
	bool dq_pins_interleaved;
	/*
	 * Rcomp resistor values.  These values represent the resistance in
	 * ohms of the three rcomp resistors attached to the DDR_COMP_0,
	 * DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
	 */
	uint16_t rcomp_resistor[3];
	/* Rcomp target values. */
	uint16_t rcomp_targets[5];
};

struct lpx_dq {
	uint8_t dq0[BITS_PER_BYTE];
	uint8_t dq1[BITS_PER_BYTE];
};

struct lpx_dqs {
	uint8_t dqs0;
	uint8_t dqs1;
};

struct lpx_dq_map {
	struct lpx_dq ddr0;
	struct lpx_dq ddr1;
	struct lpx_dq ddr2;
	struct lpx_dq ddr3;
	struct lpx_dq ddr4;
	struct lpx_dq ddr5;
	struct lpx_dq ddr6;
	struct lpx_dq ddr7;
};

struct lpx_dqs_map {
	struct lpx_dqs ddr0;
	struct lpx_dqs ddr1;
	struct lpx_dqs ddr2;
	struct lpx_dqs ddr3;
	struct lpx_dqs ddr4;
	struct lpx_dqs ddr5;
	struct lpx_dqs ddr6;
	struct lpx_dqs ddr7;
};

struct mem_lp5x_config {
	uint8_t ccc_config;
};

struct mb_cfg {
	enum mem_type type;

	union {
		/*
		 * DQ CPU<>DRAM map:
		 * Index of the array represents DQ# on the CPU and the value represents DQ# on
		 * the DRAM part.
		 */
		uint8_t dq_map[CONFIG_DATA_BUS_WIDTH];
		struct lpx_dq_map lpx_dq_map;
	};

	union {
		/*
		 * DQS CPU<>DRAM map:
		 * Index of the array represents DQS# on the CPU and the value represents DQS#
		 * on the DRAM part.
		 */
		uint8_t dqs_map[CONFIG_DATA_BUS_WIDTH/BITS_PER_BYTE];
		struct lpx_dqs_map lpx_dqs_map;
	};

	union {
		struct mem_lp5x_config lp5x_config;
		struct mem_ddr_config ddr_config;
	};

	/* Early Command Training Enable/Disable Control */
	bool ect;

	/* Board type */
	uint8_t UserBd;
};

void memcfg_init(FSP_M_CONFIG *mem_cfg, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated);

#endif /* _SOC_ALDERLAKE_MEMINIT_H_ */
