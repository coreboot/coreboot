/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <types.h>
#include <variants.h>

static uint8_t strap_to_memcfg_index(uint8_t strap)
{
	switch (strap) {
	case 13: /* 32GB */
		return 0;
	default: /* 64GB */
		return 1;
	}
}

static uint8_t memcfg_and_speed_to_cbfs_index(uint8_t memcfg, unsigned int speed)
{
	if (speed > 2)
		speed = 2;
	return (uint8_t)(memcfg * 3 + speed);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg mem_config = {
		.type = MEM_TYPE_LP5X,

		.lpx_dq_map = {
			.ddr0 = {
				.dq0 = {  8, 11, 10,  9, 15, 12, 13, 14 },
				.dq1 = {  4,  7,  5,  6,  3,  1,  2,  0 },
			},
			.ddr1 = {
				.dq0 = {  2,  0,  3,  1,  4,  6,  7,  5 },
				.dq1 = { 13, 10, 12, 15, 11,  8,  9, 14 },
			},
			.ddr2 = {
				.dq0 = {  0,  3,  1,  2,  4,  7,  6,  5 },
				.dq1 = {  8, 15,  9, 12, 10, 11, 14, 13 },
			},
			.ddr3 = {
				.dq0 = { 13, 10,  8,  9, 14, 11, 15, 12 },
				.dq1 = {  1,  6,  4,  7,  0,  5,  2,  3 },
			},
			.ddr4 = {
				.dq0 = { 10, 11,  9, 12, 13, 14, 15,  8 },
				.dq1 = {  6,  7,  4,  5,  1,  0,  3,  2 },
			},
			.ddr5 = {
				.dq0 = {  0,  5,  3,  6,  1,  4,  2,  7 },
				.dq1 = {  8, 11, 10,  9, 15, 14, 13, 12 },
			},
			.ddr6 = {
				.dq0 = {  1,  3,  0,  2,  6,  5,  7,  4 },
				.dq1 = { 13, 15, 14, 12, 11, 10,  8,  9 },
			},
			.ddr7 = {
				.dq0 = { 10,  9, 11,  8, 12, 15, 14, 13 },
				.dq1 = {  6,  4,  7,  5,  2,  1,  0,  3 },
			},
		},

		/* DQS CPU<>DRAM map */
		.lpx_dqs_map = {
			.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
			.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
			.ddr7 = { .dqs0 = 1, .dqs1 = 0 },
		},

		.ect			= true,

		.LpDdrDqDqsReTraining	= 1,

		.UserBd			= BOARD_TYPE_ULT_ULX,
		.lp5x_config = {
			.ccc_config	= 0x66,
		}
	};

	const bool half_populated = false;

	const struct mem_spd lpddr5_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = memcfg_and_speed_to_cbfs_index(
			strap_to_memcfg_index(get_memory_config_straps()),
			get_uint_option("memory_speed", 1)),
	};

	memcfg_init(mupd, &mem_config, &lpddr5_spd_info, half_populated);

	const uint8_t vtd = get_uint_option("vtd", 1);
	mupd->FspmConfig.VtdDisable = !vtd;

	/* Enable/Disable Wireless (RP09) based on CMOS settings */
	if (get_uint_option("wifi", 1) == 0)
		mupd->FspmConfig.PcieRpEnableMask &= ~(1 << 8);

	/* Enable/Disable Thunderbolt based on CMOS settings */
	if (get_uint_option("thunderbolt", 1) == 0) {
		mupd->FspmConfig.TcssDma0En = 0;
		mupd->FspmConfig.TcssItbtPcie0En = 0;
		mupd->FspmConfig.TcssXhciEn = 0;
	}

	mupd->FspmConfig.PchHdaSubSystemIds = 0x20147017;
	mupd->FspmConfig.PsysPmax = 0;
};
