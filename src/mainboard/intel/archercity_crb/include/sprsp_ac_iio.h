/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SPRSP_AC_IIO_H_
#define _SPRSP_AC_IIO_H_

#include <defs_iio.h>
#include <soc/soc_util.h>

/* For now only set 3 fields and hard-coded others, should be extended in the future */
#define CFG_UPD_PCIE_PORT(pexphide, slotimp, slotpsp)	\
	{						\
		.SLOTEIP = 0,				\
		.SLOTHPCAP = 0,				\
		.SLOTHPSUP = 0,				\
		.SLOTPIP = 0,				\
		.SLOTAIP = 0,				\
		.SLOTMRLSP = 0,				\
		.SLOTPCP = 0,				\
		.SLOTABP = 0,				\
		.SLOTIMP = slotimp,			\
		.SLOTSPLS = 0,				\
		.SLOTSPLV = 0,				\
		.SLOTPSP = slotpsp,			\
		.VppEnabled = 0,			\
		.VppPort = 0,				\
		.VppAddress = 0,			\
		.MuxAddress = 0,			\
		.ChannelID = 0,				\
		.PciePortEnable = 1,			\
		.PEXPHIDE = pexphide,			\
		.HidePEXPMenu = 0,			\
		.PciePortOwnership = 0,			\
		.RetimerConnectCount = 0,		\
		.PcieMaxPayload = 0x7,			\
		.PciePortLinkSpeed = 0,			\
		.DfxDnTxPresetGen3 = 0xFF		\
	}

/*
 * Standard ArcherCity IIO PCIe Port Table
 */
static const UPD_IIO_PCIE_PORT_CONFIG_ENTRY ac_iio_pci_port[CONFIG_MAX_SOCKET][IIO_PORT_SETTINGS] = {
	{
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(0, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 */
	CFG_UPD_PCIE_PORT(0, 1, 1), /* 15:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 */
	CFG_UPD_PCIE_PORT(0, 1, 9), /* 26:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 */
	CFG_UPD_PCIE_PORT(0, 1, 17), /* 37:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 */
	CFG_UPD_PCIE_PORT(0, 1, 25), /* 48:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 27), /* 48:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 29), /* 48:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 31), /* 48:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 */
	CFG_UPD_PCIE_PORT(0, 1, 33), /* 59:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 35), /* 59:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 37), /* 59:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 39), /* 59:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	},
	{
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 */
	CFG_UPD_PCIE_PORT(0, 1, 1), /* 97:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 */
	CFG_UPD_PCIE_PORT(0, 1, 9), /* a7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 */
	CFG_UPD_PCIE_PORT(0, 1, 17), /* b7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 */
	CFG_UPD_PCIE_PORT(0, 1, 25), /* c7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 27), /* c7:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 29), /* c7:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 31), /* c7:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 */
	CFG_UPD_PCIE_PORT(0, 1, 33), /* d7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 35), /* d7:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 37), /* d7:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 39), /* d7:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	},
};

static const UINT8 ac_iio_bifur[CONFIG_MAX_SOCKET][5] = {
	{
		IIO_BIFURCATE_xxxxxx16,
		IIO_BIFURCATE_xxxxxx16,
		IIO_BIFURCATE_xxxxxx16,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
	},
	{
		IIO_BIFURCATE_xxxxxx16,
		IIO_BIFURCATE_xxxxxx16,
		IIO_BIFURCATE_xxxxxx16,
		IIO_BIFURCATE_x4x4x4x4,
		IIO_BIFURCATE_x4x4x4x4,
	},
};
#endif /* _SPRSP_AC_IIO_H_ */
