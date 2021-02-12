/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>

static void fsp_assign_vbios_upds(FSP_S_CONFIG *scfg)
{
	scfg->vbios_buffer_addr = 0;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	fsp_assign_vbios_upds(scfg);
}
