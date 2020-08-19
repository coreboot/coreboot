/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/usb.h>
#include <soc/clock.h>
#include <types.h>

/* USB BASE ADDRESS */
#define USB_HOST0_DWC3_BASE		0x758C100
#define USB3_USB30_QSCRATCH_BASE	0x7678800
#define USB2_FEMTO_PHY_PRI_BASE		0x007A000
#define USB_HOST1_DWC3_BASE		0x78CC100
#define USB2_USB30_QSCRATCH_BASE	0x79B8800
#define USB2_FEMTO_PHY_SEC_BASE		0x007C000

struct usb_qscratch {
	u8 rsvd0[8];
	u32 *qscratch_cfg_reg;

};
check_member(usb_qscratch, qscratch_cfg_reg, 0x08);

struct usb_usb2_phy_dig {
	u8 rsvd1[116];
	u32 utmi_ctrl5;
	u32 ctrl_common0;
	u32 ctrl_common1;
	u8 rsvd2[12];
	u32 phy_ctrl1;
	u32 phy_ctrl2;
	u8 rsvd3;
	u32 override_x0;
	u32 override_x1;
	u32 override_x2;
	u32 override_x3;
	u8 rsvd4[24];
	u32 tcsr_ctrl;
	u8 rsvd5[36];
	u32 refclk_ctrl;
};
check_member(usb_usb2_phy_dig, utmi_ctrl5, 0x74);
check_member(usb_usb2_phy_dig, phy_ctrl1, 0x8C);
check_member(usb_usb2_phy_dig, override_x0, 0x98);
check_member(usb_usb2_phy_dig, tcsr_ctrl, 0xC0);
check_member(usb_usb2_phy_dig, refclk_ctrl, 0xE8);

struct usb_dwc3 {
	u32 sbuscfg0;
	u32 sbuscfg1;
	u32 txthrcfg;
	u32 rxthrcfg;
	u32 ctl;
	u32 pmsts;
	u32 sts;
	u32 uctl1;
	u32 snpsid;
	u32 gpio;
	u32 uid;
	u32 uctl;
	u64 buserraddr;
	u64 prtbimap;
	u8 reserved1[32];
	u32 dbgfifospace;
	u32 dbgltssm;
	u32 dbglnmcc;
	u32 dbgbmu;
	u32 dbglspmux;
	u32 dbglsp;
	u32 dbgepinfo0;
	u32 dbgepinfo1;
	u64 prtbimap_hs;
	u64 prtbimap_fs;
	u8 reserved2[112];
	u32 usb2phycfg;
	u8 reserved3[60];
	u32 usb2i2cctl;
	u8 reserved4[60];
	u32 usb2phyacc;
	u8 reserved5[60];
	u32 usb3pipectl;
	u8 reserved6[60];
};
check_member(usb_dwc3, usb3pipectl, 0x1c0);

struct usb_dwc3_cfg {
	struct usb_dwc3 *usb_host_dwc3;
	struct usb_usb2_phy_dig *usb2_phy_dig;
	struct usb_qscratch *usb_qscratch_reg;
	u32 *usb2_phy_bcr;
	u32 *usb2_phy_por_bcr;
	u32 *usb3_bcr;
	struct usb_board_data *board_data;
};

static struct usb_dwc3_cfg usb_host_base[2] = {
	[HSUSB_SS_PORT_0] = {
	.usb_host_dwc3 =	(void *)USB_HOST0_DWC3_BASE,
	.usb2_phy_dig  =	(void *)USB2_FEMTO_PHY_PRI_BASE,
	.usb2_phy_bcr =		(void *)GCC_USB_HS_PHY_CFG_AHB_BCR,
	.usb2_phy_por_bcr =	(void *)GCC_USB2A_PHY_BCR,
	.usb3_bcr =		(void *)GCC_USB_30_BCR,
	.usb_qscratch_reg =	(void *)USB3_USB30_QSCRATCH_BASE,
	},
	[HSUSB_HS_PORT_1] = {
	.usb_host_dwc3 =	(void *)USB_HOST1_DWC3_BASE,
	.usb2_phy_dig  =	(void *)USB2_FEMTO_PHY_SEC_BASE,
	.usb2_phy_bcr =		(void *)GCC_QUSB2_PHY_BCR,
	.usb2_phy_por_bcr =	(void *)GCC_USB2_HS_PHY_ONLY_BCR,
	.usb3_bcr =		(void *)GCC_USB_HS_BCR,
	.usb_qscratch_reg =	(void *)USB2_USB30_QSCRATCH_BASE,
	},
};

void reset_usb(enum usb_port port)
{
	struct usb_dwc3_cfg *dwc3 = &usb_host_base[port];

	/* Put Core in Reset */
	printk(BIOS_INFO, "Starting DWC3 reset for USB%d\n", port);

	/* Assert Core reset */
	clock_reset_bcr(dwc3->usb3_bcr, 1);
}

static void usb2_phy_override_phy_params(struct usb_dwc3_cfg *dwc3)
{
	/* Override disconnect & squelch threshold values */
	write8(&dwc3->usb2_phy_dig->override_x0,
			dwc3->board_data->parameter_override_x0);

	/* Override HS transmitter Pre-emphasis values */
	write8(&dwc3->usb2_phy_dig->override_x1,
			dwc3->board_data->parameter_override_x1);

	/* Override HS transmitter Rise/Fall time values */
	write8(&dwc3->usb2_phy_dig->override_x2,
			dwc3->board_data->parameter_override_x2);

	/* Override FS/LS Source impedance values */
	write8(&dwc3->usb2_phy_dig->override_x3,
			dwc3->board_data->parameter_override_x3);
}

