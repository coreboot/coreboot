/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <soc/romstage.h>

#include "ipmi.h"

/* Update bifurcation settings according to different Configs */
static void oem_update_iio(FSPM_UPD *mupd)
{
	uint8_t pcie_config = 0;

	/* Default set to PCIE_CONFIG_C first */
	mupd->FspmConfig.IioConfigIOU0[0] = IIO_BIFURCATE_x4x4x4x4;
	mupd->FspmConfig.IioConfigIOU1[0] = IIO_BIFURCATE_x4x4x4x4;
	mupd->FspmConfig.IioConfigIOU2[0] = IIO_BIFURCATE_xxxxxxxx;
	mupd->FspmConfig.IioConfigIOU3[0] = IIO_BIFURCATE_xxxxxx16;
	mupd->FspmConfig.IioConfigIOU4[0] = IIO_BIFURCATE_xxxxxxxx;
	/* Update IIO bifurcation according to different Configs */
	if (ipmi_get_pcie_config(&pcie_config) == CB_SUCCESS) {
		printk(BIOS_DEBUG, "get IPMI PCIe config: %d\n", pcie_config);
		switch (pcie_config) {
		case PCIE_CONFIG_A:
			mupd->FspmConfig.IioConfigIOU0[0] = IIO_BIFURCATE_xxxxxxxx;
			mupd->FspmConfig.IioConfigIOU3[0] = IIO_BIFURCATE_xxxxxxxx;
			break;
		case PCIE_CONFIG_B:
			mupd->FspmConfig.IioConfigIOU0[0] = IIO_BIFURCATE_xxxxxxxx;
			mupd->FspmConfig.IioConfigIOU3[0] = IIO_BIFURCATE_x4x4x4x4;
			break;
		case PCIE_CONFIG_D:
			mupd->FspmConfig.IioConfigIOU3[0] = IIO_BIFURCATE_x4x4x4x4;
			break;
		case PCIE_CONFIG_C:
		default:
			break;
		}
	} else {
		printk(BIOS_ERR, "%s failed to get IPMI PCIe config\n", __func__);
	}
}

/*
* Configure GPIO depend on platform
*/
static void mainboard_config_gpios(FSPM_UPD *mupd)
{
	/* To be implemented */
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	/* Send FSP log message to SOL */
	mupd->FspmConfig.SerialIoUartDebugEnable = 1;
	mupd->FspmConfig.SerialIoUartDebugIoBase = 0x2f8;
	oem_update_iio(mupd);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	mainboard_config_gpios(mupd);
	mainboard_config_iio(mupd);
}
