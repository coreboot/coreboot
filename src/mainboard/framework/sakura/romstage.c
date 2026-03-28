/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/meminit.h>
#include <soc/romstage.h>

/*
 * LPDDR5x LPCAMM2 module, use same config as camm_t3_mem_config on ptl_rvp
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
				.dq0 = { 14, 15, 12, 13, 10,  8, 11,  9 },
				.dq1 = {  4,  6,  7,  5,  1,  3,  0,  2 },
			},
			.ddr4 = {
				.dq0 = {  3,  0,  2,  1,  6,  7,  4,  5 },
				.dq1 = { 13, 12, 15, 14,  8, 10, 11,  9 },
			},
			.ddr5 = {
				.dq0 = { 10,  8, 11,  9, 13, 15, 12, 14 },
				.dq1 = {  2,  1,  3,  0,  7,  6,  5,  4 },
			},
			.ddr6 = {
				.dq0 = {  3,  1,  2,  0,  5,  7,  4,  6 },
				.dq1 = { 12, 14, 15, 13,  9, 10, 11,  8 },
			},
			.ddr7 = {
				.dq0 = {  8,  9, 10, 11, 12, 13, 14, 15 },
				.dq1 = {  5,  6,  7,  4,  2,  1,  3,  0 },
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
			.ddr7 = { .dqs0 = 1, .dqs1 = 0 }
		},

		.ect = true, /* Early Command Training */

		.user_bd = BOARD_TYPE_ULT_ULX,

		.lp5x_config = {
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
