/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
