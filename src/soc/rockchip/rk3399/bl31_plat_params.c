/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <arm_tf.h>
#include <cbmem.h>
#include <soc/bl31_plat_params.h>

static struct bl31_plat_param *plat_params;

void register_bl31_param(struct bl31_plat_param *param)
{
	param->next = plat_params;
	plat_params = param;
}

void *soc_get_bl31_plat_params(bl31_params_t *bl31_params)
{
	static struct bl31_u64_param cbtable_param = {
		.h = { .type = PARAM_COREBOOT_TABLE, },
	};
	if (!cbtable_param.value) {
		cbtable_param.value = (uint64_t)cbmem_find(CBMEM_ID_CBTABLE);
		if (cbtable_param.value)
			register_bl31_param(&cbtable_param.h);
	}
	return plat_params;
}
