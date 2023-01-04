/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>
#include <soc/romstage.h>
#include <defs_cxl.h>
#include <defs_iio.h>
#include <sprsp_ac_iio.h>

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	int port;

	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig =
		(UPD_IIO_PCIE_PORT_CONFIG *)mupd->FspmConfig.IioPcieConfigTablePtr;

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
	PciePortConfig[0].ConfigIOU[3] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[0].ConfigIOU[4] = IIO_BIFURCATE_x4x4x4x4;

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
	PciePortConfig[1].ConfigIOU[0] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[1] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[2] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[3] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[1].ConfigIOU[4] = IIO_BIFURCATE_x4x4x4x4;

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

	mainboard_config_iio(mupd);
}
