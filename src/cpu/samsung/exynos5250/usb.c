/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include "gpio.h"
#include "power.h"
#include "cpu.h"
#include "usb.h"
#include "chip.h"

/* Enable VBUS */
static int usb_vbus_init(int vbus_gpio)
{
	/* Enable VBUS power switch */
	gpio_direction_output(GPIO_X11, 1);
	/* VBUS turn ON time */
	mdelay(3);

	return 0;
}

/* Setup the EHCI host controller. */

static void setup_usb_phy(struct usb_phy *usb, int hsic_gpio)
{
	unsigned int hostphy_ctrl0;

	power_enable_usb_phy();

	/* Setting up host and device simultaneously */
	hostphy_ctrl0 = readl(&usb->usbphyctrl0);
	hostphy_ctrl0 &= ~(HOST_CTRL0_FSEL_MASK | HOST_CTRL0_COMMONON_N |
			   /* HOST Phy setting */
			   HOST_CTRL0_PHYSWRST |
			   HOST_CTRL0_PHYSWRSTALL |
			   HOST_CTRL0_SIDDQ |
			   HOST_CTRL0_FORCESUSPEND |
			   HOST_CTRL0_FORCESLEEP);
	hostphy_ctrl0 |= (	/* Setting up the ref freq */
				 CLK_24MHZ << 16 |
				 /* HOST Phy setting */
				 HOST_CTRL0_LINKSWRST |
				 HOST_CTRL0_UTMISWRST);
	writel(hostphy_ctrl0, &usb->usbphyctrl0);
	udelay(10);
	clrbits_le32(&usb->usbphyctrl0,
		     HOST_CTRL0_LINKSWRST | HOST_CTRL0_UTMISWRST);
	udelay(20);

	/* EHCI Ctrl setting */
	setbits_le32(&usb->ehcictrl,
		     EHCICTRL_ENAINCRXALIGN |
		     EHCICTRL_ENAINCR4 |
		     EHCICTRL_ENAINCR8 | EHCICTRL_ENAINCR16);

	/* HSIC USB Hub initialization. */
	// FIXME board specific?
	gpio_direction_output(hsic_gpio, 0);
	udelay(100);
	gpio_direction_output(hsic_gpio, 1);
	udelay(5000);

	clrbits_le32(&usb->hsicphyctrl1,
		     HOST_CTRL0_SIDDQ |
		     HOST_CTRL0_FORCESLEEP |
		     HOST_CTRL0_FORCESUSPEND);
	setbits_le32(&usb->hsicphyctrl1, HOST_CTRL0_PHYSWRST);
	udelay(10);
	clrbits_le32(&usb->hsicphyctrl1, HOST_CTRL0_PHYSWRST);

	/* PHY clock and power setup time */
	// FIXME If this happens more than 50ms before executing the payload,
	// we might not need this delay.
	mdelay(50);
}

void usb_init(device_t dev)
{
	struct usb_phy *usb;
	struct cpu_samsung_exynos5250_config *conf = dev->chip_info;

	usb_vbus_init(conf->usb_vbus_gpio);
	usb = (struct usb_phy *) samsung_get_base_usb_phy();
	setup_usb_phy(usb, conf->usb_hsic_gpio);
}
