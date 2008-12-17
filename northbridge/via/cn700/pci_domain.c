/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Aaron Lwe <aaron.lwe@gmail.com> for VIA)
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
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
#include <io.h>
#include <lib.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/x86/cpu.h>
#include <statictree.h>
#include <config.h>

static void tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;

	best = *best_p;
	if (!best || (best->base > new->base))
		best = new;
	*best_p = best;
}

static u32 find_pci_tolm(struct bus *bus)
{
	struct resource *min;
	u32 tolm;

	printk(BIOS_DEBUG, "Entering find_pci_tolm\n");

	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM,
			     tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base)
		tolm = min->base;

	printk(BIOS_DEBUG, "Leaving find_pci_tolm\n");

	return tolm;
}

static const u8 ramregs[4] = {0x43, 0x42, 0x41, 0x40};

static void cn700_pci_domain_set_resources(struct device *dev)
{
	struct device *mc_dev;
	u32 pci_tolm, tomk, tolmk;
	u8 rambits;
	int i, idx;

	printk(BIOS_SPEW, "Entering cn700 pci_domain_set_resources.\n");

#if 0
	if(!(pci_read_config8(dev, 0xe0) & (1 << 7)) || !(pci_read_config8(dev, 0x4f) & 1))
	{
		printk(BIOS_DEBUG, "Northbridge multifunction disabled, re-enabling");
		rambits = pci_read_config8(dev, 0x4f);
		rambits |= 1;
		pci_write_config8(dev, 0x4f, rambits);
	} else
		printk(BIOS_DEBUG, "Northrbidge multifunction enabled");
#endif

	pci_tolm = find_pci_tolm(&dev->link[0]);
	mc_dev = dev_find_pci_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_CN700_MEMCTRL, 0);
	

	/*
	 * Once the register value is not zero, the RAM size is
	 * this register's value multiply 64 * 1024 * 1024.
	 */
	for (rambits = 0, i = 0; i < ARRAY_SIZE(ramregs); i++) {
		rambits = pci_read_config8(mc_dev, ramregs[i]);
		if (rambits != 0)
			break;
	}

	tomk = rambits * 64 * 1024;
	printk(BIOS_SPEW, "tomk is 0x%x\n", tomk);
	/* Compute the Top Of Low Memory (TOLM), in Kb. */
	tolmk = pci_tolm >> 10;
	if (tolmk >= tomk) {
		/* The PCI hole does does not overlap the memory. */
		tolmk = tomk;
	}
	/* Report the memory regions. */
	idx = 10;
	/* TODO: Hole needed? */
	ram_resource(dev, idx++, 0, 640);	/* First 640k */
	/* Leave a hole for VGA, 0xa0000 - 0xc0000 */
	ram_resource(dev, idx++, 768,
		     (tolmk - 768 - (CONFIG_CN700_VIDEO_MB * 1024)));
	phase4_assign_resources(&dev->link[0]);
}

/** Operations for when the northbridge is running a PCI domain. */
struct device_operations cn700_north_domain = {
	.id = {.type = DEVICE_ID_PCI_DOMAIN,
		{.pci_domain = {.vendor = PCI_VENDOR_ID_VIA,
				     .device = PCI_DEVICE_ID_VIA_CN700_AGP}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= pci_domain_scan_bus,
	.phase4_read_resources		= pci_domain_read_resources,
	.phase4_set_resources		= cn700_pci_domain_set_resources,
	.phase5_enable_resources	= enable_childrens_resources,
	.phase6_init			= 0,
	.ops_pci_bus			= &pci_cf8_conf1,
};


