/*
 * K8 northbridge
 * This file is part of the coreboot project.
 * Copyright (C) 2004-2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> and Jason Schildt for Linux Networx)
 * Copyright (C) 2005-7 YingHai Lu
 * Copyright (C) 2005 Ollie Lo
 * Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
/* This should be done by Eric
	2004.12 yhlu add dual core support
	2005.01 yhlu add support move apic before pci_domain in MB Config.lb
	2005.02 yhlu add e0 memory hole support
	2005.11 yhlu add put sb ht chain on bus 0
*/

#include <mainboard.h>
#include <console.h>
#include <lib.h>
#include <string.h>
#include <mtrr.h>
#include <macros.h>
#include <spd.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <device/hypertransport_def.h>
#include <device/hypertransport.h>
#include <mc146818rtc.h>
#include <lib.h>
#include  <lapic.h>

#define FX_DEVS 8
extern struct device * __f0_dev[FX_DEVS];
extern void get_fx_devs(void);
u32 f1_read_config32(unsigned int reg);
void f1_write_config32(unsigned int reg, u32 value);
unsigned int amdk8_nodeid(struct device * dev);

static unsigned int amdk8_scan_chain(struct device * dev, unsigned nodeid, unsigned link, unsigned sblink, unsigned int max, unsigned offset_unitid)
{

		u32 link_type;
		int i;
		u32 busses, config_busses;
		unsigned free_reg, config_reg;
		unsigned ht_unitid_base[4]; // here assume only 4 HT device on chain
		unsigned max_bus;
		unsigned min_bus;
		unsigned max_devfn;
		printk(BIOS_SPEW, "amdk8_scan_chain link %x\n",link);
		dev->link[link].cap = 0x80 + (link *0x20);
		do {
			link_type = pci_read_config32(dev, dev->link[link].cap + 0x18);
		} while(link_type & ConnectionPending);
		if (!(link_type & LinkConnected)) {
			return max;
		}
		printk(BIOS_DEBUG, "amdk8_scan_chain: link %d is connected\n", link);
		do {
			link_type = pci_read_config32(dev, dev->link[link].cap + 0x18);
		} while(!(link_type & InitComplete));
		if (!(link_type & NonCoherent)) {
			return max;
		}
		/* See if there is an available configuration space mapping
		 * register in function 1.
		 */
		free_reg = 0;
		for(config_reg = 0xe0; config_reg <= 0xec; config_reg += 4) {
			u32 config;
			config = f1_read_config32(config_reg);
			if (!free_reg && ((config & 3) == 0)) {
				free_reg = config_reg;
				continue;
			}
			if (((config & 3) == 3) &&
				(((config >> 4) & 7) == nodeid) &&
				(((config >> 8) & 3) == link)) {
				break;
			}
		}
		if (free_reg && (config_reg > 0xec)) {
			config_reg = free_reg;
		}
		/* If we can't find an available configuration space mapping
		 * register skip this bus
		 */
		if (config_reg > 0xec) {
			return max;
		}

		/* Set up the primary, secondary and subordinate bus numbers.
		 * We have no idea how many busses are behind this bridge yet,
		 * so we set the subordinate bus number to 0xff for the moment.
		 */
#if SB_HT_CHAIN_ON_BUS0 > 0
		// first chain will on bus 0
		if((nodeid == 0) && (sblink==link)) { // actually max is 0 here
			min_bus = max;
		}
	#if SB_HT_CHAIN_ON_BUS0 > 1
		// second chain will be on 0x40, third 0x80, forth 0xc0
		else {
			min_bus = ((max>>6) + 1) * 0x40;
		}
		max = min_bus;
	#else
		//other ...
		else  {
			min_bus = ++max;
		}
	#endif
#else
		min_bus = ++max;
#endif
		max_bus = 0xff;

		dev->link[link].secondary = min_bus;
		dev->link[link].subordinate = max_bus;

		/* Read the existing primary/secondary/subordinate bus
		 * number configuration.
		 */
		busses = pci_read_config32(dev, dev->link[link].cap + 0x14);
		config_busses = f1_read_config32(config_reg);

		/* Configure the bus numbers for this bridge: the configuration
		 * transactions will not be propagates by the bridge if it is
		 * not correctly configured
		 */
		busses &= 0xff000000;
		busses |= (((unsigned int)(dev->bus->secondary) << 0) |
			((unsigned int)(dev->link[link].secondary) << 8) |
			((unsigned int)(dev->link[link].subordinate) << 16));
		pci_write_config32(dev, dev->link[link].cap + 0x14, busses);

		config_busses &= 0x000fc88;
		config_busses |=
			(3 << 0) |  /* rw enable, no device compare */
			(( nodeid & 7) << 4) |
			(( link & 3 ) << 8) |
			((dev->link[link].secondary) << 16) |
			((dev->link[link].subordinate) << 24);
		f1_write_config32(config_reg, config_busses);

		/* Now we can scan all of the subordinate busses i.e. the
		 * chain on the hypertranport link
		 */
		for(i=0;i<4;i++) {
			ht_unitid_base[i] = 0x20;
		}

		if (min_bus == 0)
			max_devfn = (0x17<<3) | 7;
		else
			max_devfn = (0x1f<<3) | 7;

		max = hypertransport_scan_chain(&dev->link[link], 0, max_devfn, max, ht_unitid_base, offset_unitid);

		/* We know the number of busses behind this bridge.  Set the
		 * subordinate bus number to it's real value
		 */
		dev->link[link].subordinate = max;
		busses = (busses & 0xff00ffff) |
			((unsigned int) (dev->link[link].subordinate) << 16);
		pci_write_config32(dev, dev->link[link].cap + 0x14, busses);

		config_busses = (config_busses & 0x00ffffff) |
			(dev->link[link].subordinate << 24);
		f1_write_config32(config_reg, config_busses);

		{
			// config config_reg, and ht_unitid_base to update hcdn_reg;
			int index;
			unsigned temp = 0;
			index = (config_reg-0xe0) >> 2;
			for(i=0;i<4;i++) {
				temp |= (ht_unitid_base[i] & 0xff) << (i*8);
			}

			sysconf.hcdn_reg[index] = temp;

		}

	printk(BIOS_SPEW, "amdk8_scan_chain done\n");
	return max;
}

