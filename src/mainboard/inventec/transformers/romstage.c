/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>
#include <drivers/ipmi/ipmi_if.h>
#include <drivers/ipmi/ocp/ipmi_ocp.h>
#include <drivers/ocp/ewl/ocp_ewl.h>
#include <soc/config.h>
#include <soc/romstage.h>
#include <defs_cxl.h>
#include <defs_iio.h>
#include <sprsp_ac_iio.h>
#include <stdint.h>
#include <IioPcieConfigUpd.h>

#include "ipmi.h"

void mainboard_ewl_check(void)
{
	/* Enhanced Warning Log prints EWL type 3 primarily associated with MRC training failures */
	get_ewl();
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	int port;

	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig =
		(UPD_IIO_PCIE_PORT_CONFIG *)(uintptr_t)mupd->FspmConfig.IioPcieConfigTablePtr;

	/* Socket0: Array ac_iio_pci_port_skt0 only configures DMI, IOU0 ~ IOU4, the rest will be left zero */
	for (port = 0; port < ARRAY_SIZE(ac_iio_pci_port_skt0); port++) {
		PciePortConfig[0].SLOTIMP[port] = ac_iio_pci_port_skt0[port].SLOTIMP;
		PciePortConfig[0].SLOTPSP[port] = ac_iio_pci_port_skt0[port].SLOTPSP;
		PciePortConfig[0].PciePortEnable[port] = ac_iio_pci_port_skt0[port].PciePortEnable;
		PciePortConfig[0].PEXPHIDE[port] = ac_iio_pci_port_skt0[port].PEXPHIDE;
		PciePortConfig[0].PcieMaxPayload[port] = ac_iio_pci_port_skt0[port].PcieMaxPayload;
		PciePortConfig[0].PciePortLinkSpeed[port] = ac_iio_pci_port_skt0[port].PciePortLinkSpeed;
		PciePortConfig[0].DfxDnTxPresetGen3[port] = ac_iio_pci_port_skt0[port].DfxDnTxPresetGen3;
	}
	/* Socket0: IOU5 ~ IOU6 are not used, set PEXPHIDE and HidePEXPMenu to 1 */
	for (port = ARRAY_SIZE(ac_iio_pci_port_skt0); port < MAX_IIO_PORTS_PER_SOCKET; port++) {
		PciePortConfig[0].PEXPHIDE[port] = 1;
		PciePortConfig[0].HidePEXPMenu[port] = 1;
	}
	PciePortConfig[0].ConfigIOU[0] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[0].ConfigIOU[1] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[0].ConfigIOU[2] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[0].ConfigIOU[3] = IIO_BIFURCATE_xxx8xxx8;
	PciePortConfig[0].ConfigIOU[4] = IIO_BIFURCATE_xxxxxx16;

	PciePortConfig[0].PcieGlobalAspm = 0x1;
	PciePortConfig[0].PcieMaxReadRequestSize = 0x5;

	/* Socket1: Array ac_iio_pci_port_skt1 only configures DMI, IOU0 ~ IOU4, the rest will be left zero */
	for (port = 0; port < ARRAY_SIZE(ac_iio_pci_port_skt1); port++) {
		PciePortConfig[1].SLOTIMP[port] = ac_iio_pci_port_skt1[port].SLOTIMP;
		PciePortConfig[1].SLOTPSP[port] = ac_iio_pci_port_skt1[port].SLOTPSP;
		PciePortConfig[1].PciePortEnable[port] = ac_iio_pci_port_skt1[port].PciePortEnable;
		PciePortConfig[1].PEXPHIDE[port] = ac_iio_pci_port_skt1[port].PEXPHIDE;
		PciePortConfig[1].PcieMaxPayload[port] = ac_iio_pci_port_skt1[port].PcieMaxPayload;
		PciePortConfig[1].PciePortLinkSpeed[port] = ac_iio_pci_port_skt1[port].PciePortLinkSpeed;
		PciePortConfig[1].DfxDnTxPresetGen3[port] = ac_iio_pci_port_skt1[port].DfxDnTxPresetGen3;
	}
	/* Socket1: IOU5 ~ IOU6 are not used, set PEXPHIDE and HidePEXPMenu to 1 */
	for (port = ARRAY_SIZE(ac_iio_pci_port_skt1); port < MAX_IIO_PORTS_PER_SOCKET; port++) {
		PciePortConfig[1].PEXPHIDE[port] = 1;
		PciePortConfig[1].HidePEXPMenu[port] = 1;
	}
	PciePortConfig[1].ConfigIOU[0] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[1].ConfigIOU[1] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[2] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[3] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[4] = IIO_BIFURCATE_xxx8xxx8;

	PciePortConfig[1].PcieGlobalAspm = 0x1;
	PciePortConfig[1].PcieMaxReadRequestSize = 0x5;

	/* If CONFIG(OCP_VPD) is not enabled or CXL is explicitly disabled, don't enable CXL */
	if (!CONFIG(OCP_VPD) || get_cxl_mode_from_vpd() == CXL_DISABLED) {
		printk(BIOS_DEBUG, "Don't enable CXL via VPD %s\n", CXL_MODE);
	} else {
		/* Set socket 0 IIO PCIe PE1 to CXL mode */
		/* Set socket 1 IIO PCIe PE0 to CXL mode */
		/* eg. Protocl Auto Negotiation */
		mupd->FspmConfig.IioPcieSubSystemMode1[0] = IIO_MODE_CXL;
		mupd->FspmConfig.IioPcieSubSystemMode0[1] = IIO_MODE_CXL;

		/* Disable CXL header bypass */
		mupd->FspmConfig.DfxCxlHeaderBypass = 0;

		/* Set DFX CXL security level to fully trusted */
		mupd->FspmConfig.DfxCxlSecLvl = CXL_SECURITY_FULLY_TRUSTED;

		/* Set DelayAfterPCIeLinkTraining to 2000 ms */
		mupd->FspmConfig.DelayAfterPCIeLinkTraining = 2000;
	}
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Since it's the first IPMI command, it's better to run get BMC selftest result first */
	if (ipmi_premem_init(CONFIG_BMC_KCS_BASE, 0) == CB_SUCCESS) {
		init_frb2_wdt();
	}

	/* Setup FSP log */
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

	/* Set Rank Margin Tool to disable. */
	mupd->FspmConfig.EnableRMT = 0x0;
	/* Enable - Portions of memory reference code will be skipped when possible to increase boot speed on warm boots */
	/* Disable - Disables this feature */
	/*Auto - Sets it to the MRC default setting */
	mupd->FspmConfig.AttemptFastBoot = 0x1;
	/* Set Attempt Fast Cold Boot to enable */
	/* Enable - Portions of memory reference code will be skipped when possible to increase boot speed on cold boots */
	/* Disable - Disables this feature */
	/* Auto - Sets it to the MRC default setting */
	mupd->FspmConfig.AttemptFastBootCold = 0x1;

	/* Set Adv MemTest Option to 0. */
	mupd->FspmConfig.AdvMemTestOptions = 0x0;
	/* Set MRC Promote Warnings to disable. */
	/* Determines if MRC warnings are promoted to system level. */
	mupd->FspmConfig.promoteMrcWarnings = 0x0;
	/* Set Promote Warnings to disable. */
	/* Determines if warnings are promoted to system level. */
	mupd->FspmConfig.promoteWarnings = 0x0;

	/* Reduce FSP debug message to Minimum */
	mupd->FspmConfig.serialDebugMsgLvl = 0x1;

	/* Disable FSP memory train results */
	mupd->FspmConfig.serialDebugMsgLvlTrainResults = 0x0;

	mainboard_config_iio(mupd);
}
