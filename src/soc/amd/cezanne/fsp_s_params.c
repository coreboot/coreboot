/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/apob_cache.h>
#include <device/pci.h>
#include <fsp/api.h>

static void fsp_assign_vbios_upds(FSP_S_CONFIG *scfg)
{
	scfg->vbios_buffer = CONFIG(RUN_FSP_GOP) ? PCI_VGA_RAM_IMAGE_START : 0;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	fsp_assign_vbios_upds(scfg);

	/*
	 * At this point FSP-S has been loaded into RAM. If we were to start loading the APOB
	 * before FSP-S was loaded, we would introduce contention onto the SPI bus and
	 * slow down the FSP-S read from SPI. Since FSP-S takes a while to execute and performs
	 * no SPI operations, we can read the APOB while FSP-S executes.
	 */
	start_apob_cache_read();
}
