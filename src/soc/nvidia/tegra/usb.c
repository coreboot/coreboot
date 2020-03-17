/*
 * This file is part of the coreboot project.
 *
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

#include <delay.h>
#include <device/mmio.h>
#include <console/console.h>
#include <soc/clock.h>

#include "usb.h"

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

/*
 * Tegra EHCI controllers need their usb_mode, lpm_ctrl and tx_fill_tuning
 * registers initialized after every EHCI reset and before any other actions
 * (such as Run/Stop bit) are taken. We reset the controller here, set those
 * registers and rely on the fact that libpayload doesn't reset EHCI controllers
 * on initialization for whatever weird reason. This is ugly, fragile, and I
 * really don't like it, but making this work will require an ugly hack one way
 * or another so we might as well take the path of least resistance for now.
 */
static void usb_ehci_reset_and_prepare(struct usb_ctlr *usb, enum usb_phy_type type)
{
	int timeout = 1000;

	write32(&usb->ehci_usbcmd, 1 << 1);	/* Host Controller Reset */
	/* TODO: Resets are long, find way to parallelize... or just use XHCI */
	while (--timeout && (read32(&usb->ehci_usbcmd) & 1 << 1))
		/* wait for HC to reset */;

	if (!timeout) {
		printk(BIOS_ERR, "ERROR: EHCI(%p) reset timeout", usb);
		return;
	}

	/* Controller mode: HOST */
	write32(&usb->usb_mode, 3 << 0);
	/* Parallel transceiver selct */
	write32(&usb->lpm_ctrl, type << 29);
	/* Tx FIFO Burst thresh */
	write32(&usb->tx_fill_tuning, 0x10 << 16);
}

/* Assume USBx clocked, out of reset, UTMI+ PLL set up, SAMP_x out of pwrdn */
void usb_setup_utmip(void *usb_base)
{
	struct usb_ctlr *usb = (struct usb_ctlr *)usb_base;

	/* KHz formulas were guessed from U-Boot constants. Formats unclear. */
	int khz = clock_get_pll_input_khz();

	/* Stop UTMI+ crystal clock while we mess with its settings */
	clrbits32(&usb->utmip.misc1, 1 << 30);	/* PHY_XTAL_CLKEN */
	udelay(1);

	/* Take stuff out of pwrdn and add some magic numbers from U-Boot */
	write32(&usb->utmip.xcvr0,
		0x8 << 25 |		/* HS slew rate [10:4] */
		0x3 << 22 |		/* HS driver output 'SETUP' [6:4] */
		  0 << 21 |		/* LS bias selection */
		  0 << 18 |		/* PDZI pwrdn */
		  0 << 16 |		/* PD2 pwrdn */
		  0 << 14 |		/* PD pwrdn */
		  1 << 13 |		/* (rst) HS receiver terminations */
		0x1 << 10 |		/* (rst) LS falling slew rate */
		0x1 <<  8 |		/* (rst) LS rising slew rate */
		0x4 <<  0);		/* HS driver output 'SETUP' [3:0] */
	write32(&usb->utmip.xcvr1,
		0x7 << 18 |		/* Termination range adjustment */
		  0 <<  4 |		/* PDDR pwrdn */
		  0 <<  2 |		/* PDCHRP pwrdn */
		  0 <<  0);		/* PDDISC pwrdn */
	write32(&usb->utmip.tx,
		  1 << 19 |		/* FS send initial J before sync(?) */
		  1 << 16 |		/* (rst) Allow stuff error on SoP */
		  1 <<  9);		/* (rst) Check disc only on EoP */
	write32(&usb->utmip.hsrx0,
		0x2 << 30 |		/* (rst) Keep pattern on active */
		  1 << 28 |		/* (rst) Realign inertia on pkt */
		0x0 << 24 |		/* (rst) edges-1 to move sampling */
		0x3 << 21 |		/* (rst) squelch delay on EoP */
	       0x11 << 15 |		/* cycles until IDLE */
	       0x10 << 10);		/* elastic input depth */

	/* U-Boot claims the USBD values for these are used across all UTMI+
	 * PHYs. That sounds so horribly wrong that I'm not going to implement
	 * it, but keep it in mind if we're ever not using the USBD port. */
	write32(&usb->utmip.bias0,
		0x1 << 24 |		/* HS disconnect detect level [2] */
		  1 << 23 |		/* (rst) IDPD value */
		  1 << 22 |		/* (rst) IDPD select */
		  1 << 11 |		/* (rst) OTG pwrdn */
		  0 << 10 |		/* bias pwrdn */
		0x1 <<  2 |		/* HS disconnect detect level [1:0] */
		0x2 <<  0);		/* HS squelch detect level */

	write32(&usb->utmip.bias1,
		khz / 2200 << 3 |	/* bias pwrdn cycles (20us?) */
			 1 << 2 |	/* (rst) VBUS wakeup pwrdn */
			 0 << 0);	/* PDTRK pwrdn */

	write32(&usb->utmip.debounce,
		       0xffff << 16 |	/* (rst) */
		25 * khz / 10 <<  0);	/* TODO: what's this, really? */

	udelay(1);
	setbits32(&usb->utmip.misc1, 1 << 30); /* PHY_XTAL_CLKEN */

	write32(&usb->suspend_ctrl,
		  1 << 12 |		/* UTMI+ enable */
		  0 << 11);		/* UTMI+ reset */

	usb_ehci_reset_and_prepare(usb, USB_PHY_UTMIP);
	printk(BIOS_DEBUG, "USB controller @ %p set up with UTMI+ PHY\n",usb_base);
}