static unsigned int amdk8_scan_chains(struct device * dev, unsigned int max)
{
	unsigned nodeid;
	unsigned link;
	unsigned sblink = 0;
	unsigned offset_unitid = 0;
	nodeid = amdk8_nodeid(dev);

	printk(BIOS_DEBUG, "amdk8_scan_chains\n");

	if(nodeid==0) {
		sblink = (pci_read_config32(dev, 0x64)>>8) & 3;
#if SB_HT_CHAIN_ON_BUS0 > 0
	#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
		offset_unitid = 1;
	#endif
		// do southbridge ht chain first, in case s2885 put southbridge chain (8131/8111) on link2,
		// but put 8151 on link0
		max = amdk8_scan_chain(dev, nodeid, sblink, sblink, max, offset_unitid );
#endif
	}

	for(link = 0; link < dev->links; link++) {
#if SB_HT_CHAIN_ON_BUS0 > 0
		if( (nodeid == 0) && (sblink == link) ) continue; //already done
#endif
		offset_unitid = 0;
		#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
			#if SB_HT_CHAIN_UNITID_OFFSET_ONLY == 1
			if((nodeid == 0) && (sblink == link))
			#endif
				offset_unitid = 1;
		#endif

		max = amdk8_scan_chain(dev, nodeid, link, sblink, max, offset_unitid);
	}

	return max;
}

/** 
 * reg_useable
 * @param reg register to check
 * @param goal_dev device to own the resource
 * @param goal_nodeid node number
 * @param goal_link link number
 * @return 0 if not useable, 1 if useable, or 2 if the pair is free
 * __f0 is initialized once in amdk8_read_resources
 */
static int reg_useable(unsigned reg,
	struct device * goal_dev, unsigned goal_nodeid, unsigned goal_link)
{
	struct resource *res;
	unsigned nodeid, link=0;
	int result;
	res = NULL;

	/* Look for the resource that matches this register. */
	for(nodeid = 0; !res && (nodeid < CONFIG_MAX_PHYSICAL_CPUS); nodeid++) {
		struct device * dev;
		dev = __f0_dev[nodeid];
		if (! dev)
			continue;
		for(link = 0; !res && (link < 3); link++) {
			res = probe_resource(dev, 0x100 + (reg | link));
		}
	}
	
