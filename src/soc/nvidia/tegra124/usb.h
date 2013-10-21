/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013, Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SOC_NVIDIA_TEGRA124_USB_H_
#define __SOC_NVIDIA_TEGRA124_USB_H_

#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/nvidia/tegra/usb.h>

static inline void usb_setup_utmip1(void)
{
	struct usb_ctlr *usb = (void *)TEGRA_USBD_BASE;
	usb_setup_utmip(usb);
	usb_ehci_reset_and_prepare(usb, USB_PHY_UTMIP);
	printk(BIOS_DEBUG, "USBD controller set up with UTMI+ PHY\n");
}

static inline void usb_setup_utmip2(void)
{
	struct usb_ctlr *usb = (void *)TEGRA_USB2_BASE;
	usb_setup_utmip(usb);
	usb_ehci_reset_and_prepare(usb, USB_PHY_UTMIP);
	printk(BIOS_DEBUG, "USB2 controller set up with UTMI+ PHY\n");
}

static inline void usb_setup_utmip3(void)
{
	struct usb_ctlr *usb = (void *)TEGRA_USB3_BASE;
	usb_setup_utmip(usb);
	usb_ehci_reset_and_prepare(usb, USB_PHY_UTMIP);
	printk(BIOS_DEBUG, "USB3 controller set up with UTMI+ PHY\n");
}

#endif	/* __SOC_NVIDIA_TEGRA124_USB_H_ */
