/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <types.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg mem_config = {
		.type = MEM_TYPE_LP5X,

		.lpx_dq_map = {
			.ddr0 = {
				.dq0	= {	14,	13,	9,	8,	15,	10,	11,	12	},
				.dq1	= {	1,	3,	7,	0,	6,	5,	2,	4	},
			},
			.ddr1 = {
				.dq0	= {	1,	5,	0,	3,	6,	4,	2,	7	},
				.dq1	= {	13,	12,	10,	8,	15,	9,	14,	11	},
			},
			.ddr2 = {
				.dq0	= {	0,	1,	5,	3,	7,	2,	6,	4	},
				.dq1	= {	8,	15,	12,	10,	14,	13,	9,	11	},
			},
			.ddr3 = {
				.dq0	= {	2,	0,	4,	7,	3,	5,	1,	6	},
				.dq1	= {	14,	8,	9,	11,	12,	15,	13,	10	},
			},
			.ddr4 = {
				.dq0	= {	3,	2,	1,	0,	7,	4,	6,	5	},
				.dq1	= {	8,	10,	11,	9,	14,	13,	12,	15	},
			},
			.ddr6 = {
				.dq0	= {	8,	9,	10,	11,	12,	13,	14,	15	},
				.dq1	= {	7,	6,	5,	4,	3,	2,	1,	0	},
			},
			.ddr6 =	{
				.dq0	= {	13,	12,	15,	14,	9,	8,	11,	10	},
				.dq1	= {	5,	4,	7,	6,	3,	1,	2,	0	},
			},
			.ddr7 = {
				.dq0	= {	3,	1,	2,	0,	4,	6,	5,	7	},
				.dq1	= {	15,	12,	9,	8,	11,	10,	13,	14	},
			},
		},

		.lpx_dqs_map = {
			.ddr0 =	{
				.dqs0	= 1,
				.dqs1	= 0,
			},
			.ddr1 =	{
				.dqs0	= 0,
				.dqs1	= 1,
			},
			.ddr2 =	{
				.dqs0	= 0,
				.dqs1	= 1,
			},
			.ddr3 =	{
				.dqs0	= 0,
				.dqs1	= 1,
			},
			.ddr4 =	{
				.dqs0	= 1,
				.dqs1	= 0,
			},
			.ddr5 =	{
				.dqs0	= 0,
				.dqs1	= 1,
			},
			.ddr6 =	{
				.dqs0	= 0,
				.dqs1	= 1,
			},
			.ddr7 =	{
				.dqs0	= 1,
				.dqs1	= 0,
			}
		},

		.ect			= true,
		.UserBd			= BOARD_TYPE_ULT_ULX,
		.LpDdrDqDqsReTraining	= true,

		.lp5x_config = {
			.ccc_config	= 0x00,
		}
	};

	const bool half_populated = true;

	const struct mem_spd lpddr5_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = get_uint_option("memory_speed", 0),
	};

	memcfg_init(mupd, &mem_config, &lpddr5_spd_info, half_populated);

	const uint8_t vtd = get_uint_option("vtd", 1);
	mupd->FspmConfig.VtdDisable = !vtd;
};
