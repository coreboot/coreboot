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

#include <delay.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include "cpu.h"
#include "gpio.h"
#include "power.h"
#include "sysreg.h"
#include "usb.h"

void setup_usb_host_phy(int hsic_gpio)
{
	unsigned int hostphy_ctrl0;
	struct exynos5_sysreg *sysreg = samsung_get_base_sysreg();
	struct exynos5_power *power = samsung_get_base_power();
	struct exynos5_usb_host_phy *phy = samsung_get_base_usb_host_phy();

	setbits_le32(&sysreg->usb20_phy_cfg, USB20_PHY_CFG_EN);
	setbits_le32(&power->usb_host_phy_ctrl, POWER_USB_HOST_PHY_CTRL_EN);

	printk(BIOS_DEBUG, "Powering up USB HOST PHY (%s HSIC)\n",
			hsic_gpio ? "with" : "without");

	hostphy_ctrl0 = readl(&phy->usbphyctrl0);
	hostphy_ctrl0 &= ~(HOST_CTRL0_FSEL_MASK |
			   HOST_CTRL0_COMMONON_N |
			   /* HOST Phy setting */
			   HOST_CTRL0_PHYSWRST |
			   HOST_CTRL0_PHYSWRSTALL |
			   HOST_CTRL0_SIDDQ |
			   HOST_CTRL0_FORCESUSPEND |
			   HOST_CTRL0_FORCESLEEP);
	hostphy_ctrl0 |= (/* Setting up the ref freq */
			  CLK_24MHZ << 16 |
			  /* HOST Phy setting */
			  HOST_CTRL0_LINKSWRST |
			  HOST_CTRL0_UTMISWRST);
	writel(hostphy_ctrl0, &phy->usbphyctrl0);
	udelay(10);
	clrbits_le32(&phy->usbphyctrl0,
		     HOST_CTRL0_LINKSWRST |
		     HOST_CTRL0_UTMISWRST);
	udelay(20);

	/* EHCI Ctrl setting */
	setbits_le32(&phy->ehcictrl,
		     EHCICTRL_ENAINCRXALIGN |
		     EHCICTRL_ENAINCR4 |
		     EHCICTRL_ENAINCR8 |
		     EHCICTRL_ENAINCR16);

	/* HSIC USB Hub initialization. */
	if (hsic_gpio) {
		gpio_direction_output(hsic_gpio, 0);
		udelay(100);
		gpio_direction_output(hsic_gpio, 1);
		udelay(5000);

		clrbits_le32(&phy->hsicphyctrl1,
			     HOST_CTRL0_SIDDQ |
			     HOST_CTRL0_FORCESLEEP |
			     HOST_CTRL0_FORCESUSPEND);
		setbits_le32(&phy->hsicphyctrl1, HOST_CTRL0_PHYSWRST);
		udelay(10);
		clrbits_le32(&phy->hsicphyctrl1, HOST_CTRL0_PHYSWRST);
	}

	/* At this point we need to wait for 50ms before talking to
	 * the USB controller (PHY clock and power setup time)
	 * By the time we are actually in the payload, these 50ms
	 * will have passed.
	 */
}