	/* If no allocated resource was found, it is free - return 2 */
	result = 2;
	if (res) {
		result = 0;
		/* If the resource is allocated to the link and node already */
		if (	(goal_link == (link - 1)) &&
			(goal_nodeid == (nodeid - 1)) &&
			(res->flags <= 1)) {
			result = 1;
		}
	}

	return result;
}

static struct resource *amdk8_find_iopair(struct device * dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;
	unsigned free_reg, reg;
	resource = NULL;
	free_reg = 0;
	for(reg = 0xc0; reg <= 0xd8; reg += 0x8) {
		int result;
		result = reg_useable(reg, dev, nodeid, link);
		if (result == 1) {
			/* I have been allocated this one */
			break;
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xd8) {
		reg = free_reg;
	}
	if (reg > 0) {
		resource = new_resource(dev, 0x100 + (reg | link));
	}
	return resource;
}

static struct resource *amdk8_find_mempair(struct device * dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;
	unsigned free_reg, reg;
	resource = NULL;
	free_reg = 0;
	for(reg = 0x80; reg <= 0xb8; reg += 0x8) {
		int result;
		result = reg_useable(reg, dev, nodeid, link);
		if (result == 1) {
			/* I have been allocated this one */
			break;
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xb8) {
		reg = free_reg;
	}
	if (reg > 0) {
		resource = new_resource(dev, 0x100 + (reg | link));
	}
	return resource;
}

static void amdk8_link_read_bases(struct device * dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource =  amdk8_find_iopair(dev, nodeid, link);
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2c(HT_IO_HOST_ALIGN);
		resource->gran  = log2c(HT_IO_HOST_ALIGN);
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_IO, IORESOURCE_IO);
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2c(HT_MEM_HOST_ALIGN);
		resource->gran  = log2c(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH,
			IORESOURCE_MEM | IORESOURCE_PREFETCH);
	}

	/* Initialize the memory constraints on the current bus */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2c(HT_MEM_HOST_ALIGN);
		resource->gran  = log2c(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH,
			IORESOURCE_MEM);
	}
}

static void amdk8_read_resources(struct device * dev)
{
	printk(BIOS_DEBUG, "amdk8_read_resources\n");
	unsigned nodeid, link;
	nodeid = amdk8_nodeid(dev);

	get_fx_devs(); /* Make sure __f0 is initialized*/

	for(link = 0; link < dev->links; link++) {
		if (dev->link[link].children) {
			printk(BIOS_DEBUG, "amdk8_read_resources link %d\n", link);
			amdk8_link_read_bases(dev, nodeid, link);
		}
	}
	printk(BIOS_DEBUG, "amdk8_read_resources done\n");
}

static void amdk8_set_resource(struct device * dev, struct resource *resource, unsigned nodeid)
{
	resource_t rbase, rend;
	unsigned reg, link;
	char buf[50];
	printk(BIOS_DEBUG, "amdk8_set_resource\n");
	/* Make certain the resource has actually been set */
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		return;
	}

	/* If I have already stored this resource don't worry about it */
	if (resource->flags & IORESOURCE_STORED) {
		return;
	}

	/* Only handle PCI memory and IO resources */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Ensure I am actually looking at a resource of function 1 */
	if (resource->index < 0x100) {
		return;
	}
	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend  = resource_end(resource);

	/* Get the register and link */
	reg  = resource->index & 0xfc;
	link = resource->index & 3;

	if (resource->flags & IORESOURCE_IO) {
		u32 base, limit;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_IO, IORESOURCE_IO);
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0xfe000fcc;
		base  |= rbase  & 0x01fff000;
		base  |= 3;
		limit &= 0xfe000fc8;
		limit |= rend & 0x01fff000;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);

		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
			printk(BIOS_SPEW, "%s, enabling legacy VGA IO forwarding for %s link %x\n",
				    __func__, dev_path(dev), link);
			base |= PCI_IO_BASE_VGA_EN;
		}
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_NO_ISA) {
			base |= PCI_IO_BASE_NO_ISA;
		}

		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	else if (resource->flags & IORESOURCE_MEM) {
		u32 base, limit;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH,
			resource->flags & (IORESOURCE_MEM | IORESOURCE_PREFETCH));
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0x000000f0;
		base  |= (rbase >> 8) & 0xffffff00;
		base  |= 3;
		limit &= 0x00000048;
		limit |= (rend >> 8) & 0xffffff00;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);
		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	resource->flags |= IORESOURCE_STORED;
	sprintf(buf, " <node %d link %d>",
		nodeid, link);
	report_resource_stored(dev, resource, buf);
}

