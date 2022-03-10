/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_TIGERLAKE_MEMINIT_H_
#define _SOC_TIGERLAKE_MEMINIT_H_

#include <types.h>
#include <fsp/soc_binding.h>
#include <intelblocks/meminit.h>

enum mem_type {
	MEM_TYPE_DDR4,
	MEM_TYPE_LP4X,
};

struct ddr4_dq {
	uint8_t dq0[BITS_PER_BYTE];
	uint8_t dq1[BITS_PER_BYTE];
	uint8_t dq2[BITS_PER_BYTE];
	uint8_t dq3[BITS_PER_BYTE];
	uint8_t dq4[BITS_PER_BYTE];
	uint8_t dq5[BITS_PER_BYTE];
	uint8_t dq6[BITS_PER_BYTE];
	uint8_t dq7[BITS_PER_BYTE];
};

struct ddr4_dqs {
	uint8_t dqs0;
	uint8_t dqs1;
	uint8_t dqs2;
	uint8_t dqs3;
	uint8_t dqs4;
	uint8_t dqs5;
	uint8_t dqs6;
	uint8_t dqs7;
};

struct ddr4_dq_map {
	struct ddr4_dq ddr0;
	struct ddr4_dq ddr1;
};

struct ddr4_dqs_map {
	struct ddr4_dqs ddr0;
	struct ddr4_dqs ddr1;
};

struct lp4x_dq {
	uint8_t dq0[BITS_PER_BYTE];
	uint8_t dq1[BITS_PER_BYTE];
};

struct lp4x_dqs {
	uint8_t dqs0;
	uint8_t dqs1;
};

struct lp4x_dq_map {
	struct lp4x_dq ddr0;
	struct lp4x_dq ddr1;
	struct lp4x_dq ddr2;
	struct lp4x_dq ddr3;
	struct lp4x_dq ddr4;
	struct lp4x_dq ddr5;
	struct lp4x_dq ddr6;
	struct lp4x_dq ddr7;
};

struct lp4x_dqs_map {
	struct lp4x_dqs ddr0;
	struct lp4x_dqs ddr1;
	struct lp4x_dqs ddr2;
	struct lp4x_dqs ddr3;
	struct lp4x_dqs ddr4;
	struct lp4x_dqs ddr5;
	struct lp4x_dqs ddr6;
	struct lp4x_dqs ddr7;
};

struct mem_ddr4_config {
	bool dq_pins_interleaved;
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
		struct lp4x_dq_map lp4x_dq_map;
		struct ddr4_dq_map ddr4_dq_map;
	};

	union {
		/*
		 * DQS CPU<>DRAM map:
		 * Index of the array represents DQS# on the CPU and the value represents DQS#
		 * on the DRAM part.
		 */
		uint8_t dqs_map[CONFIG_DATA_BUS_WIDTH/BITS_PER_BYTE];
		struct lp4x_dqs_map lp4x_dqs_map;
		struct ddr4_dqs_map ddr4_dqs_map;
	};

	/* Early Command Training Enable/Disable Control */
	bool ect;

	struct mem_ddr4_config ddr4_config;
};

void memcfg_init(FSPM_UPD *memupd, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated);

#endif /* _SOC_TIGERLAKE_MEMINIT_H_ */
