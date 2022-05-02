/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/romstage.h>
#include <defs_cxl.h>
#include <hob_iiouds.h>


/* For now only set 3 fields and hard-coded others, should be extended in the future */
#define CFG_UPD_PCIE_PORT(pexphide, slotimp, slotpsp)	\
	{						\
		.SLOTEIP = 0,				\
		.SLOTHPCAP = slotimp,			\
		.SLOTHPSUP = slotimp,			\
		.SLOTPIP = 0,				\
		.SLOTAIP = 0,				\
		.SLOTMRLSP = 0,				\
		.SLOTPCP = 0,				\
		.SLOTABP = 0,				\
		.SLOTIMP = slotimp,			\
		.SLOTSPLS = 0,				\
		.SLOTSPLV = slotimp ? 25 : 0,		\
		.SLOTPSP = slotpsp,			\
		.VppEnabled = 0,			\
		.VppPort = 0,				\
		.VppAddress = 0,			\
		.MuxAddress = 0,			\
		.ChannelID = 0,				\
		.PciePortEnable = !pexphide,		\
		.PEXPHIDE = pexphide,			\
		.HidePEXPMenu = pexphide,		\
		.PciePortOwnership = 0,			\
		.RetimerConnectCount = 0,		\
		.PcieMaxPayload = 0x7,			\
		.PcieHotPlugOnPort = slotimp,		\
	}

#define IIO_PORT_SETTINGS (1 + 5 * 8)

