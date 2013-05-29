/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "pch.h"

static void usb_xhci_clock_gating(device_t dev)
{
	u32 reg32;

	/* IOBP 0xE5004001[7:6] = 11b */
	pch_iobp_update(0xe5004001, ~0, (1 << 7)|(1 << 6));

	reg32 = pci_read_config32(dev, 0x40);
	reg32 &= ~(1 << 23); /* unsupported request */

	if (pch_is_lp()) {
		/* D20:F0:40h[18,17,8] = 111b */
		reg32 |= (1 << 18) | (1 << 17) | (1 << 8);
		/* D20:F0:40h[21,20,19] = 110b to enable XHCI Idle L1 */
		reg32 &= ~(1 << 19);
		reg32 |= (1 << 21) | (1 << 20);
	} else {
		/* D20:F0:40h[21,20,18,17,8] = 11111b */
		reg32 |= (1 << 21)|(1 << 20)|(1 << 18)|(1 << 17)|(1 << 8);
	}

	/* Avoid writing upper byte as it is write-once */
	pci_write_config16(dev, 0x40, (u16)(reg32 & 0xffff));
	pci_write_config8(dev, 0x40 + 2, (u8)((reg32 >> 16) & 0xff));

	/* D20:F0:44h[9,7,3] = 111b */
	reg32 = pci_read_config32(dev, 0x44);
	reg32 |= (1 << 9) | (1 << 7) | (1 << 3);
	pci_write_config32(dev, 0x44, reg32);

	reg32 = pci_read_config32(dev, 0xa0);
	if (pch_is_lp()) {
		/* D20:F0:A0h[18] = 1 */
		reg32 |= (1 << 18);
	} else {
		/* D20:F0:A0h[6] = 1 */
		reg32 |= (1 << 6);
	}
	pci_write_config32(dev, 0xa0, reg32);

	/* D20:F0:A4h[13] = 0 */
	reg32 = pci_read_config32(dev, 0xa4);
	reg32 &= ~(1 << 13);
	pci_write_config32(dev, 0xa4, reg32);
}

static void usb_xhci_init(device_t dev)
{
	struct resource *bar0 = find_resource(dev, PCI_BASE_ADDRESS_0);
	u32 reg32;

	if (!bar0 || bar0->base == 0 || bar0->base == 0xffffffff)
		return;

	/* Enable clock gating first */
	usb_xhci_clock_gating(dev);

	/* D20:F0:74h[1:0] = 11b (set D3Hot state) */
	reg32 = pci_read_config16(dev, 0x74);
	reg32 |= (1 << 1) | (1 << 0);
	pci_write_config16(dev, 0x74, reg32);

	reg32 = read32(bar0->base + 0x8144);
	if (pch_is_lp()) {
		/* XHCIBAR + 8144h[8,7,6] = 111b */
		reg32 |= (1 << 8) | (1 << 7) | (1 << 6);
	} else {
		/* XHCIBAR + 8144h[8,7,6] = 100b */
		reg32 &= ~((1 << 7) | (1 << 6));
		reg32 |= (1 << 8);
	}
	write32(bar0->base + 0x8144, reg32);

	if (pch_is_lp()) {
		/* XHCIBAR + 816Ch[19:0] = 000f0038h */
		reg32 = read32(bar0->base + 0x816c);
		reg32 &= ~0x000fffff;
		reg32 |= 0x000f0038;
		write32(bar0->base + 0x816c, reg32);

		/* D20:F0:B0h[17,14,13] = 100b */
		reg32 = pci_read_config32(dev, 0xb0);
		reg32 &= ~((1 << 14) | (1 << 13));
		reg32 |= (1 << 17);
		pci_write_config32(dev, 0xb0, reg32);

		/* XHCIBAR + 818Ch[7:0] = FFh */
		reg32 = read32(bar0->base + 0x818c);
		reg32 |= 0xff;
		write32(bar0->base + 0x818c, reg32);
	}

	reg32 = pci_read_config32(dev, 0x50);
	if (pch_is_lp()) {
		/* D20:F0:50h[28:0] = 0FCE2E5Fh */
		reg32 &= ~0x1fffffff;
		reg32 |= 0x0fce2e5f;
	} else {
		/* D20:F0:50h[26:0] = 07886E9Fh */
		reg32 &= ~0x07ffffff;
		reg32 |= 0x07886e9f;
	}
	pci_write_config32(dev, 0x50, reg32);

	/* D20:F0:44h[31] = 1 (Access Control Bit) */
	reg32 = pci_read_config32(dev, 0x40);
	reg32 |= (1 << 31);
	pci_write_config32(dev, 0x40, reg32);

	/* D20:F0:40h[31,23] = 10b (OC Configuration Done) */
	reg32 = pci_read_config32(dev, 0x40);
	reg32 &= ~(1 << 23); /* unsupported request */
	reg32 |= (1 << 31);
	pci_write_config32(dev, 0x40, reg32);
}

static void usb_xhci_set_subsystem(device_t dev, unsigned vendor,
				   unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations lops_pci = {
	.set_subsystem = &usb_xhci_set_subsystem,
};

static struct device_operations usb_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_xhci_init,
	.ops_pci		= &lops_pci,
};

static const unsigned short pci_device_ids[] = { 0x8c31, /* LynxPoint-H */
						 0x9c31, /* LynxPoint-LP */
						 0 };

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
