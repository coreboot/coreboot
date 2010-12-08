/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "mcp55.h"
#include <usbdebug.h>

extern struct ehci_debug_info dbg_info;

static void usb2_init(struct device *dev)
{
	uint32_t dword;
	dword = pci_read_config32(dev, 0xf8);
	dword |= 40;
	pci_write_config32(dev, 0xf8, dword);
}

static void usb2_set_resources(struct device *dev)
{
#if CONFIG_USBDEBUG
	struct resource *res;
	unsigned base;
	unsigned old_debug;

	old_debug = get_ehci_debug();
	set_ehci_debug(0);
#endif
	pci_dev_set_resources(dev);

#if CONFIG_USBDEBUG
	res = find_resource(dev, 0x10);
	set_ehci_debug(old_debug);
	if (!res) return;
	base = res->base;
	set_ehci_base(base);
	report_resource_stored(dev, res, "");
#endif

}

static struct device_operations usb2_ops  = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= usb2_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= usb2_init,
//	.enable		= mcp55_enable,
	.scan_bus	= 0,
	.ops_pci	= &mcp55_pci_ops,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops	= &usb2_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.device	= PCI_DEVICE_ID_NVIDIA_MCP55_USB2,
};
