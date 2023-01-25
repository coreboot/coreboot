/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>

#define DUMP_UPD(old, new, field)                                                              \
	fsp_display_upd_value(#field, sizeof(old->field), old->field, new->field)

static void soc_display_fspm_upd_iio(const FSPM_UPD *mupd)
{
	int port, socket;

	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig =
		(UPD_IIO_PCIE_PORT_CONFIG *)mupd->FspmConfig.IioPcieConfigTablePtr;

	printk(BIOS_SPEW, "UPD values for IIO:\n");
	for (socket = 0; socket < mupd->FspmConfig.IioPcieConfigTableNumber; socket++) {
		printk(BIOS_SPEW, "Socket: %d\n", socket);
		for (port = 0; port < MAX_IIO_PORTS_PER_SOCKET; port++) {
			printk(BIOS_SPEW, "port: %d\n", port);

			printk(BIOS_SPEW, "\tSLOTEIP: 0x%x\n",
			       PciePortConfig[socket].SLOTEIP[port]);
			printk(BIOS_SPEW, "\tSLOTHPCAP: 0x%x\n",
			       PciePortConfig[socket].SLOTHPCAP[port]);
			printk(BIOS_SPEW, "\tSLOTHPSUP: 0x%x\n",
			       PciePortConfig[socket].SLOTHPSUP[port]);
			printk(BIOS_SPEW, "\tSLOTPIP: 0x%x\n",
			       PciePortConfig[socket].SLOTPIP[port]);
			printk(BIOS_SPEW, "\tSLOTAIP: 0x%x\n",
			       PciePortConfig[socket].SLOTAIP[port]);
			printk(BIOS_SPEW, "\tSLOTMRLSP: 0x%x\n",
			       PciePortConfig[socket].SLOTMRLSP[port]);
			printk(BIOS_SPEW, "\tSLOTPCP: 0x%x\n",
			       PciePortConfig[socket].SLOTPCP[port]);
			printk(BIOS_SPEW, "\tSLOTABP: 0x%x\n",
			       PciePortConfig[socket].SLOTABP[port]);
			printk(BIOS_SPEW, "\tSLOTIMP: 0x%x\n",
			       PciePortConfig[socket].SLOTIMP[port]);
			printk(BIOS_SPEW, "\tSLOTSPLS: 0x%x\n",
			       PciePortConfig[socket].SLOTSPLS[port]);
			printk(BIOS_SPEW, "\tSLOTSPLV: 0x%x\n",
			       PciePortConfig[socket].SLOTSPLV[port]);
			printk(BIOS_SPEW, "\tSLOTPSP: 0x%x\n",
			       PciePortConfig[socket].SLOTPSP[port]);
			printk(BIOS_SPEW, "\tVppEnabled: 0x%x\n",
			       PciePortConfig[socket].VppEnabled[port]);
			printk(BIOS_SPEW, "\tVppPort: 0x%x\n",
			       PciePortConfig[socket].VppPort[port]);
			printk(BIOS_SPEW, "\tVppAddress: 0x%x\n",
			       PciePortConfig[socket].VppAddress[port]);
			printk(BIOS_SPEW, "\tMuxAddress: 0x%x\n",
			       PciePortConfig[socket].MuxAddress[port]);
			printk(BIOS_SPEW, "\tChannelID: 0x%x\n",
			       PciePortConfig[socket].ChannelID[port]);
			printk(BIOS_SPEW, "\tPciePortEnable: 0x%x\n",
			       PciePortConfig[socket].PciePortEnable[port]);
			printk(BIOS_SPEW, "\tPEXPHIDE: 0x%x\n",
			       PciePortConfig[socket].PEXPHIDE[port]);
			printk(BIOS_SPEW, "\tHidePEXPMenu: 0x%x\n",
			       PciePortConfig[socket].HidePEXPMenu[port]);
			printk(BIOS_SPEW, "\tPciePortOwnership: 0x%x\n",
			       PciePortConfig[socket].PciePortOwnership[port]);
			printk(BIOS_SPEW, "\tRetimerConnectCount: 0x%x\n",
			       PciePortConfig[socket].RetimerConnectCount[port]);
			printk(BIOS_SPEW, "\tPcieHotPlugOnPort: 0x%x\n",
			       PciePortConfig[socket].PcieHotPlugOnPort[port]);
			printk(BIOS_SPEW, "\tVMDPortEnable: 0x%x\n",
			       PciePortConfig[socket].VMDPortEnable[port]);
			printk(BIOS_SPEW, "\tPcieMaxPayload: 0x%x\n",
			       PciePortConfig[socket].PcieMaxPayload[port]);
			printk(BIOS_SPEW, "\tPciePortLinkSpeed: 0x%x\n",
			       PciePortConfig[socket].PciePortLinkSpeed[port]);
			printk(BIOS_SPEW, "\tDfxDnTxPresetGen3: 0x%x\n",
			       PciePortConfig[socket].DfxDnTxPresetGen3[port]);
		}

		for (port = 0; port < MAX_VMD_STACKS_PER_SOCKET; port++) {
			printk(BIOS_SPEW, "port: %d\n", port);
			printk(BIOS_SPEW, "\tVMDEnabled: 0x%x\n",
			       PciePortConfig[socket].VMDEnabled[port]);
			printk(BIOS_SPEW, "\tVMDHotPlugEnable: 0x%x\n",
			       PciePortConfig[socket].VMDHotPlugEnable[port]);
		}
		printk(BIOS_SPEW, "ConfigIOU[0]: 0x%x\n", PciePortConfig[socket].ConfigIOU[0]);
		printk(BIOS_SPEW, "ConfigIOU[1]: 0x%x\n", PciePortConfig[socket].ConfigIOU[1]);
		printk(BIOS_SPEW, "ConfigIOU[2]: 0x%x\n", PciePortConfig[socket].ConfigIOU[2]);
		printk(BIOS_SPEW, "ConfigIOU[3]: 0x%x\n", PciePortConfig[socket].ConfigIOU[3]);
		printk(BIOS_SPEW, "ConfigIOU[4]: 0x%x\n", PciePortConfig[socket].ConfigIOU[4]);
		printk(BIOS_SPEW, "PcieGlobalAspm: 0x%x\n",
		       PciePortConfig[socket].PcieGlobalAspm);
		printk(BIOS_SPEW, "PcieMaxReadRequestSize: 0x%x\n",
		       PciePortConfig[socket].PcieMaxReadRequestSize);
	}

	UINT8 *DeEmphasisConfig = (UINT8 *)mupd->FspmConfig.DeEmphasisPtr;
	for (port = 0; port < mupd->FspmConfig.DeEmphasisNumber; port++) {
		printk(BIOS_SPEW, "port: %d, DeEmphasisConfig: 0x%x\n", port,
		       DeEmphasisConfig[port]);
	}
}

/* Display the UPD parameters for MemoryInit */
void soc_display_fspm_upd_params(const FSPM_UPD *fspm_old_upd, const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *new;
	const FSP_M_CONFIG *old;

	old = &fspm_old_upd->FspmConfig;
	new = &fspm_new_upd->FspmConfig;

	printk(BIOS_DEBUG, "UPD values for MemoryInit:\n");
	DUMP_UPD(old, new, DebugPrintLevel);
	DUMP_UPD(old, new, DfxCxlHeaderBypass);
	DUMP_UPD(old, new, DfxCxlSecLvl);

	printk(BIOS_DEBUG, " Dump of original MemoryInit UPD:\n");
	hexdump(fspm_old_upd, sizeof(*fspm_old_upd));

	printk(BIOS_DEBUG, " Dump of updated MemoryInit UPD:\n");
	hexdump(fspm_new_upd, sizeof(*fspm_new_upd));
	if (CONFIG(DISPLAY_UPD_IIO_DATA))
		soc_display_fspm_upd_iio(fspm_new_upd);
}

/* Display the UPD parameters for SiliconInit */
void soc_display_fsps_upd_params(const FSPS_UPD *fsps_old_upd, const FSPS_UPD *fsps_new_upd)
{
	const FSP_S_CONFIG *new;
	const FSP_S_CONFIG *old;

	old = &fsps_old_upd->FspsConfig;
	new = &fsps_new_upd->FspsConfig;

	printk(BIOS_DEBUG, "UPD values for SiliconInit:\n");

	hexdump(fsps_new_upd, sizeof(*fsps_new_upd));
}
