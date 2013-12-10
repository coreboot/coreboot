/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA_USB_H__
#define __SOC_NVIDIA_TEGRA_USB_H__

#include <stdint.h>

struct utmip_ctlr {
	u32 pll0;
	u32 pll1;
	u32 xcvr0;
	u32 bias0;
	u32 hsrx0;
	u32 hsrx1;
	u32 fslsrx0;
	u32 fslsrx1;
	u32 tx;
	u32 misc0;
	u32 misc1;
	u32 debounce;
	u32 batchrgr;
	u32 spare;
	u32 xcvr1;
	u32 bias1;
	u32 bias_sts;
	u32 chrgr_debounce;
	u32 misc_sts;
	u32 pmc_wakeup;
};
check_member(utmip_ctlr, pmc_wakeup, 0x84c - 0x800);

struct usb_ctlr {
	u32 id;
	u32 _rsv0;
	u32 host;
	u32 device;
	u32 txbuf;			/* 0x010 */
	u32 rxbuf;
	u32 _rsv1[58];
	u16 ehci_caplen;		/* 0x100 */
	u16 ehci_version;
	u32 ehci_hcsp;
	u32 ehci_hccp;
	u32 _rsv2[5];
	u32 dci_version;		/* 0x120 */
	u32 dcc_params;
	u32 extsts;
	u32 extintr;
	u32 ehci_usbcmd;		/* 0x130 */
	u32 ehci_usbsts;
	u32 ehci_usbintr;
	u32 ehci_frindex;
	u32 _rsv3;			/* 0x140 */
	u32 ehci_periodic_base;
	u32 ehci_async_base;
	u32 async_ttsts;
	u32 burst_size;			/* 0x150 */
	u32 tx_fill_tuning;
	u32 _rsv4;
	u32 icusb_ctrl;
	u32 ulpi_viewport;		/* 0x160 */
	u32 _rsv5[4];
	u32 ehci_portsc;
	u32 _rsv6[15];
	u32 lpm_ctrl;
	u32 _rsv7[15];
	u32 otgsc;
	u32 usb_mode;
	u32 _rsv8;
	u32 ep_nak;			/* 0x200 */
	u32 ep_nak_enable;
	u32 ep_setup;
	u32 ep_init;
	u32 ep_deinit;
	u32 ep_sts;
	u32 ep_complete;
	u32 ep_ctrl[16];
	u32 _rsv9[105];
	u32 suspend_ctrl;		/* 0x400 */
	u32 vbus_sensors;
	u32 vbus_wakeup_id;
	u32 alt_vbus_sts;
	u32 legacy_ctrl;
	u32 _rsv10[3];
	u32 interpacket_delay;
	u32 _rsv11[27];
	u32 resume_delay;
	u32 _rsv12;
	u32 spare;
	u32 _rsv13[9];
	u32 new_ctrl;
	u32 _rsv14[207];
	struct utmip_ctlr utmip;	/* 0x800 */
};
check_member(usb_ctlr, utmip, 0x800);

enum usb_phy_type {		/* For use in lpm_ctrl[31:29] */
	USB_PHY_UTMIP = 0,
	USB_PHY_ULPI = 2,
	USB_PHY_ICUSB_SER = 3,
	USB_PHY_HSIC = 4,
};

void usb_setup_utmip(struct usb_ctlr *usb);
void usb_ehci_reset_and_prepare(struct usb_ctlr *usb, enum usb_phy_type type);
#endif
