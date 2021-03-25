/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <device/pci.h>

static void fsp_assign_vbios_upds(FSP_S_CONFIG *scfg)
{
	scfg->vbios_buffer = CONFIG(RUN_FSP_GOP) ? PCI_VGA_RAM_IMAGE_START : 0;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	fsp_assign_vbios_upds(scfg);
}