static const UPD_IIO_PCIE_PORT_CONFIG_ENTRY
sbp1_socket_config[CONFIG_MAX_SOCKET][IIO_PORT_SETTINGS] = {
	{
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(0, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 Not Used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 12), /* 26:01.0 RSSD12 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 11), /* 26:03.0 RSSD11 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 10), /* 26:05.0 RSSD10 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 9),  /* 26:07.0 RSSD09 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 13), /* 37:01.0 RSSD13 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 14), /* 37:03.0 RSSD14 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 15), /* 37:05.0 RSSD15 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 16), /* 37:07.0 RSSD16 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 48:01.0 - NIC2*/
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 48:05.0 - NIC1 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 Not Used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	},
	{
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 Not Used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 28), /* 26:01.0 RSSD28 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 27), /* 26:03.0 RSSD27 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 26), /* 26:05.0 RSSD26 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 25), /* 26:07.0 RSSD25 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 29), /* 37:01.0 RSSD29 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 30), /* 37:03.0 RSSD30 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 31), /* 37:05.0 RSSD31 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 32), /* 37:07.0 RSSD32 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 Not used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 59:01.0 - NIC2 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 59:05.0 - NIC1 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	},
	{
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 Not Used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 Not used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 37:01.0 - NIC1 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 37:05.0 - NIC2 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 17), /* 48:01.0 - RSSD17 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 18), /* 48:03.0 - RSSD18 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 19), /* 48:05.0 - RSSD19 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 20), /* 48:07.0 - RSSD20 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 24), /* 59:01.0 - RSSD24 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 23), /* 59:03.0 - RSSD23*/
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 22), /* 59:05.0 - RSSD22 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 21), /* 59:07.0 - RSSD21 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	},
	{
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 Not Used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 Not used */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 37:01.0 - NIC1 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 0, 0), /* 37:05.0 - NIC2 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 1), /* 48:01.0 - RSSD01 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 2), /* 48:03.0 - RSSD02 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 3), /* 48:05.0 - RSSD03 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 4), /* 48:07.0 - RSSD04 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 IIO_BIFURCATE_x4x4x4x4 */
	CFG_UPD_PCIE_PORT(0, 1, 8), /* 59:01.0 - RSSD08 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 7), /* 59:03.0 - RSSD07*/
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 6), /* 59:05.0 - RSSD06 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 5), /* 59:07.0 - RSSD05 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	},
};

static const UINT8 sbp1_socket_config_iou[CONFIG_MAX_SOCKET][5] = {
	{
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_xxxxxxxx,
	},
	{
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_x4x4x4x4,
	},
	{
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
	},
	{
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_xxxxxxxx,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
	},
};

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig;
	int port, socket;

	PciePortConfig = (UPD_IIO_PCIE_PORT_CONFIG *)(UINTN)mupd->FspmConfig.IioPcieConfigTablePtr;
	assert(mupd->FspmConfig.IioPcieConfigTableNumber == CONFIG_MAX_SOCKET);

	for (socket = 0; socket < mupd->FspmConfig.IioPcieConfigTableNumber; socket++) {

		/* Array sbp1_socket_config only configures DMI, IOU0 ~ IOU4, the rest will be left zero */
		for (port = 0; port < IIO_PORT_SETTINGS; port++) {
			const UPD_IIO_PCIE_PORT_CONFIG_ENTRY *port_cfg = &sbp1_socket_config[socket][port];
			PciePortConfig[socket].SLOTIMP[port] = port_cfg->SLOTIMP;
			PciePortConfig[socket].SLOTPSP[port] = port_cfg->SLOTPSP;
			PciePortConfig[socket].SLOTHPCAP[port] = port_cfg->SLOTHPCAP;
			PciePortConfig[socket].SLOTHPSUP[port] = port_cfg->SLOTHPSUP;
			PciePortConfig[socket].SLOTSPLS[port] = port_cfg->SLOTSPLS;
			PciePortConfig[socket].SLOTSPLV[port] = port_cfg->SLOTSPLV;
			PciePortConfig[socket].VppAddress[port] = port_cfg->VppAddress;
			PciePortConfig[socket].SLOTPIP[port] = port_cfg->SLOTPIP;
			PciePortConfig[socket].SLOTAIP[port] = port_cfg->SLOTAIP;
			PciePortConfig[socket].SLOTMRLSP[port] = port_cfg->SLOTMRLSP;
			PciePortConfig[socket].SLOTPCP[port] = port_cfg->SLOTPCP;
			PciePortConfig[socket].SLOTABP[port] = port_cfg->SLOTABP;
			PciePortConfig[socket].VppEnabled[port] = port_cfg->VppEnabled;
			PciePortConfig[socket].VppPort[port] = port_cfg->VppPort;
			PciePortConfig[socket].MuxAddress[port] = port_cfg->MuxAddress;
			PciePortConfig[socket].PciePortEnable[port] = port_cfg->PciePortEnable;
			PciePortConfig[socket].PEXPHIDE[port] = port_cfg->PEXPHIDE;
			PciePortConfig[socket].PcieHotPlugOnPort[port] = port_cfg->PcieHotPlugOnPort;
			PciePortConfig[socket].PcieMaxPayload[port] = port_cfg->PcieMaxPayload;
		}
		/* Socket0: IOU5 ~ IOU6 are not used, set PEXPHIDE and HidePEXPMenu to 1 */
		for (port = IIO_PORT_SETTINGS; port < MAX_IIO_PORTS_PER_SOCKET; port++) {
			PciePortConfig[socket].PEXPHIDE[port] = 1;
			PciePortConfig[socket].HidePEXPMenu[port] = 1;
			PciePortConfig[socket].PciePortEnable[port] = 0;
		}
		for (port = 0; port < 5; port++)
			PciePortConfig[socket].ConfigIOU[port] = sbp1_socket_config_iou[socket][port];
	}
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	UINT32 *sktbmp;

	/* Set Rank Margin Tool to disable. */
	mupd->FspmConfig.EnableRMT = 0x0;

	/* Set Promote Warnings to disable. */
	/* Determines if warnings are promoted to system level. */
	mupd->FspmConfig.promoteWarnings = 0x0;

	/* Set FSP debug message to Disable */
	mupd->FspmConfig.serialDebugMsgLvl = 0x0;

	/* Force 256MiB MMCONF (Segment0) only */
	mupd->FspmConfig.mmCfgSize = 0x2;
	mupd->FspmConfig.PcieHotPlugEnable = 1;

	/*
	 * Disable unused IIO stack:
	 * Socket 0 : IIO1, IIO4
	 * Socket 1 : IIO1, IIO2
	 * Socket 2 : IIO1, IIO5
	 * Socket 3 : IIO1, IIO5
	 * Stack Disable bit mapping is:
	 * IIO stack number:  1 2 3 4 5
	 * Stack Disable Bit: 1 5 3 2 4
	 */
	sktbmp = (UINT32 *)&mupd->FspmConfig.StackDisableBitMap[0];
	sktbmp[0] = BIT(1) | BIT(2);
	sktbmp[1] = BIT(1) | BIT(5);
	sktbmp[2] = BIT(1) | BIT(4);
	sktbmp[3] = BIT(1) | BIT(4);
	mainboard_config_iio(mupd);
}
