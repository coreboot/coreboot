/*
 * This file is part of the coreboot project.
 *
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

#include <assert.h>
#include <bl31.h>
#include <soc/bl31_plat_params.h>

static struct bl31_plat_param *plat_params;

void cn81xx_register_bl31_param(struct bl31_plat_param *param)
{
	ASSERT(param);

	param->next = plat_params;
	plat_params = param;
}

void *soc_get_bl31_plat_params(void)
{
	return plat_params;
}
