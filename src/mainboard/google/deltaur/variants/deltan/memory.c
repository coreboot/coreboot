/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>

static const struct mb_ddr4_cfg baseboard_memcfg = {
	/* DQ byte map */
	.dq_map = {
		[0] = {
			{ 10, 15, 11, 14, 13,  8, 12,  9, },	/* Byte 0 */
			{  3,  5,  1,  0,  4,  7,  2,  6, },	/* Byte 1 */
			{ 15,  8, 11, 13, 10, 12, 14,  9, },	/* Byte 2 */
			{  1,  6,  2,  4,  7,  5,  3,  0, },	/* Byte 3 */
			{  7,  2,  6,  3,  4,  0,  5,  1, },	/* Byte 4 */
			{ 14, 10, 15, 11,  9, 13,  8, 12, },	/* Byte 5 */
			{  8, 10, 14, 12,  9, 13, 11, 15, },	/* Byte 6 */
			{  2,  7,  4,  5,  1,  3,  0,  6  },    /* Byte 7 */
		},

		[1] = {
			{ 12, 14, 10, 11, 15, 13,  9,  8, },	/* Byte 0 */
			{  0,  6,  2,  7,  3,  5,  1,  4, },	/* Byte 1 */
			{ 10,  9, 14, 12, 11,  8, 15, 13, },	/* Byte 2 */
			{  7,  3,  1,  4,  6,  2,  0,  5, },	/* Byte 3 */
			{ 10,  9, 13, 12,  8, 14, 11, 15, },	/* Byte 4 */
			{  5,  4,  0,  2,  7,  3,  6,  1, },	/* Byte 5 */
			{ 15,  9, 11, 13, 10, 14,  8, 12, },	/* Byte 6 */
			{  7,  3,  0,  4,  2,  5,  1,  6  },	/* Byte 7 */
		},
	},

	/* DQS CPU<>DRAM map */
	.dqs_map = {
		{ 1, 0, 1, 0, 0, 1, 1, 0 },
		{ 1, 0, 1, 0, 1, 0, 1, 0 }
	},

	.ect = 0, /* Disable Early Command Training */
};

void variant_memory_init(FSP_M_CONFIG *mem_cfg)
{
	const struct spd_info spd_info = {
		.topology = SODIMM,
		.smbus_info[0] = {.addr_dimm0 = 0x50,
				  .addr_dimm1 = 0 },
		.smbus_info[1] = {.addr_dimm0 = 0x52,
				  .addr_dimm1 = 0 },
	};
	const bool half_populated = false;
	struct mb_ddr4_cfg new_board_cfg_ddr4;

	memcpy(&new_board_cfg_ddr4, &baseboard_memcfg, sizeof(baseboard_memcfg));

	new_board_cfg_ddr4.dq_pins_interleaved = gpio_get(MEMORY_INTERLEAVED);

	meminit_ddr4(mem_cfg, &new_board_cfg_ddr4, &spd_info, half_populated);
}
