/*
 *
 * Copyright (C) 2014 Rockchip Electronics
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

#include <usb/usb.h>
#include "generic_hub.h"
#include "dwc2_private.h"
#include "dwc2.h"

static int
dwc2_rh_port_status_changed(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	int changed;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	changed = hprt.prtconndet;

	/* Clear connect detect flag */
	if (changed) {
		hprt.d32 &= HPRT_W1C_MASK;
		hprt.prtconndet = 1;
		writel(hprt.d32, dwc2->hprt0);
	}
	return changed;
}

static int
dwc2_rh_port_connected(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	return hprt.prtconnsts;
}

static int
dwc2_rh_port_in_reset(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	return hprt.prtrst;
}

static int
dwc2_rh_port_enabled(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	return hprt.prtena;
}

static usb_speed
dwc2_rh_port_speed(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	if (hprt.prtena) {
		switch (hprt.prtspd) {
		case PRTSPD_HIGH:
			return HIGH_SPEED;
		case PRTSPD_FULL:
			return FULL_SPEED;
		case PRTSPD_LOW:
			return LOW_SPEED;
		}
	}
	return -1;
}

static int
dwc2_rh_reset_port(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	hprt.d32 &= HPRT_W1C_MASK;
	hprt.prtrst = 1;
	writel(hprt.d32, dwc2->hprt0);

	/* Wait a bit while reset is active. */
	mdelay(50);

	/* Deassert reset. */
	hprt.prtrst = 0;
	writel(hprt.d32, dwc2->hprt0);

	/*
	 * If reset and speed enum success the DWC2 core will set enable bit
	 * after port reset bit is deasserted
	 */
	mdelay(1);
	hprt.d32 = readl(dwc2->hprt0);
	usb_debug("%s reset port ok, hprt = 0x%08x\n", __func__, hprt.d32);

	if (!hprt.prtena) {
		usb_debug("%s enable port fail! hprt = 0x%08x\n",
			  __func__, hprt.d32);
		return -1;
	}

	return 0;
}

static int
dwc2_rh_enable_port(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	/* Power on the port */
	hprt.d32 = readl(dwc2->hprt0);
	hprt.d32 &= HPRT_W1C_MASK;
	hprt.prtpwr = 1;
	writel(hprt.d32, dwc2->hprt0);
	return 0;
}

static int
dwc2_rh_disable_port(usbdev_t *const dev, const int port)
{
	hprt_t hprt;
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	hprt.d32 = readl(dwc2->hprt0);
	hprt.d32 &= HPRT_W1C_MASK;
	/* Disable the port*/
	hprt.prtena = 1;
	/* Power off the port */
	hprt.prtpwr = 0;
	writel(hprt.d32, dwc2->hprt0);
	return 0;
}

static const generic_hub_ops_t dwc2_rh_ops = {
	.hub_status_changed	= NULL,
	.port_status_changed	= dwc2_rh_port_status_changed,
	.port_connected		= dwc2_rh_port_connected,
	.port_in_reset		= dwc2_rh_port_in_reset,
	.port_enabled		= dwc2_rh_port_enabled,
	.port_speed		= dwc2_rh_port_speed,
	.enable_port		= dwc2_rh_enable_port,
	.disable_port		= dwc2_rh_disable_port,
	.start_port_reset	= NULL,
	.reset_port		= dwc2_rh_reset_port,
};

void
dwc2_rh_init(usbdev_t *dev)
{
	dwc_ctrl_t *const dwc2 = DWC2_INST(dev->controller);

	/* we can set them here because a root hub _really_ shouldn't
	   appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;

	generic_hub_init(dev, 1, &dwc2_rh_ops);
	usb_debug("dwc2_rh_init HPRT 0x%08x p = %p\n ",
		  readl(dwc2->hprt0), dwc2->hprt0);
	usb_debug("DWC2: root hub init done\n");
}
