/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Rockchip, Inc.
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

#ifndef __SOC_ROCKCHIP_RK3399_USB_H_
#define __SOC_ROCKCHIP_RK3399_USB_H_

#include <soc/addressmap.h>

/* Global constants */
#define DWC3_GSNPSID_MASK			0xffff0000
#define DWC3_GSNPSID_SHIFT			16
#define DWC3_GSNPSREV_MASK			0xffff

/* Global Configuration Register */
#define DWC3_GCTL_PWRDNSCALE(n)			((n) << 19)
#define DWC3_GCTL_U2RSTECN			(1 << 16)
#define DWC3_GCTL_RAMCLKSEL(x)			\
		(((x) & DWC3_GCTL_CLK_MASK) << 6)
#define DWC3_GCTL_CLK_BUS			(0)
#define DWC3_GCTL_CLK_PIPE			(1)
#define DWC3_GCTL_CLK_PIPEHALF			(2)
#define DWC3_GCTL_CLK_MASK			(3)
#define DWC3_GCTL_PRTCAP_MASK			(3 << 12)
#define DWC3_GCTL_PRTCAP_HOST			(1 << 12)
#define DWC3_GCTL_PRTCAP_DEVICE			(2 << 12)
#define DWC3_GCTL_PRTCAP_OTG			(3 << 12)
#define DWC3_GCTL_CORESOFTRESET			(1 << 11)
#define DWC3_GCTL_SCALEDOWN(n)			((n) << 4)
#define DWC3_GCTL_SCALEDOWN_MASK		DWC3_GCTL_SCALEDOWN(3)
#define DWC3_GCTL_DISSCRAMBLE			(1 << 3)
#define DWC3_GCTL_DSBLCLKGTNG			(1 << 0)

/* Global HWPARAMS1 Register */
#define DWC3_GHWPARAMS1_EN_PWROPT(n)		(((n) & (3 << 24)) >> 24)
#define DWC3_GHWPARAMS1_EN_PWROPT_NO		0
#define DWC3_GHWPARAMS1_EN_PWROPT_CLK		1

/* Global USB2 PHY Configuration Register */
#define DWC3_GUSB2PHYCFG_PHYSOFTRST		(1 << 31)
#define DWC3_GUSB2PHYCFG_U2_FREECLK_EXISTS	(1 << 30)
#define DWC3_GUSB2PHYCFG_USBTRDTIM(n)		((n) << 10)
#define DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK	DWC3_GUSB2PHYCFG_USBTRDTIM(0xf)
#define DWC3_GUSB2PHYCFG_SUSPHY			(1 << 6)
#define DWC3_GUSB2PHYCFG_PHYIF(n)		((n) << 3)
#define DWC3_GUSB2PHYCFG_PHYIF_MASK		DWC3_GUSB2PHYCFG_PHYIF(1)
#define USBTRDTIM_UTMI_8_BIT			9
#define USBTRDTIM_UTMI_16_BIT			5

/* Global USB3 PIPE Control Register */
#define DWC3_GUSB3PIPECTL_PHYSOFTRST		(1 << 31)
#define DWC3_GUSB3PIPECTL_SUSPHY		(1 << 17)

struct rockchip_usb_dwc3 {
	uint32_t sbuscfg0;
	uint32_t sbuscfg1;
	uint32_t txthrcfg;
	uint32_t rxthrcfg;
	uint32_t ctl;
	uint32_t evten;
	uint32_t sts;
	uint8_t reserved0[4];
	uint32_t snpsid;
	uint32_t gpio;
	uint32_t uid;
	uint32_t uctl;
	uint64_t buserraddr;
	uint64_t prtbimap;
	uint32_t hwparams0;
	uint32_t hwparams1;
	uint32_t hwparams2;
	uint32_t hwparams3;
	uint32_t hwparams4;
	uint32_t hwparams5;
	uint32_t hwparams6;
	uint32_t hwparams7;
	uint32_t dbgfifospace;
	uint32_t dbgltssm;
	uint32_t dbglnmcc;
	uint32_t dbgbmu;
	uint32_t dbglspmux;
	uint32_t dbglsp;
	uint32_t dbgepinfo0;
	uint32_t dbgepinfo1;
	uint64_t prtbimap_hs;
	uint64_t prtbimap_fs;
	uint8_t reserved2[112];
	uint32_t usb2phycfg;
	uint8_t reserved3[60];
	uint32_t usb2i2cctl;
	uint8_t reserved4[60];
	uint32_t usb2phyacc;
	uint8_t reserved5[60];
	uint32_t usb3pipectl;
	uint8_t reserved6[60];
};

static struct rockchip_usb_dwc3 * const rockchip_usb_otg0_dwc3 =
		(void *)USB_OTG0_DWC3_BASE;
static struct rockchip_usb_dwc3 * const rockchip_usb_otg1_dwc3 =
		(void *)USB_OTG1_DWC3_BASE;

#define TCPHY_ISOLATION_CTRL_EN		(1 << 15)
#define TCPHY_ISOLATION_CTRL_CMN_EN	(1 << 14)
#define TCPHY_ISOLATION_CTRL_MODE_SEL	(1 << 12)
#define TCPHY_ISOLATION_CTRL_LN_EN(ln)	(1 << (ln))
#define TCPHY_CMN_HSCLK_PLL_CONFIG	0x30
#define TCPHY_CMN_HSCLK_PLL_MASK	0x33

struct rk3399_tcphy {
	uint8_t _res0[0x780 - 0x0];
	uint32_t cmn_diag_hsclk_sel;
	uint8_t _res1[0x10000 - 0x784];
	struct {
		uint8_t _res2[0x3c8 - 0x0];
		uint32_t xcvr_diag_lane_fcm_en_mgn;
		uint8_t _res3[0x408 - 0x3cc];
		uint32_t tx_psc_a2;
		uint8_t _res4[0x488 - 0x40c];
		uint32_t tx_rcvdet_en_tmr;
		uint32_t tx_rcvdet_st_tmr;
		uint8_t _res5[0x784 - 0x490];
		uint32_t tx_diag_tx_drv;
		uint8_t _res6[0x800 - 0x788];
	} lane[4];
	uint8_t _res7[0x32000 - 0x12000];
	uint32_t pma_cmn_ctrl1;
	uint8_t _res8[0x3207c - 0x32004];
	uint32_t isolation_ctrl;
};
check_member(rk3399_tcphy, lane[2].tx_diag_tx_drv, 0x11784);
check_member(rk3399_tcphy, isolation_ctrl, 0x3207c);

static struct rk3399_tcphy * const rockchip_usb_otg0_phy =
		(void *)USB_OTG0_TCPHY_BASE;
static struct rk3399_tcphy * const rockchip_usb_otg1_phy =
		(void *)USB_OTG1_TCPHY_BASE;

/* Call reset_ before setup_ */
void reset_usb_otg0(void);
void reset_usb_otg1(void);
void setup_usb_otg0(void);
void setup_usb_otg1(void);

#endif /* __SOC_ROCKCHIP_RK3399_USB_H_ */
