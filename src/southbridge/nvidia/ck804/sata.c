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
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"

#ifndef CK804_SATA_RESET_FOR_ATAPI
#define CK804_SATA_RESET_FOR_ATAPI 0
#endif

#if CK804_SATA_RESET_FOR_ATAPI
static void sata_com_reset(struct device *dev, unsigned reset)
// reset = 1 : reset
// reset = 0 : clear
{
	u32 *base;
	u32 dword;
	int loop;

	base = (u32 *) pci_read_config32(dev, 0x24);

	printk(BIOS_DEBUG, "base = %08lx\n", base);

	if (reset) {
		*(base + 4) = 0xffffffff;
		*(base + 0x44) = 0xffffffff;
	}

	dword = *(base + 8);
	dword &= ~(0xf);
	dword |= reset;

	*(base + 8) = dword;
	*(base + 0x48) = dword;

#if 0
	udelay(1000);
	dword &= ~(0xf);
	*(base + 8) = dword;
	*(base + 0x48) = dword;
#endif

	if (reset)
		return;

	dword = *(base + 0);
	printk(BIOS_DEBUG, "*(base+0)=%08x\n", dword);
	if (dword == 0x113) {
		loop = 200000;	// 2
		do {
			dword = *(base + 4);
			if ((dword & 0x10000) != 0)
				break;
			udelay(10);
		} while (--loop > 0);
		printk(BIOS_DEBUG, "loop=%d, *(base+4)=%08x\n", loop, dword);
	}

	dword = *(base + 0x40);
	printk(BIOS_DEBUG, "*(base+0x40)=%08x\n", dword);
	if (dword == 0x113) {
		loop = 200000;	//2
		do {
			dword = *(base + 0x44);
			if ((dword & 0x10000) != 0)
				break;
			udelay(10);
		} while (--loop > 0);
		printk(BIOS_DEBUG, "loop=%d, *(base+0x44)=%08x\n", loop, dword);
	}
}
#endif

static void sata_init(struct device *dev)
{
	u32 dword;
	struct southbridge_nvidia_ck804_config *conf;

	conf = dev->chip_info;

	dword = pci_read_config32(dev, 0x50);
	/* Ensure prefetch is disabled. */
	dword &= ~((1 << 15) | (1 << 13));
	if (conf->sata1_enable) {
		/* Enable secondary SATA interface. */
		dword |= (1 << 0);
		printk(BIOS_DEBUG, "SATA S \t");
	}
	if (conf->sata0_enable) {
		/* Enable primary SATA interface. */
		dword |= (1 << 1);
		printk(BIOS_DEBUG, "SATA P \n");
	}
#if 0
	/* Write back */
	dword |= (1 << 12);
	dword |= (1 << 14);
#endif

#if 0
	/* ADMA */
	dword |= (1 << 16);
	dword |= (1 << 17);
#endif

#if 1
	/* DO NOT relay OK and PAGE_FRNDLY_DTXFR_CNT. */
	dword &= ~(0x1f << 24);
	dword |= (0x15 << 24);
#endif
	pci_write_config32(dev, 0x50, dword);

#if 0
	/* SLUMBER_DURING_D3 */
	dword = pci_read_config32(dev, 0x7c);
	dword &= ~(1 << 4);
	pci_write_config32(dev, 0x7c, dword);

	dword = pci_read_config32(dev, 0xd0);
	dword &= ~(0xff << 24);
	dword |= (0x68 << 24);
	pci_write_config32(dev, 0xd0, dword);

	dword = pci_read_config32(dev, 0xe0);
	dword &= ~(0xff << 24);
	dword |= (0x68 << 24);
	pci_write_config32(dev, 0xe0, dword);
#endif

	dword = pci_read_config32(dev, 0xf8);
	dword |= 2;
	pci_write_config32(dev, 0xf8, dword);

#if CK804_SATA_RESET_FOR_ATAPI
	dword = pci_read_config32(dev, 0xac);
	dword &= ~((1 << 13) | (1 << 14));
	dword |= (1 << 13) | (0 << 14);
	pci_write_config32(dev, 0xac, dword);

	sata_com_reset(dev, 1);	/* For discover some s-atapi device. */
#endif
}

static struct device_operations sata_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	// .enable        = ck804_enable,
	.init             = sata_init,
	.scan_bus         = 0,
	.ops_pci          = &ck804_pci_ops,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_SATA0,
};

static const struct pci_driver sata1_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_SATA1,
};
