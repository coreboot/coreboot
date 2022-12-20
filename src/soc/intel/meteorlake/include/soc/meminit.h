/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_MEMINIT_H_
#define _SOC_METEORLAKE_MEMINIT_H_

#include <types.h>
#include <fsp/soc_binding.h>
#include <intelblocks/meminit.h>

enum mem_type {
	MEM_TYPE_DDR5,
	MEM_TYPE_LP5X,
};

struct mem_ddr_config {
	/* Dqs Pins Interleaved Setting. Enable/Disable Control */
	bool dq_pins_interleaved;
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

struct rcomp {
	/*
	 * Rcomp resistor value. This values represents the resistance in
	 * ohms of the rcomp resistor attached to the DDR_COMP pin on the SoC.
	 *
	 * Note: If mainboard users don't want to override rcomp related settings
	 * then associated rcomp UPDs will have its default value.
	 */
	uint16_t resistor;
	/* Rcomp target values. */
	uint16_t targets[5];
};

struct mb_cfg {
	enum mem_type type;
	struct rcomp rcomp;
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

	/* Command Mirror */
	uint8_t CmdMirror;

	/* Enable/Disable TxDqDqs Retraining for Lp4/Lp5/DDR */
	uint8_t LpDdrDqDqsReTraining;
};

void memcfg_init(FSPM_UPD *memupd, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated);

#endif /* _SOC_METEORLAKE_MEMINIT_H_ */
