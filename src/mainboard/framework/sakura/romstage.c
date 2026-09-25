/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/meminit.h>
#include <soc/romstage.h>

/*
 * LPDDR5x LPCAMM2 module.
 *
 * The DQ/DQS maps come from the schematic: sheet 9 ties SoC balls to DDR_Mx nets,
 * and sheet 24 routes those nets to the LPCAMM2 connector pins.
 * Each entry is the connector DQ pin that the SoC DQ bit is wired to.
 */
void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_LP5X,

		.lpx_dq_map = {
			.ddr0 = {
				.dq0 = {  0,  3,  1,  2,  6,  7,  4,  5 },
				.dq1 = { 13, 12, 15, 14,  8, 10, 11,  9 },
			},
			.ddr1 = {
				.dq0 = {  8, 10, 11,  9, 13, 15, 14, 12 },
				.dq1 = {  5,  7,  6,  4,  3,  2,  1,  0 },
			},
			.ddr2 = {
				.dq0 = {  1,  3,  0,  2,  6,  7,  5,  4 },
				.dq1 = { 12, 13, 15, 14,  8, 11,  9, 10 },
			},
			.ddr3 = {
				.dq0 = { 14, 15, 12, 13, 10,  8,  9, 11 },
				.dq1 = {  4,  6,  7,  5,  1,  3,  2,  0 },
			},
			.ddr4 = {
				.dq0 = {  3,  0,  2,  1,  6,  7,  4,  5 },
				.dq1 = { 13, 12, 15, 14,  8, 10, 11,  9 },
			},
			.ddr5 = {
				.dq0 = { 10,  8, 11,  9, 13, 15, 14, 12 },
				.dq1 = {  2,  1,  3,  0,  7,  6,  5,  4 },
			},
			.ddr6 = {
				.dq0 = {  3,  1,  2,  0,  6,  7,  4,  5 },
				.dq1 = { 12, 13, 15, 14,  9, 10, 11,  8 },
			},
			.ddr7 = {
				.dq0 = {  9,  8, 10, 11, 12, 15, 14, 13 },
				.dq1 = {  5,  6,  7,  4,  0,  1,  3,  2 },
			},
		},

		.lpx_dqs_map = {
			.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr7 = { .dqs0 = 1, .dqs1 = 0 },
		},

		.ect = true, /* Early Command Training */

		.user_bd = BOARD_TYPE_ULT_ULX,

		.lp5x_config = {
			/* CA pins: channels 0/2/4/6 descending, 1/3/5/7 ascending */
			.ccc_config = 0x55,
		},
	};

	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_LP5_CAMM,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x50, },
			[2] = { .addr_dimm[0] = 0x50, },
			[3] = { .addr_dimm[0] = 0x50, },
			[4] = { .addr_dimm[0] = 0x50, },
			[5] = { .addr_dimm[0] = 0x50, },
			[6] = { .addr_dimm[0] = 0x50, },
			[7] = { .addr_dimm[0] = 0x50, },
		},
	};

	const bool half_populated = false;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
