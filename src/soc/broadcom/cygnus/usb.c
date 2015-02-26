/*
 * Copyright (C) 2014 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <delay.h>
#include <console/console.h>
#include <soc/cygnus.h>

#define CDRU_USBPHY_CLK_RST_SEL_OFFSET			0x11b4
#define CDRU_USBPHY2_HOST_DEV_SEL_OFFSET		0x11b8
#define CDRU_SPARE_REG_0_OFFSET				0x1238
#define CRMU_USB_PHY_AON_CTRL_OFFSET			0x00028
#define CDRU_USB_DEV_SUSPEND_RESUME_CTRL_OFFSET		0x1210
#define CDRU_USBPHY_P2_STATUS_OFFSET			0x1200

#define CDRU_USB_DEV_SUSPEND_RESUME_CTRL_DISABLE	0
#define PHY2_DEV_HOST_CTRL_SEL_DEVICE			0
#define PHY2_DEV_HOST_CTRL_SEL_HOST			1
#define CDRU_USBPHY_P2_STATUS__USBPHY_ILDO_ON_FLAG	1
#define CDRU_USBPHY_P2_STATUS__USBPHY_PLL_LOCK		0
#define CRMU_USBPHY_P0_AFE_CORERDY_VDDC			1
#define CRMU_USBPHY_P0_RESETB				2
#define CRMU_USBPHY_P1_AFE_CORERDY_VDDC			9
#define CRMU_USBPHY_P1_RESETB				10
#define CRMU_USBPHY_P2_AFE_CORERDY_VDDC			17
#define CRMU_USBPHY_P2_RESETB				18

#define USB2_IDM_IDM_IO_CONTROL_DIRECT_OFFSET		0x0408
#define USB2_IDM_IDM_IO_CONTROL_DIRECT__clk_enable	0
#define USB2_IDM_IDM_RESET_CONTROL_OFFSET		0x0800
#define USB2_IDM_IDM_RESET_CONTROL__RESET		0

#define PLL_LOCK_RETRY_COUNT	1000
#define MAX_REGULATOR_NAME_LEN  25
#define NUM_PHYS		3

struct bcm_phy_instance {
	struct phy *generic_phy;
	int port;
	int host_mode; /* 1 - Host , 0 - device */
	int power; /* 1 -powered_on 0 -powered off */
	struct regulator *vbus_supply;
};

struct bcm_phy_driver {
	void *usbphy_regs;
	void *usb2h_idm_regs;
	void *usb2d_idm_regs;
	int num_phys, idm_host_enabled;
	struct bcm_phy_instance instances[NUM_PHYS];
};

static struct bcm_phy_driver phy_driver;

static int bcm_phy_init(struct bcm_phy_instance *instance_ptr)
{
	/* Only PORT 2 is capabale of being device and host
	 * Default setting is device, check if it is set to host */
	if (instance_ptr->port == 2) {
		if (instance_ptr->host_mode == PHY2_DEV_HOST_CTRL_SEL_HOST)
			write32(phy_driver.usbphy_regs + CDRU_USBPHY2_HOST_DEV_SEL_OFFSET,
				PHY2_DEV_HOST_CTRL_SEL_HOST);
		else
			die("usb device mode unsupported\n");
	}

	return 0;
}

static int bcm_phy_poweron(struct bcm_phy_instance *instance_ptr)
{
	int clock_reset_flag = 1;
	u32 val;

	/* Bring the AFE block out of reset to start powering up the PHY */
	val = read32(phy_driver.usbphy_regs + CRMU_USB_PHY_AON_CTRL_OFFSET);
	if (instance_ptr->port == 0)
		val |= (1 << CRMU_USBPHY_P0_AFE_CORERDY_VDDC);
	else if (instance_ptr->port == 1)
		val |= (1 << CRMU_USBPHY_P1_AFE_CORERDY_VDDC);
	else if (instance_ptr->port == 2)
		val |= (1 << CRMU_USBPHY_P2_AFE_CORERDY_VDDC);
	write32(phy_driver.usbphy_regs + CRMU_USB_PHY_AON_CTRL_OFFSET, val);

	instance_ptr->power = 1;

	/* Check if the port 2 is configured for device */
	if (instance_ptr->port == 2 &&
		instance_ptr->host_mode == PHY2_DEV_HOST_CTRL_SEL_DEVICE)
		die("usb device mode unsupported\n");

	val = read32(phy_driver.usbphy_regs + CDRU_USBPHY_CLK_RST_SEL_OFFSET);

	/* Check if the phy that is configured
	 * to provide clock and reset is powered on*/
	if (val >= 0 && val < phy_driver.num_phys) {
		if (phy_driver.instances[val].power == 1)
			clock_reset_flag = 0;
	}

	/* if not set the current phy */
	if (clock_reset_flag) {
		val = instance_ptr->port;
		write32(phy_driver.usbphy_regs + CDRU_USBPHY_CLK_RST_SEL_OFFSET,
			val);
	}

	if (phy_driver.idm_host_enabled != 1) {
		/* Enable clock to USB and get the USB out of reset */
		setbits_le32(phy_driver.usb2h_idm_regs +
			     USB2_IDM_IDM_IO_CONTROL_DIRECT_OFFSET,
			     (1 << USB2_IDM_IDM_IO_CONTROL_DIRECT__clk_enable));
		clrbits_le32(phy_driver.usb2h_idm_regs +
			     USB2_IDM_IDM_RESET_CONTROL_OFFSET,
			     (1 << USB2_IDM_IDM_RESET_CONTROL__RESET));
		phy_driver.idm_host_enabled = 1;
	}

	return 0;
}

static int bcm_phy_probe(void)
{
	int i;

	phy_driver.num_phys = NUM_PHYS;
	phy_driver.usbphy_regs = (void *)0x0301c000;
	phy_driver.usb2h_idm_regs = (void *)0x18115000;
	phy_driver.usb2d_idm_regs = (void *)0x18111000;
	phy_driver.idm_host_enabled = 0;

	/* Shutdown all ports. They can be powered up as required */
	clrbits_le32(phy_driver.usbphy_regs + CRMU_USB_PHY_AON_CTRL_OFFSET,
		     (1 << CRMU_USBPHY_P0_AFE_CORERDY_VDDC) |
		     (1 << CRMU_USBPHY_P0_RESETB) |
		     (1 << CRMU_USBPHY_P1_AFE_CORERDY_VDDC) |
		     (1 << CRMU_USBPHY_P1_RESETB) |
		     (1 << CRMU_USBPHY_P2_AFE_CORERDY_VDDC) |
		     (1 << CRMU_USBPHY_P2_RESETB));

	for (i = 0; i < phy_driver.num_phys; i++) {
		phy_driver.instances[i].port = i;
		phy_driver.instances[i].host_mode = PHY2_DEV_HOST_CTRL_SEL_HOST;
	}

	return 0;
}

void usb_init(void)
{
	bcm_phy_probe();
	/* currently, we only need thus support port 0 */
	bcm_phy_init(&phy_driver.instances[0]);
	bcm_phy_poweron(&phy_driver.instances[0]);
	printk(BIOS_INFO, "usb phy[%d] is powered on\n", 0);
}
