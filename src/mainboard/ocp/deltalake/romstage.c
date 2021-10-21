/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <console/uart.h>
#include <drivers/ipmi/ipmi_if.h>
#include <drivers/ipmi/ocp/ipmi_ocp.h>
#include <drivers/vpd/vpd.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <soc/romstage.h>
#include <FspmUpdHelper.h>

#include "chip.h"
#include "ipmi.h"
#include "vpd.h"

/* Convert the vpd integer to the DDR frenquency limit enum */
static enum ddr_freq_limit ddr_freq_limit(int num)
{
	switch (num) {
	case 0:
		return DDR_AUTO;
	case 1:
		return DDR_1333;
	case 2:
		return DDR_1600;
	case 3:
		return DDR_1866;
	case 4:
		return DDR_2133;
	case 5:
		return DDR_2400;
	case 6:
		return DDR_2666;
	case 7:
		return DDR_2933;
	case 8:
		return DDR_3200;
	default:
		printk(BIOS_WARNING, "Invalid DdrFreqLimit value from VPD: "
			"%d\n", num);
		return DDR_AUTO;
	};
}

/*
 * Search from VPD_RW first then VPD_RO for UPD config variables,
 * overwrites them from VPD if it's found.
 */
static void mainboard_config_upd(FSPM_UPD *mupd)
{
	uint8_t val;
	int val_int;

	/* Send FSP log message to SOL */
	if (vpd_get_bool(FSP_LOG, VPD_RW_THEN_RO, &val))
		mupd->FspmConfig.SerialIoUartDebugEnable = val;
	else {
		printk(BIOS_INFO, "Not able to get VPD %s, default set "
			"SerialIoUartDebugEnable to %d\n", FSP_LOG, FSP_LOG_DEFAULT);
		mupd->FspmConfig.SerialIoUartDebugEnable = FSP_LOG_DEFAULT;
	}

	/* Select UART IO of FSP */
	static const unsigned int bases[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
	mupd->FspmConfig.SerialIoUartDebugIoBase = bases[get_uart_for_console()];

	if (mupd->FspmConfig.SerialIoUartDebugEnable) {
		/* FSP debug log level */
		if (vpd_get_int(FSP_LOG_LEVEL, VPD_RW_THEN_RO, &val_int)) {
			if (val_int < 0 || val_int > 0x0f) {
				printk(BIOS_DEBUG, "Invalid DebugPrintLevel value from VPD: "
					"%d\n", val_int);
				val_int = FSP_LOG_LEVEL_DEFAULT;
			}
			printk(BIOS_DEBUG, "Setting DebugPrintLevel %d from VPD\n", val_int);
			mupd->FspmConfig.DebugPrintLevel = (uint8_t)val_int;
		} else {
			printk(BIOS_INFO, "Not able to get VPD %s, default set "
				"DebugPrintLevel to %d\n", FSP_LOG_LEVEL,
				FSP_LOG_LEVEL_DEFAULT);
			mupd->FspmConfig.DebugPrintLevel = FSP_LOG_LEVEL_DEFAULT;
		}
	}

	/* Enable DCI */
	if (vpd_get_bool(FSP_DCI, VPD_RW_THEN_RO, &val)) {
		printk(BIOS_DEBUG, "Setting DciEn %d from VPD\n", val);
		mupd->FspmConfig.PchDciEn = val;
	} else {
		printk(BIOS_INFO, "Not able to get VPD %s, default set "
			"DciEn to %d\n", FSP_DCI, FSP_DCI_DEFAULT);
		mupd->FspmConfig.PchDciEn = FSP_DCI_DEFAULT;
	}

	/*
	 * UnusedUpdSpace0[0] is reserved for Memory Refresh Watermark.
	 * Following code is effective when MemRefreshWaterMark patch is added to FSP
	 * and when corresponding VPD variable is set.
	 */
	if (vpd_get_int(FSPM_MEMREFRESHWATERMARK, VPD_RW_THEN_RO, &val_int)) {
		if (val_int < 0 || val_int > 2) {
			printk(BIOS_DEBUG, "Invalid MemRefreshWatermark value from VPD: "
				"%d\n", val_int);
			val_int = FSPM_MEMREFRESHWATERMARK_DEFAULT;
		}
		printk(BIOS_DEBUG, "Setting MemRefreshWatermark %d from VPD\n", val_int);
		mupd->FspmConfig.UnusedUpdSpace0[0] = (uint8_t)val_int;
	}

	/* Select DDR Frequency Limit */
	if (vpd_get_int(FSP_DIMM_FREQ, VPD_RW_THEN_RO, (int *const) &val_int)) {
		printk(BIOS_INFO, "Setting DdrFreqLimit %d from VPD\n", val_int);
		mupd->FspmConfig.DdrFreqLimit = ddr_freq_limit(val_int);
	} else {
		printk(BIOS_WARNING, "Not able to get VPD %s, default set "
			"DdrFreqLimit to %d\n", FSP_DIMM_FREQ,
			FSP_DIMM_FREQ_DEFAULT);
		mupd->FspmConfig.DdrFreqLimit = ddr_freq_limit(FSP_DIMM_FREQ_DEFAULT);
	}
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
	uint8_t val;

	/* Since it's the first IPMI command, it's better to run get BMC
	   selftest result first */
	if (ipmi_premem_init(CONFIG_BMC_KCS_BASE, 0) == CB_SUCCESS) {
		ipmi_set_post_start(CONFIG_BMC_KCS_BASE);
		init_frb2_wdt();
	}

	/* Enable force memory training */
	if (vpd_get_bool(MEM_TRAIN_FORCE, VPD_RW_THEN_RO, &val)) {
		if (mupd->FspmArchUpd.NvsBufferPtr && val) {
			mupd->FspmArchUpd.NvsBufferPtr = 0;
			printk(BIOS_DEBUG, "Force Memory Training...Start\n");
		}
	}

	mainboard_config_gpios(mupd);
	mainboard_config_iio(mupd);
	mainboard_config_upd(mupd);
}

void mainboard_rtc_failed(void)
{
	if (ipmi_set_cmos_clear() == CB_SUCCESS)
		printk(BIOS_DEBUG, "%s: IPMI set cmos clear successful\n", __func__);
	else
		printk(BIOS_ERR, "%s: IPMI set cmos clear failed\n", __func__);
}