static void hs_usb_phy_init(struct usb_dwc3_cfg *dwc3)
{
	write8(&dwc3->usb2_phy_dig->tcsr_ctrl, USB2PHY_TCSR_CTRL);
	write8(&dwc3->usb2_phy_dig->refclk_ctrl, USB2PHY_REFCLK_CTRL);
	write8(&dwc3->usb2_phy_dig->utmi_ctrl5, USB2PHY_UTMI_CTRL5);
	write8(&dwc3->usb2_phy_dig->override_x0, USB2PHY_PARAMETER_OVERRIDE_X0);
	write8(&dwc3->usb2_phy_dig->override_x1, USB2PHY_PARAMETER_OVERRIDE_X1);
	write8(&dwc3->usb2_phy_dig->override_x2, USB2PHY_PARAMETER_OVERRIDE_X2);
	write8(&dwc3->usb2_phy_dig->override_x3, USB2PHY_PARAMETER_OVERRIDE_X3);

	if (dwc3->board_data)
		/* Override board specific PHY tuning values */
		usb2_phy_override_phy_params(dwc3);

	write8(&dwc3->usb2_phy_dig->phy_ctrl1, USB2PHY_HS_PHY_CTRL1);
	write8(&dwc3->usb2_phy_dig->ctrl_common0, QUSB2PHY_HS_PHY_CTRL_COMMON0);
	write8(&dwc3->usb2_phy_dig->ctrl_common1, QUSB2PHY_HS_PHY_CTRL_COMMON1);
	write8(&dwc3->usb2_phy_dig->phy_ctrl2, USB2PHY_HS_PHY_CTRL2);
	udelay(20);
	write8(&dwc3->usb2_phy_dig->utmi_ctrl5, USB2PHY_UTMI_CTRL5_POR_CLEAR);
	write8(&dwc3->usb2_phy_dig->phy_ctrl2,
					USB2PHY_HS_PHY_CTRL2_SUSPEND_N_SEL);
}

static void setup_dwc3(struct usb_dwc3 *dwc3)
{
	/* core exits U1/U2/U3 only in PHY power state P1/P2/P3 respectively */
	clrsetbits32(&dwc3->usb3pipectl,
		DWC3_GUSB3PIPECTL_DELAYP1TRANS,
		DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX);

	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_SCALEDOWN_MASK |
			DWC3_GCTL_DISSCRAMBLE),
			DWC3_GCTL_U2EXIT_LFPS | DWC3_GCTL_DSBLCLKGTNG);

	/* configure controller in Host mode */
	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG)),
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));
	printk(BIOS_INFO, "Configure USB in Host mode\n");
}

/* Initialization of DWC3 Core and PHY */
void setup_usb_host(enum usb_port port, struct usb_board_data *board_data)
{
	struct usb_dwc3_cfg *dwc3 = &usb_host_base[port];
	u32 val;

	printk(BIOS_INFO, "Setting up USB HOST%d controller.\n", port);

	dwc3->board_data = board_data;

	 /* Clear core reset. */
	clock_reset_bcr(dwc3->usb3_bcr, 0);

	if (port == HSUSB_SS_PORT_0) {
		/* Set PHY reset. */
		setbits32(&dwc3->usb2_phy_bcr, BIT(1));
		udelay(15);
		/* Clear PHY reset. */
		clrbits32(&dwc3->usb2_phy_bcr, BIT(1));
	} else {
		clock_reset_bcr(dwc3->usb2_phy_bcr, 1);
		udelay(15);
		clock_reset_bcr(dwc3->usb2_phy_bcr, 0);
	}
	udelay(100);

	/* Initialize PHYs */
	hs_usb_phy_init(dwc3);

	if (port == HSUSB_SS_PORT_0) {
		/* Set PHY POR reset. */
		setbits32(&dwc3->usb2_phy_por_bcr, BIT(0));
		val = read8(&dwc3->usb2_phy_dig->ctrl_common0);
		val &= ~(0x4);
		write8(&dwc3->usb2_phy_dig->ctrl_common0, val);
		udelay(20);
		/* Clear PHY POR reset. */
		clrbits32(&dwc3->usb2_phy_por_bcr, BIT(0));
	} else {
		clock_reset_bcr(dwc3->usb2_phy_por_bcr, 1);
		val = read8(&dwc3->usb2_phy_dig->ctrl_common0);
		val &= ~(0x4);
		write8(&dwc3->usb2_phy_dig->ctrl_common0, val);
		udelay(20);
		clock_reset_bcr(dwc3->usb2_phy_por_bcr, 0);
	}
	udelay(100);

	setup_dwc3(dwc3->usb_host_dwc3);

	/*
	 * Below sequence is used when dwc3 operates without
	 * SSPHY and only HS/FS/LS modes are supported.
	 */

	 /* Configure dwc3 to use UTMI clock as PIPE clock not present */
	setbits32(&dwc3->usb_qscratch_reg->qscratch_cfg_reg,
			PIPE_UTMI_CLK_DIS);
	udelay(2);
	setbits32(&dwc3->usb_qscratch_reg->qscratch_cfg_reg,
			PIPE_UTMI_CLK_SEL | PIPE3_PHYSTATUS_SW);
	udelay(3);
	clrbits32(&dwc3->usb_qscratch_reg->qscratch_cfg_reg,
			PIPE_UTMI_CLK_DIS);

	printk(BIOS_INFO, "DWC3 and PHY setup finished\n");
}
