/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/fsp_params.h>
#include <fsp/api.h>

void starlabs_update_fsp_s_policy(FSP_S_CONFIG *params)
{
	params->PmcModPhySusPgEnable = 1;
}
