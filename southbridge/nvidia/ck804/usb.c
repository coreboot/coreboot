/*
 * This file is part of the coreboot project.
 *
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <statictree.h>
#include "ck804.h"

static void usb1_init(struct device *dev)
{
	struct southbridge_nvidia_ck804_usb_config const *conf = dev->device_configuration;
	if (conf->usb1_hc_reset) {
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
}

struct device_operations ck804_usb_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			 .device = PCI_DEVICE_ID_NVIDIA_CK804_USB}}},
	.phase3_chip_setup_dev	 = ck804_enable,
	.phase3_scan		 = NULL,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = usb1_init,
	.ops_pci		 = &ck804_ops_pci,
};

