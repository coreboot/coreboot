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

#include <delay.h>
#include <arch/io.h>
#include <console/console.h>
#include <soc/clock.h>

#include "usb.h"

/* Assume USBx clocked, out of reset, UTMI+ PLL set up, SAMP_x out of pwrdn */
void usb_setup_utmip(struct usb_ctlr *usb)
{
	/* KHz formulas were guessed from U-Boot constants. Formats unclear. */
	int khz = clock_get_osc_khz();

	/* Stop UTMI+ crystal clock while we mess with its settings */
	clrbits_le32(&usb->utmip.misc1, 1 << 30);	/* PHY_XTAL_CLKEN */
	udelay(1);

	/* Take stuff out of pwrdn and add some magic numbers from U-Boot */
	write32(0x8 << 25 |		/* HS slew rate [10:4] */
		0x3 << 22 |		/* HS driver output 'SETUP' [6:4] */
		0 << 21 |		/* LS bias selection */
		0 << 18 |		/* PDZI pwrdn */
		0 << 16 |		/* PD2 pwrdn */
		0 << 14 |		/* PD pwrdn */
		1 << 13 |		/* (rst) HS receiver terminations */
		0x1 << 10 |		/* (rst) LS falling slew rate */
		0x1 << 8 |		/* (rst) LS rising slew rate */
		0x4 << 0 |		/* HS driver output 'SETUP' [3:0] */
		0, &usb->utmip.xcvr0);
	write32(0x7 << 18 |		/* Termination range adjustment */
		0 << 4 |		/* PDDR pwrdn */
		0 << 2 |		/* PDCHRP pwrdn */
		0 << 0 |		/* PDDISC pwrdn */
		0, &usb->utmip.xcvr1);
	write32(1 << 19 |		/* FS send initial J before sync(?) */
		1 << 16 |		/* (rst) Allow stuff error on SoP */
		1 << 9 |		/* (rst) Check disc only on EoP */
		0, &usb->utmip.tx);
	write32(0x2 << 30 |		/* (rst) Keep pattern on active */
		1 << 28 |		/* (rst) Realign inertia on pkt */
		0x1 << 24 |		/* (rst) edges-1 to move sampling */
		0x3 << 21 |		/* (rst) squelch delay on EoP */
		0x11 << 15 |		/* cycles until IDLE */
		0x10 << 10 |		/* elastic input depth */
		0, &usb->utmip.hsrx0);

	/* U-Boot claims the USBD values for these are used across all UTMI+
	 * PHYs. That sounds so horribly wrong that I'm not going to implement
	 * it, but keep it in mind if we're ever not using the USBD port. */
	write32(0x1 << 24 |		/* HS disconnect detect level [2] */
		1 << 23 |		/* (rst) IDPD value */
		1 << 22 |		/* (rst) IDPD select */
		1 << 11 |		/* (rst) OTG pwrdn */
		0 << 10 |		/* bias pwrdn */
		0x1 << 2 |		/* HS disconnect detect level [1:0] */
		0x2 << 0 |		/* HS squelch detect level */
		0, &usb->utmip.bias0);

	write32(khz / 2200 << 3 |	/* bias pwrdn cycles (20us?) */
		1 << 2 |		/* (rst) VBUS wakeup pwrdn */
		0 << 0 |		/* PDTRK pwrdn */
		0, &usb->utmip.bias1);

	write32(0xffff << 16 |		/* (rst) */
		25 * khz / 10 << 0 |	/* TODO: what's this, really? */
		0, &usb->utmip.debounce);

	udelay(1);
	setbits_le32(&usb->utmip.misc1, 1 << 30); /* PHY_XTAL_CLKEN */

	write32(1 << 12 |		/* UTMI+ enable */
		0 << 11 |		/* UTMI+ reset */
		0, &usb->suspend_ctrl);
}

/*
 * Tegra EHCI controllers need their usb_mode, lpm_ctrl and tx_fill_tuning
 * registers initialized after every EHCI reset and before any other actions
 * (such as Run/Stop bit) are taken. We reset the controller here, set those
 * registers and rely on the fact that libpayload doesn't reset EHCI controllers
 * on initialization for whatever weird reason. This is ugly, fragile, and I
 * really don't like it, but making this work will require an ugly hack one way
 * or another so we might as well take the path of least resistance for now.
 */
void usb_ehci_reset_and_prepare(struct usb_ctlr *usb, enum usb_phy_type type)
{
	int timeout = 1000;

	write32(1 << 1, &usb->ehci_usbcmd);	/* Host Controller Reset */
	/* TODO: Resets are long, find way to parallelize... or just use XHCI */
	while (--timeout && (read32(&usb->ehci_usbcmd) & 1 << 1))
		/* wait for HC to reset */;

	if (!timeout) {
		printk(BIOS_ERR, "ERROR: EHCI(%p) reset timeout", usb);
		return;
	}

	write32(3 << 0, &usb->usb_mode);	/* Controller mode: HOST */
	write32(type << 29, &usb->lpm_ctrl);	/* Parallel transceiver selct */
	write32(0x10 << 16, &usb->tx_fill_tuning);  /* Tx FIFO Burst thresh */
}
