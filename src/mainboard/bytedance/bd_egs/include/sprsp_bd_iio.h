/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SPRSP_BD_IIO_H_
#define _SPRSP_BD_IIO_H_

#include <defs_iio.h>

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
	}

/*
 * ByteDance IIO PCIe Port Table
 */
static const UPD_IIO_PCIE_PORT_CONFIG_ENTRY bd_iio_pci_port_skt0[] = {
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(0, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 */
	CFG_UPD_PCIE_PORT(0, 1, 101), /* 15:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 103), /* 15:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 105), /* 15:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 107), /* 15:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 */
	CFG_UPD_PCIE_PORT(0, 1, 109), /* 26:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 111), /* 26:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 113), /* 26:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 115), /* 26:07.0 */
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
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 129),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 131),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 */
	CFG_UPD_PCIE_PORT(0, 1, 133), /* 59:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 135), /* 59:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 137), /* 59:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 139), /* 59:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* Bytedance doesn't use IOU5 ~ IOU6. */
};

static const UPD_IIO_PCIE_PORT_CONFIG_ENTRY bd_iio_pci_port_skt1[] = {
	/* DMI port: array index 0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU0 (PE0): array index 1 ~ 8 */
	CFG_UPD_PCIE_PORT(0, 1, 41), /* 97:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU1 (PE1): array index 9 ~ 16 */
	CFG_UPD_PCIE_PORT(0, 1, 149), /* a7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 151), /* a7:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 153), /* a7:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 155), /* a7:07.0 */
	CFG_UPD_PCIE_PORT(0, 0, 0),
	/* IOU2 (PE2): array index 17 ~ 24 */
	CFG_UPD_PCIE_PORT(0, 1, 157), /* b7:01.0 */ //only use 1 x4.
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 1, 100),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 1, 100),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(1, 1, 100),
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU3 (PE3): array index 25 ~ 32 */
	CFG_UPD_PCIE_PORT(0, 1, 165), /* c7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 167), /* c7:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 169), /* c7:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 171), /* c7:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* IOU4 (PE4): array index 33 ~ 40 */
	CFG_UPD_PCIE_PORT(0, 1, 173), /* d7:01.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 175), /* d7:03.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 177), /* d7:05.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	CFG_UPD_PCIE_PORT(0, 1, 179), /* d7:07.0 */
	CFG_UPD_PCIE_PORT(1, 0, 0),
	/* Bytedance doesn't use IOU5 ~ IOU6. */
};
#endif /* _SPRSP_BD_IIO_H_ */
