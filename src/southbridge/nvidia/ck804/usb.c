/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"

static void usb1_init(struct device *dev)
{
	struct southbridge_nvidia_ck804_config const *conf = dev->chip_info;

	if (!conf->usb1_hc_reset)
		return;

	/*
	 * Somehow the warm reset does not really reset the USB
	 * controller. Later, during boot, when the Bus Master bit is
	 * set, the USB controller trashes the memory, causing weird
	 * misbehavior. Was detected on Sun Ultra40, where mptable
	 * was damaged.
	 */
	u32 bar0 = pci_read_config32(dev, 0x10);
	u32 *regs = (u32 *) (bar0 & ~0xfff);

	/* OHCI USB HCCommandStatus Register, HostControllerReset bit */
	regs[2] |= 1;
}

static struct device_operations usb_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb1_init,
	// .enable        = ck804_enable,
	.scan_bus         = 0,
	.ops_pci          = &ck804_pci_ops,
};

static const struct pci_driver usb_driver __pci_driver = {
	.ops    = &usb_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_USB,
};
