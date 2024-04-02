/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

const struct mb_cfg *__weak variant_memory_params(void)
{
	/* TODO */
	return NULL;
}

bool __weak variant_is_half_populated(void)
{
	/* TODO */
	return false;
}

void __weak variant_get_spd_info(struct mem_spd *spd_info)
{
	/* TODO */
}
