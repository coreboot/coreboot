/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clock.h>
#include <soc/iomap.h>
#include <soc/usb.h>

#define CRPORT_TX_OVRD_DRV_LO	0x1002
#define CRPORT_RX_OVRD_IN_HI	0x1006
#define CRPORT_TX_ALT_BLOCK	0x102d

static u32 *const tcsr_usb_sel = (void *)0x1a4000b0;

struct usb_qc_phy {
	u32 ipcat;
	u32 ctrl;
	u32 general_cfg;
	u32 ram1;
	u32 hs_phy_ctrl;
	u32 param_ovrd;
	u32 chrg_det_ctrl;
	u32 chrg_det_output;
	u32 alt_irq_en;
	u32 hs_phy_irq_stat;
	u32 cgctl;
	u32 dbg_bus;
	u32 ss_phy_ctrl;
	u32 ss_phy_param1;
	u32 ss_phy_param2;
	u32 crport_data_in;
	u32 crport_data_out;
	u32 crport_cap_addr;
	u32 crport_cap_data;
	u32 crport_ack_read;
	u32 crport_ack_write;
};
check_member(usb_qc_phy, crport_ack_write, 0x50);

static struct usb_qc_phy * const usb_host1_phy = (void *)USB_HOST1_PHY_BASE;
static struct usb_qc_phy * const usb_host2_phy = (void *)USB_HOST2_PHY_BASE;

