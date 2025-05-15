/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/ocp/ewl/ocp_ewl.h>
#include <soc/romstage.h>
#include <soc/ddr.h>
#include <defs_cxl.h>
#include <defs_iio.h>
#include <mainboard_iio.h>

void mainboard_ewl_check(void)
{
	get_ewl();
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	/* Set socket 0 IIO PCIe PE0,PE1,PE2,PE3 to CXL mode */
	mupd->FspmConfig.IioPcieSubSystemMode0[0] = IIO_MODE_CXL;
	mupd->FspmConfig.IioPcieSubSystemMode1[0] = IIO_MODE_CXL;
	mupd->FspmConfig.IioPcieSubSystemMode2[0] = IIO_MODE_CXL;
	mupd->FspmConfig.IioPcieSubSystemMode3[0] = IIO_MODE_CXL;

	mupd->FspmConfig.DfxCxlHeaderBypass = 0;
	mupd->FspmConfig.DfxCxlSecLvl = CXL_SECURITY_FULLY_TRUSTED;

	mupd->FspmConfig.DelayAfterPCIeLinkTraining = 2000; /* ms */
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Send FSP log message to main serial port */
	mupd->FspmConfig.SerialIoUartDebugEnable = 1;
	mupd->FspmConfig.SerialIoUartDebugIoBase = CONFIG_TTYS0_BASE;

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
	soc_config_iio(mupd, iio_pci_port, iio_bifur);
	mainboard_config_iio(mupd);
}

bool mainboard_dimm_slot_exists(uint8_t socket, uint8_t channel, uint8_t dimm)
{
	if (socket >= CONFIG_MAX_SOCKET)
		return false;
	// SPC741D8 supports 8 channels with 1 DIMM each
	if (channel >= 8)
		return false;
	if (dimm >= 1)
		return false;

	return true;
}
