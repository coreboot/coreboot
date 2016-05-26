/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Rockchip, Inc.
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
#include <soc/usb.h>

static void reset_dwc3(struct rockchip_usb_drd_dwc3 *dwc3_reg)
{
	/* Before Resetting PHY, put Core in Reset */
	setbits_le32(&dwc3_reg->ctl, DWC3_GCTL_CORESOFTRESET);
	/* Assert USB3 PHY reset */
	setbits_le32(&dwc3_reg->usb3pipectl, DWC3_GUSB3PIPECTL_PHYSOFTRST);
	/* Assert USB2 PHY reset */
	setbits_le32(&dwc3_reg->usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);
}

static void setup_dwc3(struct rockchip_usb_drd_dwc3 *dwc3_reg)
{
	u32 reg;
	u32 revision;
	u32 dwc3_hwparams1;

	/* Clear USB3 PHY reset */
	clrbits_le32(&dwc3_reg->usb3pipectl, DWC3_GUSB3PIPECTL_PHYSOFTRST);
	/* Clear USB2 PHY reset */
	clrbits_le32(&dwc3_reg->usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);
	/* After PHYs are stable we can take Core out of reset state */
	clrbits_le32(&dwc3_reg->ctl, DWC3_GCTL_CORESOFTRESET);

	revision = read32(&dwc3_reg->snpsid);
	/* This should read as U3 followed by revision number */
	if ((revision & DWC3_GSNPSID_MASK) != 0x55330000) {
		printk(BIOS_ERR, "ERROR: not a DesignWare USB3 DRD Core\n");
		return;
	}

	dwc3_hwparams1 = read32(&dwc3_reg->hwparams1);

	reg = read32(&dwc3_reg->ctl);
	reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
	reg &= ~DWC3_GCTL_DISSCRAMBLE;
	if (DWC3_GHWPARAMS1_EN_PWROPT(dwc3_hwparams1) ==
	    DWC3_GHWPARAMS1_EN_PWROPT_CLK)
		reg &= ~DWC3_GCTL_DSBLCLKGTNG;
	else
		printk(BIOS_DEBUG, "No power optimization available\n");

	write32(&dwc3_reg->ctl, reg);

	/* We are hard-coding DWC3 core to Host Mode */
	clrsetbits_le32(&dwc3_reg->ctl,
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));
	/*
	 * Configure USB phy interface of DWC3 core.
	 * For rockchip rk3399 SOC DWC3 core:
	 * 1. Clear U2_FREECLK_EXITS.
	 * 2. Select UTMI+ PHY with 16-bit interface.
	 * 3. Set USBTRDTIM to the corresponding value
	 * according to the UTMI+ PHY interface.
	 */
	reg = read32(&dwc3_reg->usb2phycfg);
	reg &= ~(DWC3_GUSB2PHYCFG_U2_FREECLK_EXISTS |
		DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK |
		DWC3_GUSB2PHYCFG_PHYIF_MASK);
	reg |= DWC3_GUSB2PHYCFG_PHYIF(1) |
	       DWC3_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_16_BIT);
	write32(&dwc3_reg->usb2phycfg, reg);
}

void reset_usb_drd0_dwc3(void)
{
	printk(BIOS_DEBUG, "Starting DWC3 reset for USB DRD0\n");
	reset_dwc3(rockchip_usb_drd0_dwc3);
}

void reset_usb_drd1_dwc3(void)
{
	printk(BIOS_DEBUG, "Starting DWC3 reset for USB DRD1\n");
	reset_dwc3(rockchip_usb_drd1_dwc3);
}

void setup_usb_drd0_dwc3(void)
{
	setup_dwc3(rockchip_usb_drd0_dwc3);
	printk(BIOS_DEBUG, "DWC3 setup for USB DRD0 finished\n");
}

void setup_usb_drd1_dwc3(void)
{
	setup_dwc3(rockchip_usb_drd1_dwc3);
	printk(BIOS_DEBUG, "DWC3 setup for USB DRD1 finished\n");
}