struct usb_dwc3 {
	u32 sbuscfg0;
	u32 sbuscfg1;
	u32 txthrcfg;
	u32 rxthrcfg;
	u32 ctl;
	u32 evten;
	u32 sts;
	u8 reserved0[4];
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

static struct usb_dwc3 * const usb_host1_dwc3 = (void *)USB_HOST1_DWC3_BASE;
static struct usb_dwc3 * const usb_host2_dwc3 = (void *)USB_HOST2_DWC3_BASE;

static void setup_dwc3(struct usb_dwc3 *dwc3)
{
	write32(&dwc3->usb3pipectl,
		0x1 << 31 |	/* assert PHY soft reset */
		0x1 << 25 |	/* (default) U1/U2 exit fail -> recovery? */
		0x1 << 24 |	/* (default) activate PHY low power states */
		0x1 << 19 |	/* (default) PHY low power delay value */
		0x1 << 18 |	/* (default) activate PHY low power delay */
		0x1 <<  1 |	/* (default) Tx deemphasis value */
		0x1 <<  0);	/* (default) elastic buffer mode */

	write32(&dwc3->usb2phycfg,
		0x1 << 31 |	/* assert PHY soft reset */
		0x9 << 10 |	/* (default) PHY clock turnaround 8-bit UTMI+ */
		0x1 <<  8 |	/* (default) enable PHY sleep in L1 */
		0x1 <<  6);	/* (default) enable PHY suspend */

	write32(&dwc3->ctl,
		0x2 << 19 |	/* (default) suspend clock scaling */
		0x1 << 16 |	/* retry SS three times before HS downgrade */
		0x1 << 12 |	/* port capability HOST */
		0x1 << 11 |	/* assert core soft reset */
		0x1 << 10 |	/* (default) sync ITP to refclk */
		0x1 <<  2);	/* U2 exit after 8us LFPS (instead of 248ns) */

	write32(&dwc3->uctl,
		0x32 << 22 |	/* (default) reference clock period in ns */
		 0x1 << 15 |	/* (default) XHCI compliant device addressing */
		0x10 << 0);	/* (default) devices time out after 32us */

	udelay(5);

	clrbits_le32(&dwc3->ctl, 0x1 << 11);	/* deassert core soft reset */
	clrbits_le32(&dwc3->usb2phycfg, 0x1 << 31);	/* PHY soft reset */
	clrbits_le32(&dwc3->usb3pipectl, 0x1 << 31);	/* PHY soft reset */
}

static void setup_phy(struct usb_qc_phy *phy)
{
	write32(&phy->ss_phy_ctrl,
		0x1 << 24 |	/* Indicate VBUS power present */
		0x1 <<  8 |	/* Enable USB3 ref clock to prescaler */
		0x1 <<  7 |	/* assert SS PHY reset */
		0x19 << 0);	/* (default) reference clock multiplier */

	write32(&phy->hs_phy_ctrl,
		0x1 << 26 |	/* (default) unclamp DPSE/DMSE VLS */
		0x1 << 25 |	/* (default) select freeclk for utmi_clk */
		0x1 << 24 |	/* (default) unclamp DMSE VLS */
		0x1 << 21 |	/* (default) enable UTMI clock */
		0x1 << 20 |	/* set OTG VBUS as valid */
		0x1 << 18 |	/* use ref clock from core */
		0x1 << 17 |	/* (default) unclamp DPSE VLS */
		0x1 << 11 |	/* force xo/bias/pll to stay on in suspend */
		0x1 <<  9 |	/* (default) unclamp IDHV */
		0x1 <<  8 |	/* (default) unclamp VLS (again???) */
		0x1 <<  7 |	/* (default) unclamp HV VLS */
		0x7 <<  4 |	/* select frequency (no idea which one) */
		0x1 <<  1);	/* (default) "retention enable" */

	write32(&phy->ss_phy_param1,
		0x6e << 20 |	/* full TX swing amplitude */
		0x20 << 14 |	/* (default) 6dB TX deemphasis */
		0x17 <<  8 |	/* 3.5dB TX deemphasis */
		 0x9 <<  3);	/* (default) LoS detector level */

	write32(&phy->general_cfg, 0x1 << 2);	/* set XHCI 1.00 compliance */

	udelay(5);
	clrbits_le32(&phy->ss_phy_ctrl, 0x1 << 7); /* deassert SS PHY reset */
}

static void crport_handshake(void *capture_reg, void *acknowledge_bit, u32 data)
{
	int usec = 100;

	if (capture_reg)
		write32(capture_reg, data);

	write32(acknowledge_bit, 0x1 << 0);
	while (read32(acknowledge_bit) && --usec)
		udelay(1);

	if (!usec)
		printk(BIOS_ERR, "CRPORT handshake timed out (0x%08x)\n", data);
}

static void crport_write(struct usb_qc_phy *phy, u16 addr, u16 data)
{
	crport_handshake(&phy->crport_data_in, &phy->crport_cap_addr, addr);
	crport_handshake(&phy->crport_data_in, &phy->crport_cap_data, data);
	crport_handshake(NULL, &phy->crport_ack_write, 0);
}

static void tune_phy(struct usb_qc_phy *phy)
{
	crport_write(phy, CRPORT_RX_OVRD_IN_HI,
		      0x1 << 11 |	/* Set RX_EQ override? */
		      0x4 <<  8 |	/* Set RX_EQ to 4? */
		      0x1 <<  7);	/* Enable RX_EQ override */
	crport_write(phy, CRPORT_TX_OVRD_DRV_LO,
		      0x1 << 14 |	/* Enable amplitude (override?) */
		     0x17 <<  7 |	/* Set TX deemphasis to 23 */
		     0x6e <<  0);	/* Set amplitude to 110 */
	crport_write(phy, CRPORT_TX_ALT_BLOCK,
		      0x1 <<  7);	/* ALT block? ("partial RX reset") */
}

void setup_usb_host1(void)
{
	printk(BIOS_INFO, "Setting up USB HOST1 controller...\n");
	setbits_le32(tcsr_usb_sel, 1 << 0);	/* Select DWC3 controller */
	setup_phy(usb_host1_phy);
	setup_dwc3(usb_host1_dwc3);
	tune_phy(usb_host1_phy);
}

void setup_usb_host2(void)
{
	printk(BIOS_INFO, "Setting up USB HOST2 controller...\n");
	setbits_le32(tcsr_usb_sel, 1 << 1);	/* Select DWC3 controller */
	setup_phy(usb_host2_phy);
	setup_dwc3(usb_host2_dwc3);
	tune_phy(usb_host2_phy);
}
