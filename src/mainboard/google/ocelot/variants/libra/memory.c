/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	/* Map CPU DQ/DQS indices to DRAM pins. */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  9, 11,  8, 10, 12, 15, 13, 14, },
			.dq1 = {  5,  7,  6,  4,  2,  0,  1,  3, },
		},
		.ddr1 = {
			.dq0 = {  8,  9, 10, 11, 12, 14, 13, 15, },
			.dq1 = {  7,  6,  5,  4,  1,  3,  0,  2, },
		},
		.ddr2 = {
			.dq0 = {  2,  0,  3,  1,  5,  7,  6,  4, },
			.dq1 = {  9,  8, 10, 11, 14, 15, 13, 12, },
		},
		.ddr3 = {
			.dq0 = { 10, 11,  8,  9, 13, 14, 12, 15, },
			.dq1 = {  6,  7,  5,  4,  3,  2,  0,  1, },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.ect = true, /* Early Command Training */

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &lp5_mem_config;
}

/*
 * Both 4 GB packages populate all four LPDDR5X channels.
 * WCL FSP disables MC1 after detecting the x64 DDR PHY.
 */
bool variant_is_half_populated(void)
{
	return false;
}

int variant_memory_sku(void)
{
	/* All supported memory parts share SPD index 0. */
	gpio_t spd_gpios[] = {
		GPP_H23, /* MEM_ID0 */
		GPP_H24, /* MEM_ID1 */
		GPP_B06, /* MEM_ID2 */
		GPP_B08, /* MEM_ID3 */
	};
	uint32_t mem_id = gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));

	printk(BIOS_INFO, "Libra MEM_ID=0x%x\n", mem_id);

	switch (mem_id) {
	case 0x0: /* Samsung K3KL8L80EM-MGCU */
	case 0x8: /* Micron MT62F1G32D2DS-020 WT:D */
	case 0x4: /* Hynix H58G56CK8BX146 */
		return 0;
	default:
		die("Unsupported Libra MEM_ID 0x%x\n", mem_id);
	}
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
