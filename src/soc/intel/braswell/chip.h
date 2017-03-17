/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree.
 */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <stdint.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>

#define SVID_CONFIG1		1
#define SVID_CONFIG3		3
#define SVID_PMIC_CONFIG	8

#define MEM_DDR3	0
#define MEM_LPDDR3	1

enum lpe_clk_src {
	LPE_CLK_SRC_XTAL,
	LPE_CLK_SRC_PLL,
};

enum usb_comp_bg_value {
	USB_COMP_BG_575_MV = 7,
	USB_COMP_BG_650_MV = 6,
	USB_COMP_BG_550_MV = 5,
	USB_COMP_BG_537_MV = 4,
	USB_COMP_BG_625_MV = 3,
	USB_COMP_BG_700_MV = 2,
	USB_COMP_BG_600_MV = 1,
	USB_COMP_BG_675_MV = 0,
};


struct soc_intel_braswell_config {
	uint8_t enable_xdp_tap;
	uint8_t clkreq_enable;

	/* Disable SLP_X stretching after SUS power well loss. */
	int disable_slp_x_stretch_sus_fail;

	/* LPE Audio Clock configuration. */
	enum lpe_clk_src lpe_codec_clk_src; /* 0=xtal 1=PLL, Both are 19.2Mhz */

	/* Native SD Card controller - override controller capabilities. */
	uint32_t sdcard_cap_low;
	uint32_t sdcard_cap_high;

	/* Enable devices in ACPI mode */
	int lpss_acpi_mode;
	int emmc_acpi_mode;
	int sd_acpi_mode;
	int lpe_acpi_mode;

	/* Allow PCIe devices to wake system from suspend. */
	int pcie_wake_enable;

	/* Program USB2_COMPBG register.
	 * [10:7] - select vref to AFE port
	 *  x111 - 575mV, x110 - 650mV, x101 - 550mV, x100 - 537.5mV,
	 *  x011 - 625mV, x010 - 700mV, x001 - 600mV, x000 - 675mV
	 */
	enum usb_comp_bg_value usb_comp_bg;


	/*
	 * The following fields come from fsp_vpd.h .aka. VpdHeader.h.
	 * These are configuration values that are passed to FSP during
	 * MemoryInit.
	 */
	UINT16 PcdMrcInitTsegSize;
	UINT16 PcdMrcInitMmioSize;
	UINT8  PcdMrcInitSpdAddr1;
	UINT8  PcdMrcInitSpdAddr2;
	UINT8  PcdIgdDvmt50PreAlloc;
	UINT8  PcdApertureSize;
	UINT8  PcdGttSize;
	UINT8  PcdLegacySegDecode;
	UINT8  PcdDvfsEnable;
	UINT8  PcdCaMirrorEn; /* Command Address Mirroring Enabled */

	/*
	 * The following fields come from fsp_vpd.h .aka. VpdHeader.h.
	 * These are configuration values that are passed to FSP during
	 * SiliconInit.
	 */
	UINT8  PcdSdcardMode;
	UINT8  PcdEnableHsuart0;
	UINT8  PcdEnableHsuart1;
	UINT8  PcdEnableAzalia;
	UINT8  PcdEnableSata;
	UINT8  PcdEnableXhci;
	UINT8  PcdEnableLpe;
	UINT8  PcdEnableDma0;
	UINT8  PcdEnableDma1;
	UINT8  PcdEnableI2C0;
	UINT8  PcdEnableI2C1;
	UINT8  PcdEnableI2C2;
	UINT8  PcdEnableI2C3;
	UINT8  PcdEnableI2C4;
	UINT8  PcdEnableI2C5;
	UINT8  PcdEnableI2C6;
	UINT8  PunitPwrConfigDisable;
	UINT8  ChvSvidConfig;
	UINT8  DptfDisable;
	UINT8  PcdEmmcMode;
	UINT8  PcdUsb3ClkSsc;
	UINT8  PcdDispClkSsc;
	UINT8  PcdSataClkSsc;
	UINT8  Usb2Port0PerPortPeTxiSet;
	UINT8  Usb2Port0PerPortTxiSet;
	UINT8  Usb2Port0IUsbTxEmphasisEn;
	UINT8  Usb2Port0PerPortTxPeHalf;
	UINT8  Usb2Port1PerPortPeTxiSet;
	UINT8  Usb2Port1PerPortTxiSet;
	UINT8  Usb2Port1IUsbTxEmphasisEn;
	UINT8  Usb2Port1PerPortTxPeHalf;
	UINT8  Usb2Port2PerPortPeTxiSet;
	UINT8  Usb2Port2PerPortTxiSet;
	UINT8  Usb2Port2IUsbTxEmphasisEn;
	UINT8  Usb2Port2PerPortTxPeHalf;
	UINT8  Usb2Port3PerPortPeTxiSet;
	UINT8  Usb2Port3PerPortTxiSet;
	UINT8  Usb2Port3IUsbTxEmphasisEn;
	UINT8  Usb2Port3PerPortTxPeHalf;
	UINT8  Usb2Port4PerPortPeTxiSet;
	UINT8  Usb2Port4PerPortTxiSet;
	UINT8  Usb2Port4IUsbTxEmphasisEn;
	UINT8  Usb2Port4PerPortTxPeHalf;
	UINT8  Usb3Lane0Ow2tapgen2deemph3p5;
	UINT8  Usb3Lane1Ow2tapgen2deemph3p5;
	UINT8  Usb3Lane2Ow2tapgen2deemph3p5;
	UINT8  Usb3Lane3Ow2tapgen2deemph3p5;
	UINT8  PcdSataInterfaceSpeed;
	UINT8  PcdPchUsbSsicPort;
	UINT8  PcdPchUsbHsicPort;
	UINT8  PcdPcieRootPortSpeed;
	UINT8  PcdPchSsicEnable;
	UINT32 PcdLogoPtr;
	UINT32 PcdLogoSize;
	UINT8  PcdRtcLock;
	UINT8  PMIC_I2CBus;
	UINT8  ISPEnable;
	UINT8  ISPPciDevConfig;
	UINT8  PcdSdDetectChk; /*Enable\Disable SD Card Detect Simulation*/
};

extern struct chip_operations soc_intel_braswell_ops;

#endif /* _SOC_CHIP_H_ */
