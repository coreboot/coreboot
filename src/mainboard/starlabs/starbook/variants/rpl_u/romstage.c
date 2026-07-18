/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/memory.h>
#include <fsp/api.h>
#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <types.h>

#define MEMORY_SPEED_DEFAULT 1

static unsigned int get_memory_speed_option(void)
{
	return starlabs_get_memory_speed_option(MEMORY_SPEED_DEFAULT);
}

uint8_t fsp_memory_mainboard_version(void)
{
	return get_memory_speed_option();
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg mem_config = {
		.type = MEM_TYPE_LP5X,

		.lpx_dq_map = {
			.ddr0 = {
				.dq0 = {  5,  0,  4,  1,  2,  6,  7,  3 },
				.dq1 = { 11, 15, 13, 12, 10, 14,  8,  9 },
			},
			.ddr1 = {
				.dq0 = {  9, 10, 11,  8, 13, 14, 12, 15 },
				.dq1 = {  0,  2,  1,  3,  7,  5,  6,  4 },
			},
			.ddr2 = {
				.dq0 = {  3,  7,  2,  6,  4,  1,  5,  0 },
				.dq1 = { 12, 14, 15, 13, 11, 10,  8,  9 },
			},
			.ddr3 = {
				.dq0 = { 15, 14, 12, 13, 10,  9, 11,  8 },
				.dq1 = {  7,  6,  4,  5,  0,  3,  1,  2 },
			},
			.ddr4 = {
				.dq0 = { 15, 14, 12, 13, 10,  9,  8, 11 },
				.dq1 = {  1,  3,  0,  2,  5,  6,  7,  4 },
			},
			.ddr5 = {
				.dq0 = {  9, 10, 11,  8, 12, 15, 13, 14 },
				.dq1 = {  3,  7,  2,  6,  0,  4,  5,  1 },
			},
			.ddr6 = {
				.dq0 = { 11,  8, 10,  9, 12, 14, 13, 15 },
				.dq1 = {  0,  7,  1,  2,  6,  4,  3,  5 },
			},
			.ddr7 = {
				.dq0 = {  1,  2,  3,  0,  7,  5,  6,  4 },
				.dq1 = { 15, 14, 11, 13,  8,  9, 12, 10 },
			},
		},
		.lpx_dqs_map = {
			.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
		},

		.ect			= true,
		.UserBd			= BOARD_TYPE_MOBILE,
		.LpDdrDqDqsReTraining	= true,

		.lp5x_config = {
			.ccc_config	= 0xff,
		}
	};

	const bool half_populated = false;

	const struct mem_spd lpddr5_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = fsp_memory_mainboard_version(),
	};

	memcfg_init(mupd, &mem_config, &lpddr5_spd_info, half_populated);

	mupd->FspmConfig.PchHdaDspEnable = get_uint_option("hda_dsp", 1);

	const uint8_t vtd = get_uint_option("vtd", 1);
	mupd->FspmConfig.VtdDisable = !vtd;

	/* Enable/Disable Thunderbolt based on CMOS settings */
	if (get_uint_option("thunderbolt", 1) == 0) {
		mupd->FspmConfig.VtdItbtEnable = 0;
		mupd->FspmConfig.VtdBaseAddress[3] = 0;
		mupd->FspmConfig.VtdBaseAddress[4] = 0;
		mupd->FspmConfig.TcssDma0En = 0;
		mupd->FspmConfig.TcssItbtPcie0En = 0;
		mupd->FspmConfig.TcssItbtPcie1En = 0;
		mupd->FspmConfig.TcssXhciEn = 0;
	}
};
