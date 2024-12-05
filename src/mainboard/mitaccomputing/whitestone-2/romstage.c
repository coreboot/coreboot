/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>
#include <soc/romstage.h>
#include <defs_cxl.h>
#include <defs_iio.h>
#include <sprsp_ws_2_iio.h>

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	/* If CONFIG(OCP_VPD) is not enabled or CXL is explicitly disabled, don't enable CXL */
	if (!CONFIG(OCP_VPD) || get_cxl_mode_from_vpd() == CXL_DISABLED) {
		printk(BIOS_DEBUG, "Don't enable CXL via VPD %s\n", CXL_MODE);
	} else {
		/* Set socket 0 IIO PCIe PE1 to CXL mode */
		/* eg. Protocol Auto Negotiation */
		mupd->FspmConfig.IioPcieSubSystemMode1[0] = IIO_MODE_CXL;

		mupd->FspmConfig.DfxCxlHeaderBypass = 0;
		mupd->FspmConfig.DfxCxlSecLvl = CXL_SECURITY_FULLY_TRUSTED;

		mupd->FspmConfig.DelayAfterPCIeLinkTraining = 2000; /* ms */
	}
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	uint8_t val;

	/* Send FSP log message to SOL */
	if (CONFIG(VPD) && vpd_get_bool(FSP_LOG, VPD_RW_THEN_RO, &val))
		mupd->FspmConfig.SerialIoUartDebugEnable = val;
	else {
		printk(BIOS_INFO, "Not able to get VPD %s, default set SerialIoUartDebugEnable to %d\n",
				FSP_LOG, FSP_LOG_DEFAULT);
		mupd->FspmConfig.SerialIoUartDebugEnable = FSP_LOG_DEFAULT;
	}

	/* Set Rank Margin Tool to disable. */
	mupd->FspmConfig.EnableRMT = 0x0;
	/* Enable - Portions of memory reference code will be skipped
	 * when possible to increase boot speed on warm boots.
	 * Disable - Disables this feature.
	 * Auto - Sets it to the MRC default setting.
	 */
	mupd->FspmConfig.AttemptFastBoot = 0x1;
	mupd->FspmConfig.AttemptFastBootCold = 0x1;

	/* Set Adv MemTest Option to 0. */
	mupd->FspmConfig.AdvMemTestOptions = 0x0;
	/* Set MRC Promote Warnings to disable.
	   Determines if MRC warnings are promoted to system level. */
	mupd->FspmConfig.promoteMrcWarnings = 0x0;
	/* Set Promote Warnings to disable.
	   Determines if warnings are promoted to system level. */
	mupd->FspmConfig.promoteWarnings = 0x0;
	soc_config_iio(mupd, ws2_iio_pci_port, ws2_iio_bifur);
	mainboard_config_iio(mupd);
}
