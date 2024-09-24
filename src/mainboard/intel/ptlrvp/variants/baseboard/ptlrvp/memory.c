/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <baseboard/variants.h>

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
	return 0;
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
