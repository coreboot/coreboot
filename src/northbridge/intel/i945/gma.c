/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include "i945.h"

#define GDRST 0xc0

static void gma_func0_init(struct device *dev)
{
	u32 reg32;

	/* Unconditionally reset graphics */
	pci_write_config8(dev, GDRST, 1);
	udelay(50);
	pci_write_config8(dev, GDRST, 0);
	/* wait for device to finish */
	while (pci_read_config8(dev, GDRST) & 1) { };

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);


#if !CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);
#endif


#if CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	/* This should probably run before post VBIOS init. */
	printk(BIOS_SPEW, "Initializing VGA without OPROM.\n");
	u32 iobase, mmiobase, physbase, graphics_base;
	iobase = dev->resource_list[1].base;
	mmiobase = dev->resource_list[0].base;
	physbase = pci_read_config32(dev, 0x5c) & ~0xf;
	graphics_base = dev->resource_list[2].base + 0x20000 ;

	int i915lightup(u32 physbase, u32 iobase, u32 mmiobase, u32 gfx);
	i915lightup(physbase, iobase, mmiobase, graphics_base);
#endif

}

/* This doesn't reclaim stolen UMA memory, but IGD could still
   be reenabled later. */
static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = dev_find_slot(0, PCI_DEVFN(0x0, 0));

	pci_write_config16(dev, GCFC, 0xa00);
	pci_write_config16(dev_host, GGC, (1 << 1));

	unsigned int reg32 = pci_read_config32(dev_host, DEVEN);
	reg32 &= ~(DEVEN_D2F0 | DEVEN_D2F1);
	pci_write_config32(dev_host, DEVEN, reg32);

	dev->enabled = 0;
}

static void gma_func1_init(struct device *dev)
{
	u32 reg32;
	u8 val;

	/* IGD needs to be Bus Master, also enable IO accesss */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 |
			PCI_COMMAND_MASTER | PCI_COMMAND_IO);

	if (!get_option(&val, "tft_brightness"))
		pci_write_config8(dev, 0xf4, val);
	else
		pci_write_config8(dev, 0xf4, 0xff);
}

static void gma_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem    = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.scan_bus		= 0,
	.enable			= 0,
	.disable		= gma_func0_disable,
	.ops_pci		= &gma_pci_ops,
};


static struct device_operations gma_func1_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func1_init,
	.scan_bus		= 0,
	.enable			= 0,
	.ops_pci		= &gma_pci_ops,
};

static const struct pci_driver i945_gma_func0_driver __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27a2,
};

static const struct pci_driver i945_gma_func1_driver __pci_driver = {
	.ops	= &gma_func1_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27a6,
};

