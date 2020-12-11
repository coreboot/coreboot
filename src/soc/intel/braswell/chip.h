/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree.
 */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <stdint.h>
#include <drivers/intel/gma/i915.h>
#include <fsp/util.h>
#include <intelblocks/lpc_lib.h>
#include <soc/pci_devs.h>

#define SVID_CONFIG1		1
#define SVID_CONFIG3		3
#define SVID_PMIC_CONFIG	8

#define IGD_MEMSIZE_32MB	0x01
#define IGD_MEMSIZE_64MB	0x02
#define IGD_MEMSIZE_96MB	0x03
#define IGD_MEMSIZE_128MB	0x04

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

	enum serirq_mode serirq_mode;

	/* Disable SLP_X stretching after SUS power well loss */
	int disable_slp_x_stretch_sus_fail;

	/* LPE Audio Clock configuration */
	enum lpe_clk_src lpe_codec_clk_src; /* Both are 19.2MHz */

	/* Native SD Card controller - override controller capabilities */
	uint32_t sdcard_cap_low;
	uint32_t sdcard_cap_high;

	/* Enable devices in ACPI mode */
	int lpss_acpi_mode;
	int emmc_acpi_mode;
	int sd_acpi_mode;
	int lpe_acpi_mode;

	/* Allow PCIe devices to wake system from suspend */
	int pcie_wake_enable;

	/* Program USB2_COMPBG register.
	 * [10:7] - select vref to AFE port
	 *  x111 - 575mV, x110 - 650mV, x101 - 550mV, x100 - 537.5mV,
	 *  x011 - 625mV, x010 - 700mV, x001 - 600mV, x000 - 675mV
	 */
	enum usb_comp_bg_value usb_comp_bg;

	/*
	 * The following fields come from fsp_vpd.h .aka. VpdHeader.h.
	 * These are configuration values that are passed to FSP during MemoryInit.
	 */
	uint8_t  PcdMrcInitSpdAddr1;
	uint8_t  PcdMrcInitSpdAddr2;
	uint8_t  PcdIgdDvmt50PreAlloc;
	uint8_t  PcdDvfsEnable;
	uint8_t  PcdCaMirrorEn; /* Command Address Mirroring Enabled */

	/*
	 * The following fields come from fsp_vpd.h .aka. VpdHeader.h.
	 * These are configuration values that are passed to FSP during SiliconInit.
	 */
	uint8_t  PcdSdcardMode;
	uint8_t  PcdEnableHsuart0;
	uint8_t  PcdEnableHsuart1;
	uint8_t  PcdEnableAzalia;
	uint8_t  PcdEnableSata;
	uint8_t  PcdEnableXhci;
	uint8_t  PcdEnableLpe;
	uint8_t  PcdEnableDma0;
	uint8_t  PcdEnableDma1;
	uint8_t  PcdEnableI2C0;
	uint8_t  PcdEnableI2C1;
	uint8_t  PcdEnableI2C2;
	uint8_t  PcdEnableI2C3;
	uint8_t  PcdEnableI2C4;
	uint8_t  PcdEnableI2C5;
	uint8_t  PcdEnableI2C6;
	uint8_t  PunitPwrConfigDisable;
	uint8_t  ChvSvidConfig;
	uint8_t  DptfDisable;
	uint8_t  PcdEmmcMode;
	uint8_t  Usb2Port0PerPortPeTxiSet;
	uint8_t  Usb2Port0PerPortTxiSet;
	uint8_t  Usb2Port0IUsbTxEmphasisEn;
	uint8_t  Usb2Port0PerPortTxPeHalf;
	uint8_t  Usb2Port1PerPortPeTxiSet;
	uint8_t  Usb2Port1PerPortTxiSet;
	uint8_t  Usb2Port1IUsbTxEmphasisEn;
	uint8_t  Usb2Port1PerPortTxPeHalf;
	uint8_t  Usb2Port2PerPortPeTxiSet;
	uint8_t  Usb2Port2PerPortTxiSet;
	uint8_t  Usb2Port2IUsbTxEmphasisEn;
	uint8_t  Usb2Port2PerPortTxPeHalf;
	uint8_t  Usb2Port3PerPortPeTxiSet;
	uint8_t  Usb2Port3PerPortTxiSet;
	uint8_t  Usb2Port3IUsbTxEmphasisEn;
	uint8_t  Usb2Port3PerPortTxPeHalf;
	uint8_t  Usb2Port4PerPortPeTxiSet;
	uint8_t  Usb2Port4PerPortTxiSet;
	uint8_t  Usb2Port4IUsbTxEmphasisEn;
	uint8_t  Usb2Port4PerPortTxPeHalf;
	uint8_t  Usb3Lane0Ow2tapgen2deemph3p5;
	uint8_t  Usb3Lane1Ow2tapgen2deemph3p5;
	uint8_t  Usb3Lane2Ow2tapgen2deemph3p5;
	uint8_t  Usb3Lane3Ow2tapgen2deemph3p5;
	uint8_t  PcdPchUsbSsicPort;
	uint8_t  PcdPchUsbHsicPort;
	uint8_t  PcdPchSsicEnable;
	uint32_t PcdLogoPtr;
	uint32_t PcdLogoSize;
	uint8_t  PMIC_I2CBus;
	uint8_t  ISPEnable;
	uint8_t  ISPPciDevConfig;
	uint8_t  PcdSdDetectChk; /* Enable / Disable SD Card Detect Simulation */
	uint8_t  I2C0Frequency;  /* 0 - 100KHz, 1 - 400KHz, 2 - 1MHz */
	uint8_t  I2C1Frequency;
	uint8_t  I2C2Frequency;
	uint8_t  I2C3Frequency;
	uint8_t  I2C4Frequency;
	uint8_t  I2C5Frequency;
	uint8_t  I2C6Frequency;

	struct i915_gpu_controller_info gfx;
};

#endif /* _SOC_CHIP_H_ */
