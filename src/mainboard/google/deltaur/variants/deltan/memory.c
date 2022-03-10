/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>
#include <string.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_DDR4,

	/* DQ byte map */
	.ddr4_dq_map = {
		.ddr0 = {
			.dq0 = { 10, 15, 11, 14, 13,  8, 12,  9, },	/* Byte 0 */
			.dq1 = {  3,  5,  1,  0,  4,  7,  2,  6, },	/* Byte 1 */
			.dq2 = { 15,  8, 11, 13, 10, 12, 14,  9, },	/* Byte 2 */
			.dq3 = {  1,  6,  2,  4,  7,  5,  3,  0, },	/* Byte 3 */
			.dq4 = {  7,  2,  6,  3,  4,  0,  5,  1, },	/* Byte 4 */
			.dq5 = { 14, 10, 15, 11,  9, 13,  8, 12, },	/* Byte 5 */
			.dq6 = {  8, 10, 14, 12,  9, 13, 11, 15, },	/* Byte 6 */
			.dq7 = {  2,  7,  4,  5,  1,  3,  0,  6  },	/* Byte 7 */
		},

		.ddr1 = {
			.dq0 = { 12, 14, 10, 11, 15, 13,  9,  8, },	/* Byte 0 */
			.dq1 = {  0,  6,  2,  7,  3,  5,  1,  4, },	/* Byte 1 */
			.dq2 = { 10,  9, 14, 12, 11,  8, 15, 13, },	/* Byte 2 */
			.dq3 = {  7,  3,  1,  4,  6,  2,  0,  5, },	/* Byte 3 */
			.dq4 = { 10,  9, 13, 12,  8, 14, 11, 15, },	/* Byte 4 */
			.dq5 = {  5,  4,  0,  2,  7,  3,  6,  1, },	/* Byte 5 */
			.dq6 = { 15,  9, 11, 13, 10, 14,  8, 12, },	/* Byte 6 */
			.dq7 = {  7,  3,  0,  4,  2,  5,  1,  6  },	/* Byte 7 */
		},
	},

	/* DQS CPU<>DRAM map */
	.ddr4_dqs_map = {
		.ddr0 = {
			.dqs0 = 1,
			.dqs1 = 0,
			.dqs2 = 1,
			.dqs3 = 0,
			.dqs4 = 0,
			.dqs5 = 1,
			.dqs6 = 1,
			.dqs7 = 0,
		},
		.ddr1 = {
			.dqs0 = 1,
			.dqs1 = 0,
			.dqs2 = 1,
			.dqs3 = 0,
			.dqs4 = 1,
			.dqs5 = 0,
			.dqs6 = 1,
			.dqs7 = 0,
		}
	},

	.ect = false, /* Disable Early Command Training */
};

void variant_memory_init(FSPM_UPD *mupd)
{
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};
	const bool half_populated = false;
	struct mb_cfg new_board_cfg_ddr4;

	memcpy(&new_board_cfg_ddr4, &baseboard_memcfg, sizeof(baseboard_memcfg));

	new_board_cfg_ddr4.ddr4_config.dq_pins_interleaved = gpio_get(MEMORY_INTERLEAVED);

	memcfg_init(mupd, &new_board_cfg_ddr4, &spd_info, half_populated);
}
