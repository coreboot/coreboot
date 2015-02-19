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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <soc/power.h>
#include <soc/sysreg.h>
#include <soc/usb.h>

static void reset_dwc3(struct exynos5_usb_drd_dwc3 *dwc3)
{
	setbits_le32(&dwc3->ctl, 0x1 << 11);		/* core soft reset */
	setbits_le32(&dwc3->usb3pipectl, 0x1 << 31);	/* PHY soft reset */
	setbits_le32(&dwc3->usb2phycfg, 0x1 << 31);	/* PHY soft reset */
}

void reset_usb_drd0_dwc3()
{
	printk(BIOS_DEBUG, "Starting DWC3 reset for USB DRD0\n");
	reset_dwc3(exynos_usb_drd0_dwc3);
}

void reset_usb_drd1_dwc3()
{
	printk(BIOS_DEBUG, "Starting DWC3 reset for USB DRD1\n");
	reset_dwc3(exynos_usb_drd1_dwc3);
}

static void setup_dwc3(struct exynos5_usb_drd_dwc3 *dwc3)
{
	if (!(dwc3->ctl & 0x1 << 11) ||
	    !(dwc3->usb3pipectl & 0x1 << 31) ||
	    !(dwc3->usb2phycfg & 0x1 << 31)) {
		printk(BIOS_ERR, "DWC3 at %p not in reset (you need to call "
		       "reset_usb_drdX_dwc3() first)!\n", dwc3);
	}

	/* Set relevant registers to default values (clearing all reset bits) */

	write32(&dwc3->usb3pipectl,
		0x1 << 24 | 0x4 << 19 | 0x1 << 18 | 0x1 << 17 | 0x1 << 1 | 0);

	/* Configure PHY clock turnaround for 8-bit UTMI+, disable suspend */
	write32(&dwc3->usb2phycfg, 0x9 << 10 | 0x1 << 8 | 0x1 << 6 | 0);

	write32(&dwc3->ctl, 0x5dc << 19 | 0x1 << 16 | 0x1 << 12 | 0);
}

void setup_usb_drd0_dwc3()
{
	setup_dwc3(exynos_usb_drd0_dwc3);
	printk(BIOS_DEBUG, "DWC3 setup for USB DRD0 finished\n");
}

void setup_usb_drd1_dwc3()
{
	setup_dwc3(exynos_usb_drd1_dwc3);
	printk(BIOS_DEBUG, "DWC3 setup for USB DRD1 finished\n");
}

static void setup_drd_phy(struct exynos5_usb_drd_phy *phy)
{
	/* Set all PHY registers to default values */

	/* XHCI Version 1.0, Frame Length adjustment 30 MHz */
	setbits_le32(&phy->linksystem, 0x1 << 27 | 0x20 << 1);

	/* Disable OTG, ID0 and DRVVBUS, do not force sleep/suspend */
	write32(&phy->utmi, 1 << 6);

	write32(&phy->clkrst,
		0x88 << 23 | 0x1 << 20 | 0x1 << 19 | 0x68 << 11 | 0x5 << 5 | 0x1 << 4 | 0x3 << 2 | 0x1 << 1 | 0x1 << 0 | 0);

	write32(&phy->param0,
		0x9 << 26 | 0x3 << 22 | 0x1 << 20 | 0x1 << 18 | 0x3 << 13 | 0x3 << 9 | 0x3 << 6 | 0x4 << 3 | 0x4 << 0 | 0);

	write32(&phy->param1,
		0x7f << 19 | 0x7f << 12 | 0x20 << 6 | 0x1c << 0 | 0);

	/* disable all test features */
	write32(&phy->test, 0);

	/* UTMI clock select? ("must be 0x1") */
	write32(&phy->utmiclksel, 0x1 << 2);

	/* Samsung magic, undocumented (from U-Boot) */
	write32(&phy->resume, 0x0);

	udelay(10);
	clrbits_le32(&phy->clkrst, 0x1 << 1);  /* deassert port reset */
}

void setup_usb_drd0_phy()
{
	printk(BIOS_DEBUG, "Powering up USB DRD0 PHY\n");
	setbits_le32(&exynos_power->usb_drd0_phy_ctrl, POWER_USB_PHY_CTRL_EN);
	setup_drd_phy(exynos_usb_drd0_phy);
}

void setup_usb_drd1_phy()
{
	printk(BIOS_DEBUG, "Powering up USB DRD1 PHY\n");
	setbits_le32(&exynos_power->usb_drd1_phy_ctrl, POWER_USB_PHY_CTRL_EN);
	setup_drd_phy(exynos_usb_drd1_phy);
}

void setup_usb_host_phy(int hsic_gpio)
{
	unsigned int hostphy_ctrl0;

	setbits_le32(&exynos_sysreg->usb20_phy_cfg, USB20_PHY_CFG_EN);
	setbits_le32(&exynos_power->usb_host_phy_ctrl, POWER_USB_PHY_CTRL_EN);

	printk(BIOS_DEBUG, "Powering up USB HOST PHY (%s HSIC)\n",
			hsic_gpio ? "with" : "without");

	hostphy_ctrl0 = read32(&exynos_usb_host_phy->usbphyctrl0);
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
	write32(&exynos_usb_host_phy->usbphyctrl0, hostphy_ctrl0);
	udelay(10);
	clrbits_le32(&exynos_usb_host_phy->usbphyctrl0,
		     HOST_CTRL0_LINKSWRST |
		     HOST_CTRL0_UTMISWRST);
	udelay(20);

	/* EHCI Ctrl setting */
	setbits_le32(&exynos_usb_host_phy->ehcictrl,
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

		clrbits_le32(&exynos_usb_host_phy->hsicphyctrl1,
			     HOST_CTRL0_SIDDQ |
			     HOST_CTRL0_FORCESLEEP |
			     HOST_CTRL0_FORCESUSPEND);
		setbits_le32(&exynos_usb_host_phy->hsicphyctrl1,
			     HOST_CTRL0_PHYSWRST);
		udelay(10);
		clrbits_le32(&exynos_usb_host_phy->hsicphyctrl1,
			     HOST_CTRL0_PHYSWRST);
	}

	/* At this point we need to wait for 50ms before talking to
	 * the USB controller (PHY clock and power setup time)
	 * By the time we are actually in the payload, these 50ms
	 * will have passed.
	 */
}
