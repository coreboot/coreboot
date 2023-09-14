/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <memory_info.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <device/pci_ops.h>
#include <console/console.h>

static const struct mb_cfg board_mem_cfg = {
	.dq_map[DDR_CH0] = {
		{0x0f, 0xf0},
		{0x0f, 0xf0},
		{0xff, 0x00},
		{0x00, 0x00},
		{0x00, 0x00},
		{0x00, 0x00}
	},
	.dq_map[DDR_CH1] = {
		{0x0f, 0xf0},
		{0x0f, 0xf0},
		{0xff, 0x00},
		{0x00, 0x00},
		{0x00, 0x00},
		{0x00, 0x00}
	},

	.dqs_map[DDR_CH0] = {0, 3, 2, 1, 5, 7, 4, 6},
	.dqs_map[DDR_CH1] = {3, 1, 2, 0, 4, 7, 6, 5},

	/* Enable Early Command Training */
	.ect = 1,

	/* User Board Type */
	.UserBd = BOARD_TYPE_ULT_ULX,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct spd_info spd_info = {
		.read_type = READ_SPD_CBFS,
		.spd_spec.spd_index = 0,
	};

	memcfg_init(&memupd->FspmConfig, &board_mem_cfg, &spd_info, false);
}
