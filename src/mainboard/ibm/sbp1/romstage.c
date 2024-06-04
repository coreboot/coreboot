/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/romstage.h>
#include <soc/ddr.h>
#include <soc/soc_util.h>
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
	CFG_UPD_PCIE_PORT(0, 1, 37), /* 48:01.0 - NIC2*/
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 33), /* 48:05.0 - NIC1 */
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
	CFG_UPD_PCIE_PORT(0, 1, 38), /* 59:01.0 - NIC2 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 34), /* 59:05.0 - NIC1 */
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
	CFG_UPD_PCIE_PORT(0, 1, 35), /* 37:01.0 - NIC1 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 39), /* 37:05.0 - NIC2 */
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
	CFG_UPD_PCIE_PORT(0, 1, 36), /* 37:01.0 - NIC1 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 40), /* 37:05.0 - NIC2 */
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

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Set Rank Margin Tool to disable. */
	mupd->FspmConfig.EnableRMT = 0x0;

	/* Set Promote Warnings to disable. */
	/* Determines if warnings are promoted to system level. */
	mupd->FspmConfig.promoteWarnings = 0x0;

	if (CONFIG(DEFAULT_CONSOLE_LOGLEVEL_7) ||
	    CONFIG(DEFAULT_CONSOLE_LOGLEVEL_8))
		mupd->FspmConfig.serialDebugMsgLvl = 3;
	else if (CONFIG(DEFAULT_CONSOLE_LOGLEVEL_6))
		mupd->FspmConfig.serialDebugMsgLvl = 1;
	else
		mupd->FspmConfig.serialDebugMsgLvl = 0;

	mupd->FspmConfig.PcieHotPlugEnable = 1;
	soc_config_iio(mupd, sbp1_socket_config, sbp1_socket_config_iou);
}

bool mainboard_dimm_slot_exists(uint8_t socket, uint8_t channel, uint8_t dimm)
{
	if (socket >= CONFIG_MAX_SOCKET)
		return false;
	if (channel >= 8)
		return false;
	if (dimm >= 2)
		return false;

	return true;
}
