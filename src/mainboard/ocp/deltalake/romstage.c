/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/ipmi/ipmi_kcs.h>
#include <drivers/vpd/vpd.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <soc/romstage.h>

#include "chip.h"
#include "ipmi.h"
#include "vpd.h"

/*
 * Search from VPD_RW first then VPD_RO for UPD config variables,
 * overwrites them from VPD if it's found.
 */
static void mainboard_config_upd(FSPM_UPD *mupd)
{
	uint8_t val;

	/* Send FSP log message to SOL */
	if (vpd_get_bool(FSP_LOG, VPD_RW_THEN_RO, &val))
		mupd->FspmConfig.SerialIoUartDebugEnable = val;
	else {
		printk(BIOS_INFO, "Not able to get VPD %s, default set "
			"SerialIoUartDebugEnable to 1\n", FSP_LOG);
		mupd->FspmConfig.SerialIoUartDebugEnable = 1;
	}
	mupd->FspmConfig.SerialIoUartDebugIoBase = 0x2f8;
}

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
	uint8_t index;
	const config_t *config = config_of_soc();

	oem_update_iio(mupd);

	for (index = 0; index < MAX_PCH_PCIE_PORT; index++) {
		mupd->FspmConfig.PchPcieForceEnable[index] =
			config->pch_pci_port[index].ForceEnable;
		mupd->FspmConfig.PchPciePortLinkSpeed[index] =
			config->pch_pci_port[index].PortLinkSpeed;
	}

	mupd->FspmConfig.PchPcieRootPortFunctionSwap = 0x00;
	/* The default value is 0XFF in FSP, set it to 0xFE by platform */
	mupd->FspmConfig.PchPciePllSsc = 0xFE;
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Since it's the first IPMI command, it's better to run get BMC
	   selftest result first */
	if (ipmi_kcs_premem_init(CONFIG_BMC_KCS_BASE, 0) == CB_SUCCESS)
		init_frb2_wdt();

	mainboard_config_gpios(mupd);
	mainboard_config_iio(mupd);
	mainboard_config_upd(mupd);
}
