/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/usb/usb_common.h>
#include <soc/addressmap.h>
#include <soc/clock.h>

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
	u8 reserved3[124];
	u32 usb2phyacc;
	u8 reserved4[60];
	u32 usb3pipectl;
	u8 reserved5[60];
};
check_member(usb_dwc3, usb2phycfg, 0x100);
check_member(usb_dwc3, usb3pipectl, 0x1c0);

struct usb_dwc3_cfg {
	struct usb_dwc3 *usb_host_dwc3;
	u32 *usb3_bcr;
	u32 *qusb2phy_bcr;
	u32 *gcc_usb3phy_bcr_reg;
	u32 *gcc_qmpphy_bcr_reg;
};

static struct usb_dwc3_cfg usb_port0 = {
	.usb_host_dwc3 =	(void *)USB_HOST_DWC3_BASE,
	.usb3_bcr =		&gcc->usb30_prim_bcr,
	.qusb2phy_bcr =		&gcc->qusb2phy_prim_bcr,
	.gcc_usb3phy_bcr_reg =	&gcc->usb3_dp_phy_prim_bcr,
	.gcc_qmpphy_bcr_reg =	&gcc->usb3_phy_prim_bcr,
};

static void reset_usb(struct usb_dwc3_cfg *dwc3)
{
	/* Assert Core reset */
	clock_reset_bcr(dwc3->usb3_bcr, 1);

	/* Assert HS PHY reset */
	clock_reset_bcr(dwc3->qusb2phy_bcr, 1);

	/* Assert QMP PHY reset */
	clock_reset_bcr(dwc3->gcc_usb3phy_bcr_reg, 1);
	clock_reset_bcr(dwc3->gcc_qmpphy_bcr_reg, 1);
}

void reset_usb0(void)
{
	/* Before Resetting PHY, put Core in Reset */
	printk(BIOS_INFO, "Starting DWC3 and PHY resets for USB(0)\n");

	reset_usb(&usb_port0);
}

static void setup_dwc3(struct usb_dwc3 *dwc3)
{
	/* core exits U1/U2/U3 only in PHY power state P1/P2/P3 respectively */
	clrsetbits32(&dwc3->usb3pipectl,
		DWC3_GUSB3PIPECTL_DELAYP1TRANS,
		DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX);

	/*
	 * Configure USB phy interface of DWC3 core.
	 * 1. Select UTMI+ PHY with 16-bit interface.
	 * 2. Set USBTRDTIM to the corresponding value
	 * according to the UTMI+ PHY interface.
	 */
	clrsetbits32(&dwc3->usb2phycfg,
			(DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK |
			DWC3_GUSB2PHYCFG_PHYIF_MASK),
			(DWC3_GUSB2PHYCFG_PHYIF(UTMI_PHYIF_8_BIT) |
			DWC3_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_8_BIT)));

	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_SCALEDOWN_MASK |
			DWC3_GCTL_DISSCRAMBLE),
			DWC3_GCTL_U2EXIT_LFPS | DWC3_GCTL_DSBLCLKGTNG);

	/* configure controller in Host mode */
	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG)),
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));
	printk(BIOS_SPEW, "Configure USB in Host mode\n");
}

/* Initialization of DWC3 Core and PHY */

static void setup_usb_host(struct usb_dwc3_cfg *dwc3,
			void  *board_data)
{
	 /* Clear core reset. */
	clock_reset_bcr(dwc3->usb3_bcr, 0);

	/* Clear QUSB PHY reset. */
	clock_reset_bcr(dwc3->qusb2phy_bcr, 0);

	/* Initialize HS PHY */
	hs_usb_phy_init(board_data);

	/* Clear QMP PHY resets. */
	clock_reset_bcr(dwc3->gcc_usb3phy_bcr_reg, 0);
	clock_reset_bcr(dwc3->gcc_qmpphy_bcr_reg, 0);

	/* Initialize QMP PHY */
	ss_qmp_phy_init();

	setup_dwc3(dwc3->usb_host_dwc3);

	printk(BIOS_INFO, "DWC3 and PHY setup finished\n");
}
void setup_usb_host0(void  *board_data)
{
	printk(BIOS_INFO, "Setting up USB HOST0 controller.\n");
	setup_usb_host(&usb_port0, board_data);
}