/**
 *
 * I tried to reuse the resource allocation code in amdk8_set_resource()
 * but it is too diffcult to deal with the resource allocation magic.
 */
#ifdef CONFIG_MULTIPLE_VGA_INIT
extern struct device * vga_pri;	// the primary vga device, defined in device.c
#endif

static void amdk8_create_vga_resource(struct device * dev, unsigned nodeid)
{
	struct resource *resource;
	unsigned link;
	u32 base, limit;
	unsigned reg;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = 0; link < dev->links; link++) {
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#ifdef CONFIG_MULTIPLE_VGA_INIT
			printk(BIOS_DEBUG, "VGA: vga_pri bus num = %d dev->link[link] bus range [%d,%d]\n", vga_pri->bus->secondary,
				dev->link[link].secondary,dev->link[link].subordinate);
			/* We need to make sure the vga_pri is under the link */
			if((vga_pri->bus->secondary >= dev->link[link].secondary ) &&
				(vga_pri->bus->secondary <= dev->link[link].subordinate )
			)
#endif
			break;
		}
	}

	/* no VGA card installed */
	if (link == dev->links)
		return;

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, link);

	/* allocate a temp resrouce for legacy VGA buffer */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if(!resource){
		printk(BIOS_DEBUG, "VGA: Can not find free mmio reg for legacy VGA buffer\n");
		return;
	}
	resource->base = 0xa0000;
	resource->size = 0x20000;

	/* write the resource to the hardware */
	reg  = resource->index & 0xfc;
	base  = f1_read_config32(reg);
	limit = f1_read_config32(reg + 0x4);
	base  &= 0x000000f0;
	base  |= (resource->base >> 8) & 0xffffff00;
	base  |= 3;
	limit &= 0x00000048;
	limit |= (resource_end(resource) >> 8) & 0xffffff00;
	limit |= (resource->index & 3) << 4;
	limit |= (nodeid & 7);
	f1_write_config32(reg + 0x4, limit);
	f1_write_config32(reg, base);

	/* release the temp resource */
	resource->flags = 0;
}

static void amdk8_set_resources(struct device * dev)
{
	unsigned nodeid, link;
	int i;

	/* Find the nodeid */
	nodeid = amdk8_nodeid(dev);

	printk(BIOS_DEBUG, "amdk8_set_resources: nodeid %d\n", nodeid);
	amdk8_create_vga_resource(dev, nodeid);

	/* Set each resource we have found */
	for(i = 0; i < dev->resources; i++) {
		amdk8_set_resource(dev, &dev->resource[i], nodeid);
	}

	for(link = 0; link < dev->links; link++) {
		struct bus *bus;
		bus = &dev->link[link];
		if (bus->children) {
			phase4_assign_resources(bus);
		}
	}
}

static void amdk8_enable_resources(struct device * dev)
{
	printk(BIOS_DEBUG, "amdk8_enable_resources\n");
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static void mcf0_control_init(struct device *dev)
{
	printk(BIOS_DEBUG, "NB: Function 0 Misc Control.. Nothing to do ...");

	printk(BIOS_DEBUG, "done.\n");
}

#ifdef CONFIG_PCI_64BIT_PREF_MEM
#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM | IORESOURCE_PREFETCH)
#endif

struct device_operations k8_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = 0x1100}}},
	.constructor		 = default_device_constructor,
	.reset_bus		 = pci_bus_reset,
	.phase3_scan		 = amdk8_scan_chains,
	.phase4_read_resources	 = amdk8_read_resources,
	.phase4_set_resources	 = amdk8_set_resources,
	.phase5_enable_resources = amdk8_enable_resources,
	.phase6_init		 = mcf0_control_init,
	.ops_pci		 = &pci_dev_ops_pci,
	.ops_pci_bus      = &pci_cf8_conf1,
};
