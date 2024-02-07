/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>
#include <defs_cxl.h>
#include <defs_iio.h>
#include <sprsp_bd_iio.h>
#include <stdint.h>
#include <IioPcieConfigUpd.h>

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	int port;

	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig =
		(UPD_IIO_PCIE_PORT_CONFIG *)(uintptr_t)mupd->FspmConfig.IioPcieConfigTablePtr;

	/* Socket0: Array bd_iio_pci_port_skt0 only configures DMI, IOU0 ~ IOU4, the rest will be left zero */
	for (port = 0; port < ARRAY_SIZE(bd_iio_pci_port_skt0); port++) {
		PciePortConfig[0].SLOTIMP[port] = bd_iio_pci_port_skt0[port].SLOTIMP;
		PciePortConfig[0].SLOTPSP[port] = bd_iio_pci_port_skt0[port].SLOTPSP;
		PciePortConfig[0].PciePortEnable[port] = bd_iio_pci_port_skt0[port].PciePortEnable;
		PciePortConfig[0].PEXPHIDE[port] = bd_iio_pci_port_skt0[port].PEXPHIDE;
		PciePortConfig[0].SLOTHPCAP[port] = bd_iio_pci_port_skt0[port].SLOTHPCAP;
		PciePortConfig[0].VppPort[port] = bd_iio_pci_port_skt0[port].VppPort;
		PciePortConfig[0].VppAddress[port] = bd_iio_pci_port_skt0[port].VppAddress;
	}
	/* Socket0: IOU5 ~ IOU6 are not used, set PEXPHIDE and HidePEXPMenu to 1 */
	for (port = ARRAY_SIZE(bd_iio_pci_port_skt0); port < MAX_IIO_PORTS_PER_SOCKET; port++) {
		PciePortConfig[0].PEXPHIDE[port] = 1;
		PciePortConfig[0].HidePEXPMenu[port] = 1;
	}
	PciePortConfig[0].ConfigIOU[0] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[0].ConfigIOU[1] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[0].ConfigIOU[2] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[0].ConfigIOU[3] = IIO_BIFURCATE_x4x4xxx8;
	PciePortConfig[0].ConfigIOU[4] = IIO_BIFURCATE_x4x4x4x4;

	/* Socket1: Array bd_iio_pci_port_skt1 only configures DMI, IOU0 ~ IOU4, the rest will be left zero */
	for (port = 0; port < ARRAY_SIZE(bd_iio_pci_port_skt1); port++) {
		PciePortConfig[1].SLOTIMP[port] = bd_iio_pci_port_skt1[port].SLOTIMP;
		PciePortConfig[1].SLOTPSP[port] = bd_iio_pci_port_skt1[port].SLOTPSP;
		PciePortConfig[1].PciePortEnable[port] = bd_iio_pci_port_skt1[port].PciePortEnable;
		PciePortConfig[1].PEXPHIDE[port] = bd_iio_pci_port_skt1[port].PEXPHIDE;
		PciePortConfig[1].SLOTHPCAP[port] = bd_iio_pci_port_skt1[port].SLOTHPCAP;
		PciePortConfig[1].VppPort[port] = bd_iio_pci_port_skt1[port].VppPort;
		PciePortConfig[1].VppAddress[port] = bd_iio_pci_port_skt1[port].VppAddress;
	}
	/* Socket1: IOU5 ~ IOU6 are not used, set PEXPHIDE and HidePEXPMenu to 1 */
	for (port = ARRAY_SIZE(bd_iio_pci_port_skt1); port < MAX_IIO_PORTS_PER_SOCKET; port++) {
		PciePortConfig[1].PEXPHIDE[port] = 1;
		PciePortConfig[1].HidePEXPMenu[port] = 1;
	}
	PciePortConfig[1].ConfigIOU[0] = IIO_BIFURCATE_xxxxxx16;
	PciePortConfig[1].ConfigIOU[1] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[1].ConfigIOU[2] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[1].ConfigIOU[3] = IIO_BIFURCATE_x4x4x4x4;
	PciePortConfig[1].ConfigIOU[4] = IIO_BIFURCATE_x4x4x4x4;
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Disable CXL header bypass */
	mupd->FspmConfig.DfxCxlHeaderBypass = 0;

	/* Set DFX CXL security level to fully trusted */
	mupd->FspmConfig.DfxCxlSecLvl = CXL_SECURITY_FULLY_TRUSTED;

	/* Set DelayAfterPCIeLinkTraining to 2000 ms */
	mupd->FspmConfig.DelayAfterPCIeLinkTraining = 2000;

	mainboard_config_iio(mupd);
}
