/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/ewl/ocp_ewl.h>
#include <drivers/ocp/include/vpd.h>
#include <soc/romstage.h>
#include <defs_cxl.h>
#include <defs_iio.h>
#include <sprsp_ac_iio.h>

void mainboard_ewl_check(void)
{
	if (CONFIG(OCP_EWL))
		get_ewl();
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	/* If CONFIG(OCP_VPD) is not enabled or CXL is explicitly disabled, don't enable CXL */
	if (!CONFIG(OCP_VPD) || get_cxl_mode_from_vpd() == CXL_DISABLED) {
		printk(BIOS_DEBUG, "Don't enable CXL via VPD %s\n", CXL_MODE);
	} else {
		/* Set socket 0 IIO PCIe PE1 to CXL mode */
		/* Set socket 1 IIO PCIe PE0 to CXL mode */
		/* eg. Protocl Auto Negotiation */
		mupd->FspmConfig.IioPcieSubSystemMode1[0] = IIO_MODE_CXL;
		mupd->FspmConfig.IioPcieSubSystemMode0[1] = IIO_MODE_CXL;

		mupd->FspmConfig.DfxCxlHeaderBypass = 0;
		mupd->FspmConfig.DfxCxlSecLvl = CXL_SECURITY_FULLY_TRUSTED;

		mupd->FspmConfig.DelayAfterPCIeLinkTraining = 2000; /* ms */
	}
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Setup FSP log */
	if (CONFIG(OCP_VPD)) {
		mupd->FspmConfig.SerialIoUartDebugEnable = get_bool_from_vpd(FSP_LOG,
			FSP_LOG_DEFAULT);
		if (mupd->FspmConfig.SerialIoUartDebugEnable) {
			mupd->FspmConfig.serialDebugMsgLvl = get_int_from_vpd_range(
				FSP_MEM_LOG_LEVEL, FSP_MEM_LOG_LEVEL_DEFAULT, 0, 4);
			/* If serialDebugMsgLvl less than 1, disable FSP memory train results */
			if (mupd->FspmConfig.serialDebugMsgLvl <= 1) {
				printk(BIOS_DEBUG, "Setting serialDebugMsgLvlTrainResults to 0\n");
				mupd->FspmConfig.serialDebugMsgLvlTrainResults = 0x0;
			}
		}

		/* FSP Dfx PMIC Secure mode */
		mupd->FspmConfig.DfxPmicSecureMode = get_int_from_vpd_range(
			FSP_PMIC_SECURE_MODE, FSP_PMIC_SECURE_MODE_DEFAULT, 0, 2);
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
	soc_config_iio(mupd, ac_iio_pci_port, ac_iio_bifur);
	mainboard_config_iio(mupd);
}
