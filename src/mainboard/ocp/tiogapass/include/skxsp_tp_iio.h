/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SKXSP_TP_IIO_H_
#define _SKXSP_TP_IIO_H_

#include <FspmUpd.h>
#include <soc/pci_devs.h>

enum tp_iio_bifur_table_index {
	Skt0_Iou0 = 0,
	Skt0_Iou1,
	Skt0_Iou2,
	Skt0_Mcp0,
	Skt0_Mcp1,
	Skt1_Iou0,
	Skt1_Iou1,
	Skt1_Iou2,
	Skt1_Mcp0,
	Skt1_Mcp1
};

/*
 * Standard Tioga Pass Iio Bifurcation Table
 * This is SS 2x16 config. As documented in OCP TP spec, there are
 * 3 configs. SS 2x16 is the most common.
 * TODO: figure out config through board SKU ID and through PCIe
 * config GPIO setting (SLT_CFG0 / SLT_CFG1).
 */
static const UPD_IIO_BIFURCATION_DATA_ENTRY tp_iio_bifur_table[] = {
	{ Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 }, /* 1A x16 */
	{ Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 }, /* 2A x16 */
	{ Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 }, /* 3A x16 */
	{ Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxxxx }, /* No MCP */
	{ Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxxxx }, /* No MCP */
	{ Iio_Socket1, Iio_Iou0, IIO_BIFURCATE_xxxxxxxx }, /* no IOU0 */
	{ Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxxxx }, /* no IOU1 */
	{ Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 }, /* 3A x8, 3C x8 */
	{ Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxxxx }, /* No MCP */
	{ Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxxxx }, /* No MCP */
};

#define CFG_UPD_PORT(port, hide)				\
	{							\
		.PortIndex           = port,			\
		.HidePort            = hide,			\
		.DeEmphasis          = 0x00,			\
		.PortLinkSpeed       = PcieAuto,		\
		.MaxPayload          = 0x00,			\
		.DfxDnTxPreset       = 0xFF,			\
		.DfxRxPreset         = 0xFF,			\
		.DfxUpTxPreset       = 0xFF,			\
		.Sris                = 0x00,			\
		.PcieCommonClock     = 0x00,			\
		.NtbPpd              = NTB_PORT_TRANSPARENT,	\
		.NtbSplitBar         = 0x00,			\
		.NtbBarSizePBar23    = 0x16,			\
		.NtbBarSizePBar4     = 0x16,			\
		.NtbBarSizePBar5     = 0x16,			\
		.NtbBarSizePBar45    = 0x16,			\
		.NtbBarSizeSBar23    = 0x16,			\
		.NtbBarSizeSBar4     = 0x16,			\
		.NtbBarSizeSBar5     = 0x16,			\
		.NtbBarSizeSBar45    = 0x16,			\
		.NtbSBar01Prefetch   = 0x00,			\
		.NtbXlinkCtlOverride = 0x03,			\
	}

/*
 * Standard Tioga Pass Iio PCIe Port Table
 */
static const UPD_PCI_PORT_CONFIG tp_iio_pci_port_skt0[] = {
	CFG_UPD_PORT(PORT_1A, NOT_HIDE),
	CFG_UPD_PORT(PORT_1B, HIDE),
	CFG_UPD_PORT(PORT_1C, HIDE),
	CFG_UPD_PORT(PORT_1D, HIDE),
	CFG_UPD_PORT(PORT_2A, NOT_HIDE),
	CFG_UPD_PORT(PORT_2B, HIDE),
	CFG_UPD_PORT(PORT_2C, HIDE),
	CFG_UPD_PORT(PORT_2D, HIDE),
	CFG_UPD_PORT(PORT_3A, NOT_HIDE),
	CFG_UPD_PORT(PORT_3B, HIDE),
	CFG_UPD_PORT(PORT_3C, NOT_HIDE),
	CFG_UPD_PORT(PORT_3D, HIDE),
};

/*
 * Standard Tioga Pass PCH PCIe Port Table
 */
static const UPD_PCH_PCIE_PORT tp_pch_pci_port_skt0[] = {
	//PortIndex ; ForceEnable ; PortLinkSpeed
	{ 0x00, 0x00, PcieAuto },
	{ 0x04, 0x00, PcieAuto },
	{ 0x05, 0x00, PcieAuto },
};

#endif /* _SKXSP_TP_IIO_H_ */
