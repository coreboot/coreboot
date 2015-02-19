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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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

static u32 * const tcsr_usb_sel = (void *)0x1a4000b0;

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
	writel(0x1 << 31 | 0x1 << 25 | 0x1 << 24 | 0x1 << 19 | 0x1 << 18 | 0x1 << 1 | 0x1 << 0 | 0,
	       &dwc3->usb3pipectl);

	writel(0x1 << 31 | 0x9 << 10 | 0x1 << 8 | 0x1 << 6 | 0,
	       &dwc3->usb2phycfg);

	writel(0x2 << 19 | 0x1 << 16 | 0x1 << 12 | 0x1 << 11 | 0x1 << 10 | 0x1 << 2 | 0,
	       &dwc3->ctl);

	writel(0x32 << 22 | 0x1 << 15 | 0x10 << 0 | 0, &dwc3->uctl);

	udelay(5);

	clrbits_le32(&dwc3->ctl, 0x1 << 11);	/* deassert core soft reset */
	clrbits_le32(&dwc3->usb2phycfg, 0x1 << 31);	/* PHY soft reset */
	clrbits_le32(&dwc3->usb3pipectl, 0x1 << 31);	/* PHY soft reset */
}

static void setup_phy(struct usb_qc_phy *phy)
{
	writel(0x1 << 24 | 0x1 << 8 | 0x1 << 7 | 0x19 << 0 | 0,
	       &phy->ss_phy_ctrl);

	writel(0x1 << 26 | 0x1 << 25 | 0x1 << 24 | 0x1 << 21 | 0x1 << 20 | 0x1 << 18 | 0x1 << 17 | 0x1 << 11 | 0x1 << 9 | 0x1 << 8 | 0x1 << 7 | 0x7 << 4 | 0x1 << 1 | 0,
	       &phy->hs_phy_ctrl);

	writel(0x6e << 20 | 0x20 << 14 | 0x17 << 8 | 0x9 << 3 | 0,
	       &phy->ss_phy_param1);

	writel(0x1 << 2, &phy->general_cfg);	/* set XHCI 1.00 compliance */

	udelay(5);
	clrbits_le32(&phy->ss_phy_ctrl, 0x1 << 7); /* deassert SS PHY reset */
}

static void crport_handshake(void *capture_reg, void *acknowledge_bit, u32 data)
{
	int usec = 100;

	if (capture_reg)
		writel(data, capture_reg);

	writel(0x1 << 0, acknowledge_bit);
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
