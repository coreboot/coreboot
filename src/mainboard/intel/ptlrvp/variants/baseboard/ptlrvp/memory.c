/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <baseboard/variants.h>
#include <ec/intel/board_id.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	/* TODO: Add Memory configuration */
	.ect = 1, /* Early Command Training */
};

const struct mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	uint8_t board_id = get_rvp_board_id();
	size_t spd_index;

	printk(BIOS_INFO, "Board ID is 0x%x\n", board_id);

	spd_index = (board_id == GCS_32GB || board_id == GCS_64GB) ? 1 : 0;

	return spd_index;
}

bool __weak variant_is_half_populated(void)
{
	return 0;
}

void __weak variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
