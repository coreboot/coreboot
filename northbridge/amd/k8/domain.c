/*
 * K8 northbridge
 * This file is part of the coreboot project.
 * Copyright (C) 2004-2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> and Jason Schildt for Linux Networx)
 * Copyright (C) 2005-7 YingHai Lu
 * Copyright (C) 2005 Ollie Lo
 * Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2008-2009 Myles Watson <mylesgw@gmail.com>
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
#include <lapic.h>

#define K8_RESOURCE_INDEX(node, func, link, reg) (((node) << 16) | ((func<<8) | (reg) | (link)))
#define K8_RESOURCE_NODE(index) (((index) & 0x70000)>>16)
#define K8_RESOURCE_REG(index)  ((index) & 0xfc)
#define K8_RESOURCE_LINK(index)  ((index) & 0x3)
#define K8_RESOURCE_FUNC(index)  (((index) & 0x700)>>8)

#define FX_DEVS 8
extern struct device * __f0_dev[FX_DEVS];
u32 f1_read_config32(unsigned int reg);
void f1_write_config32(unsigned int reg, u32 value);
unsigned int amdk8_nodeid(struct device * dev);

static void tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;
	best = *best_p;
	if (!best || (best->base > new->base)) {
		best = new;
	}
	*best_p = best;
}

static u32 find_pci_tolm(struct bus *bus)
{
	struct resource *min;
	u32 tolm;
	min = NULL;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

/**
 * reg_useable
 * @param reg register to check
 * @param goal_dev device to own the resource
 * @param goal_nodeid node number
 * @param goal_link link number
 * @return 0 if not useable, 1 if useable, or 2 if the pair is free
 */
static int reg_useable(unsigned reg, struct device *dev, unsigned goal_nodeid,
		       unsigned goal_link)
{
	struct resource *res = NULL;
	unsigned nodeid = 0, link = 0;
	int result;

	/* Look for the resource that matches this register. */
	for(nodeid = 0; !res && (nodeid < FX_DEVS); nodeid++) {
		for (link = 0; !res && (link < 3); link++) {
			res = probe_resource(dev,K8_RESOURCE_INDEX(nodeid,1,link,reg));
		}
	}

	if (res) {
		/* If the resource is allocated to the link and node already. */
		if (0 && (goal_link == K8_RESOURCE_LINK(res->index)) &&
		    (goal_nodeid == K8_RESOURCE_NODE(res->index)) &&
		    (res->flags <= 1)) {
			printk(BIOS_DEBUG, "Re-allocated resource %lx!\n",
			       res->index);
			result = 1;
		}
		/* Allocated, but not to this node. */
		else
			result = 0;
	} else
		/* If no allocated resource was found, it is free - return 2. */
		result = 2;

	return result;
}

static struct resource *amdk8_find_regpair(struct device *dev, unsigned nodeid,
					   unsigned minreg, unsigned maxreg,
					  unsigned link)
{
	struct resource *resource;
	unsigned free_reg, reg;
	resource = NULL;
	free_reg = 0;
	for (reg = minreg; reg <= maxreg; reg += 0x8) {
		int result;
		result = reg_useable(reg, dev, nodeid, link);
		if (result == 1) {
			/* I have been allocated this one */
			break;
		} else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > maxreg) {
		reg = free_reg;
	}
	if (reg > 0) {
		resource = new_resource(dev, 0x100 + (reg | link));
	}
	return resource;
}

static struct resource *amdk8_find_iopair(struct device *dev, unsigned nodeid,
					  unsigned link)
{
	return amdk8_find_regpair(dev,nodeid,0xc0, 0xd8,link);
}

static struct resource *amdk8_find_mempair(struct device *dev, unsigned nodeid,
					   unsigned link)
{
	return amdk8_find_regpair(dev,nodeid,0x80, 0xb8,link);
}

static void amdk8_link_read_bases(struct device *dev, unsigned nodeid,
				  unsigned link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource = amdk8_find_iopair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2c(HT_IO_HOST_ALIGN);
		resource->gran = log2c(HT_IO_HOST_ALIGN);
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2c(HT_MEM_HOST_ALIGN);
		resource->gran = log2c(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
#ifdef CONFIG_PCI_64BIT_PREF_MEM
		resource->flags |= IORESOURCE_BRIDGE;
#endif
	}

	/* Initialize the memory constraints on the current bus */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2c(HT_MEM_HOST_ALIGN);
		resource->gran = log2c(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
	}
}

static void k8_pci_domain_read_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	unsigned link;

	for (link = 0; link < dev->links; link++) {
		if (dev->link[link].children) {
			amdk8_link_read_bases(dev, 0, link);
		}
	}
	printk(BIOS_DEBUG, "%s done\n", __func__);
}

static void amdk8_set_resource(struct device *dev, struct resource *resource)
{
	resource_t rbase, rend;
	unsigned reg, link, nodeid, func;
	char buf[50];

	printk(BIOS_DEBUG, "%s: %s@%lx flags %lx\n", __func__, dev->dtsname,
	       resource->index, resource->flags);

	/* Make certain the resource has actually been set. */
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		return;
	}

	/* If I have already stored this resource don't worry about it. */
	if (resource->flags & IORESOURCE_STORED) {
		return;
	}

	/* Only handle PCI memory and IO resources. */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Get the base address. */
	rbase = resource->base;

	/* Get the limit (rounded up). */
	rend = resource_end(resource);

	/* Get the register, nodeid, and link */
	nodeid = K8_RESOURCE_NODE(resource->index);
	reg = K8_RESOURCE_REG(resource->index);
	link = K8_RESOURCE_LINK(resource->index);
	func = K8_RESOURCE_FUNC(resource->index);

	if (func != 1) {
		printk(BIOS_INFO, "%s: K8 function %d register!\n", __func__,
		       func);
		return;
	}

	if (resource->flags & IORESOURCE_IO) {
		u32 base, limit;
		base = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base &= 0xfe000fcc;
		base |= rbase & 0x01fff000;
		base |= 3;
		limit &= 0xfe000fc8;
		limit |= rend & 0x01fff000;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);

		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
			printk(BIOS_SPEW,
			       "%s, enabling legacy VGA IO forwarding for %s link %x\n",
			       __func__, dev_path(dev), link);
			base |= PCI_IO_BASE_VGA_EN;
		}
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_NO_ISA) {
			base |= PCI_IO_BASE_NO_ISA;
		}

		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	} else if (resource->flags & IORESOURCE_MEM) {
		u32 base, limit;
		base = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base &= 0x000000f0;
		base |= (rbase >> 8) & 0xffffff00;
		base |= 3;
		limit &= 0x00000048;
		limit |= (rend >> 8) & 0xffffff00;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);
		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	resource->flags |= IORESOURCE_STORED;
	sprintf(buf, " <node %d link %d>", nodeid, link);
	report_resource_stored(dev, resource, buf);
}

#ifdef CONFIG_MULTIPLE_VGA_INIT
extern struct device *vga_pri;	// the primary vga device, defined in device.c
#endif

static void amdk8_create_vga_resource(struct device *dev)
{
	struct resource *resource;
	unsigned link;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = 0; link < dev->links; link++) {
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#ifdef CONFIG_MULTIPLE_VGA_INIT
			printk(BIOS_DEBUG,
			       "VGA: vga_pri bus num = %d dev->link[link] bus range [%d,%d]\n",
			       vga_pri->bus->secondary,
			       dev->link[link].secondary,
			       dev->link[link].subordinate);
			/* We need to make sure the vga_pri is under the link */
			if ((vga_pri->bus->secondary >=
			     dev->link[link].secondary)
			    && (vga_pri->bus->secondary <=
				dev->link[link].subordinate)
			    )
#endif
				break;
		}
	}

	/* no VGA card installed */
	if (link == dev->links)
		return;

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n",
	       dev_path(dev), 0, link);

	/* Allocate a resrouce for the legacy VGA buffer. */
	resource = amdk8_find_mempair(dev, 0, link);
	if (!resource) {
		printk(BIOS_DEBUG,
		       "VGA: Can not find free mmio reg for legacy VGA buffer\n");
		return;
	}
	resource->base = 0xa0000;
	resource->size = 0x20000;

	resource->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
}

static void amdk8_set_resources(struct device *dev)
{
	unsigned link;
	int i;

	printk(BIOS_DEBUG, "%s\n", __func__);

	printk(BIOS_DEBUG, "%s ops %p ops_pci_bus %p\n",
		dev->dtsname, dev->ops, dev->ops->ops_pci_bus);
	pci_check_pci_ops(dev->ops->ops_pci_bus);

	amdk8_create_vga_resource(dev);

	/* Set each resource we have found */
	for (i = 0; i < dev->resources; i++) {
		amdk8_set_resource(dev, &dev->resource[i]);
	}

	for (link = 0; link < dev->links; link++) {
		struct bus *bus;
		bus = &dev->link[link];
		if (bus->children) {
			phase4_set_resources(bus);
		}
	}
}

static void k8_pci_domain_set_resources(struct device * dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info get_hw_mem_hole_info(void);
	void disable_hoist_memory(unsigned long hole_startk, int i);
	u32 hoist_memory(unsigned long hole_startk, int i);
#endif
#ifdef CONFIG_PCI_64BIT_PREF_MEM
	struct resource *io, *mem1, *mem2;
	struct resource *resource, *last;
#endif
	unsigned long mmio_basek;
	u32 pci_tolm;
	int i, idx;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
	unsigned reset_memhole = 1;
#endif

	pci_tolm = find_pci_tolm(&dev->link[0]);

#warning "FIXME handle interleaved nodes"
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

#warning "FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M MMIO hole"
	/* Round the mmio hole to 64M */
	mmio_basek &= ~((64*1024) - 1);

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	/* If the hw mem hole is already set in raminit stage, here we will
	 * compare mmio_basek and hole_basek. If mmio_basek is bigger than
	 * hole_basek, we use hole_basek as mmio_basek and we don't need
	 * to reset hole. Otherwise, we reset the hole to the mmio_basek.
	 */

		mem_hole = get_hw_mem_hole_info();

		if ((mem_hole.node_id !=  -1) && (mmio_basek > mem_hole.hole_startk)) { //We will use hole_basek as mmio_basek, and we don't need to reset hole anymore
			mmio_basek = mem_hole.hole_startk;
			reset_memhole = 0;
		}

		//mmio_basek = 3*1024*1024; // for debug to meet boundary

		if(reset_memhole) {
			if(mem_hole.node_id!=-1) { // We need to select CONFIG_HW_MEM_HOLE_SIZEK for raminit, it can not make hole_startk to some basek too....!
				// We need to reset our Mem Hole, because We want more big HOLE than we already set
				//Before that We need to disable mem hole at first, becase memhole could already be set on i+1 instead
				disable_hoist_memory(mem_hole.hole_startk, mem_hole.node_id);
			}

		#if HW_MEM_HOLE_SIZE_AUTO_INC == 1
			//We need to double check if the mmio_basek is valid for hole setting, if it is equal to basek, we need to decrease it some
			u32 basek_pri;
			for (i = 0; i < 8; i++) {
				u32 base;
				u32 basek;
				base  = f1_read_config32(0x40 + (i << 3));
				if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
					continue;
				}

				basek = (base & 0xffff0000) >> 2;
				if(mmio_basek == basek) {
					mmio_basek -= (basek - basek_pri)>>1; // increase mem hole size to make sure it is on middle of pri node
					break;
				}
				basek_pri = basek;
			}
		#endif
		}

#endif

	idx = 0x10;
	for(i = 0; i < 8; i++) {
		u32 base, limit;
		unsigned basek, limitk, sizek;
		base  = f1_read_config32(0x40 + (i << 3));
		limit = f1_read_config32(0x44 + (i << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		basek = (base & 0xffff0000) >> 2;
		limitk = ((limit + 0x00010000) & 0xffff0000) >> 2;
		sizek = limitk - basek;

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < ((8*64)+(8*16))) && (sizek > ((8*64)+(16*16)))) {
			ram_resource(dev, (idx | i), basek, ((8*64)+(8*16)) - basek);
			idx += 0x10;
			basek = (8*64)+(16*16);
			sizek = limitk - ((8*64)+(16*16));

		}


		printk(BIOS_DEBUG, "node %d : mmio_basek=%08lx, basek=%08x, limitk=%08x\n", i, mmio_basek, basek, limitk); //yhlu

		/* See if I need to split the region to accomodate pci memory space */
		if ( (basek < 4*1024*1024 ) && (limitk > mmio_basek) ) {
			if (basek <= mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - basek;
				if(pre_sizek>0) {
					ram_resource(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
				}
				#if CONFIG_HW_MEM_HOLE_SIZEK != 0
				if(reset_memhole)
					sizek += hoist_memory(mmio_basek,i);
				#endif

				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4*1024*1024) {
				sizek = 0;
			}
			else {
				basek = 4*1024*1024;
				sizek -= (4*1024*1024 - mmio_basek);
			}
		}
		ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
	}

	amdk8_set_resources(dev);

	phase4_set_resources(&dev->link[0]);
}

static unsigned int amdk8_scan_chain(struct device *domain,
				     struct device* k8dev,
				     unsigned link, unsigned sblink,
				     unsigned int max, unsigned offset_unitid)
{

	u32 link_type;
	int i;
	u32 busses, config_busses;
	unsigned free_reg, config_reg;
	unsigned ht_unitid_base[4];	// here assume only 4 HT device on chain
	unsigned max_bus;
	unsigned min_bus;
	unsigned max_devfn;
	unsigned nodeid = amdk8_nodeid(k8dev);
	printk(BIOS_SPEW, "%s node %x link %x\n", __func__, nodeid, link);
	k8dev->link[link].cap = 0x80 + (link * 0x20);
	do {
		link_type = pci_read_config32(k8dev, k8dev->link[link].cap + 0x18);
	} while (link_type & ConnectionPending);
	if (!(link_type & LinkConnected)) {
		return max;
	}
	printk(BIOS_DEBUG, "amdk8_scan_chain: link %d is connected\n", link);
	do {
		link_type = pci_read_config32(k8dev, k8dev->link[link].cap + 0x18);
	} while (!(link_type & InitComplete));
	if (!(link_type & NonCoherent)) {
		return max;
	}
	/* See if there is an available configuration space mapping
	 * register in function 1.
	 */
	free_reg = 0;
	for (config_reg = 0xe0; config_reg <= 0xec; config_reg += 4) {
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
	if ((nodeid == 0) && (sblink == link)) {	// actually max is 0 here
		min_bus = max;
	}
	#if SB_HT_CHAIN_ON_BUS0 > 1
	// second chain will be on 0x40, third 0x80, forth 0xc0
	else {
		min_bus = ((max >> 6) + 1) * 0x40;
	}
	max = min_bus;
	#else
	//other ...
	else {
		min_bus = ++max;
	}
	#endif
#else
	min_bus = ++max;
#endif
	max_bus = 0xff;

	k8dev->link[link].secondary = min_bus;
	k8dev->link[link].subordinate = max_bus;

	/* Read the existing primary/secondary/subordinate bus
	 * number configuration.
	 */
	busses = pci_read_config32(k8dev, k8dev->link[link].cap + 0x14);
	config_busses = f1_read_config32(config_reg);

	/* Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagates by the bridge if it is
	 * not correctly configured
	 */
	busses &= 0xff000000;
	busses |= (((unsigned int)(k8dev->bus->secondary) << 0) |
		   ((unsigned int)(k8dev->link[link].secondary) << 8) |
		   ((unsigned int)(k8dev->link[link].subordinate) << 16));
	pci_write_config32(k8dev, k8dev->link[link].cap + 0x14, busses);

	config_busses &= 0x000fc88;
	config_busses |= (3 << 0) |	/* rw enable, no device compare */
	    ((nodeid & 7) << 4) |
	    ((link & 3) << 8) |
	    ((k8dev->link[link].secondary) << 16) |
	    ((k8dev->link[link].subordinate) << 24);
	f1_write_config32(config_reg, config_busses);

	/* Now we can scan all of the subordinate busses i.e. the
	 * chain on the hypertranport link
	 */
	for (i = 0; i < 4; i++) {
		ht_unitid_base[i] = 0x20;
	}

	max_devfn = PCI_DEVFN(0x1f, 7);

	max = hypertransport_scan_chain(k8dev, &domain->link[link], 0,
				        max_devfn, max, ht_unitid_base,
				        offset_unitid);

	/* We know the number of busses behind this bridge.  Set the
	 * subordinate bus number to it's real value
	 */
	k8dev->link[link].subordinate = max;
	busses = (busses & 0xff00ffff) |
	    ((unsigned int)(k8dev->link[link].subordinate) << 16);
	pci_write_config32(k8dev, k8dev->link[link].cap + 0x14, busses);

	config_busses = (config_busses & 0x00ffffff) |
	    (k8dev->link[link].subordinate << 24);
	f1_write_config32(config_reg, config_busses);

	{
		// config config_reg, and ht_unitid_base to update hcdn_reg;
		int index;
		unsigned temp = 0;
		index = (config_reg - 0xe0) >> 2;
		for (i = 0; i < 4; i++) {
			temp |= (ht_unitid_base[i] & 0xff) << (i * 8);
		}

		sysconf.hcdn_reg[index] = temp;

	}

	printk(BIOS_SPEW, "amdk8_scan_chain done\n");
	return max;
}

static unsigned int amdk8_scan_chains(struct device *domain, struct device *k8dev,
				      unsigned int max)
{
	unsigned nodeid;
	unsigned link;
	unsigned sblink = 0;
	unsigned offset_unitid = 0;
	nodeid = amdk8_nodeid(k8dev);

	printk(BIOS_DEBUG, "amdk8_scan_chains\n");

	if (nodeid == 0) {
		sblink = (pci_read_config32(k8dev, 0x64) >> 8) & 3;
#if SB_HT_CHAIN_ON_BUS0 > 0
#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
		offset_unitid = 1;
#endif
		// do southbridge ht chain first, in case s2885 put southbridge chain (8131/8111) on link2,
		// but put 8151 on link0
		max =
		    amdk8_scan_chain(domain, k8dev, sblink, sblink, max,
				     offset_unitid);
#endif
	}

	for (link = 0; link < k8dev->links; link++) {
#if SB_HT_CHAIN_ON_BUS0 > 0
		if ((nodeid == 0) && (sblink == link))
			continue;	//already done
#endif
		offset_unitid = 0;
#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
#if SB_HT_CHAIN_UNITID_OFFSET_ONLY == 1
		if ((nodeid == 0) && (sblink == link))
#endif
			offset_unitid = 1;
#endif

		max = amdk8_scan_chain(domain, k8dev, link, sblink, max,
				       offset_unitid);
	}

	return max;
}

static unsigned int k8_domain_scan_bus(struct device * dev, unsigned int max)
{
	unsigned reg;
	int i;

	printk(BIOS_DEBUG, "%s: %s \n", __func__, dev->dtsname);

	/* Unmap all of the HT chains */
	for(reg = 0xe0; reg <= 0xec; reg += 4) {
		f1_write_config32(reg, 0);
	}

	/* Tune the hypertransport transaction for best performance.
	 * Including enabling relaxed ordering if it is safe.
	 */
	for(i = 0; i < FX_DEVS; i++) {
		struct device * f0_dev;
		f0_dev = __f0_dev[i]; /* Initialized by f1_write_config32. */
		if (f0_dev && f0_dev->enabled) {
			u32 httc;
			httc = pci_read_config32(f0_dev, HT_TRANSACTION_CONTROL);
			httc &= ~HTTC_RSP_PASS_PW;
			if (!dev->link[0].disable_relaxed_ordering) {
				httc |= HTTC_RSP_PASS_PW;
			}
			printk(BIOS_SPEW, "%s passpw: %s\n",
				dev_path(dev),
				(!dev->link[0].disable_relaxed_ordering)?
				"enabled":"disabled");
			pci_write_config32(f0_dev, HT_TRANSACTION_CONTROL, httc);
		}
	}

	max = amdk8_scan_chains(dev, __f0_dev[0], max);

	return max;
}

static void k8_pci_domain_enable_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	enable_childrens_resources(dev);
}

struct device_operations k8domain_ops = {
	.id = {.type = DEVICE_ID_APIC_CLUSTER,
		{.pci_domain = {.vendor = PCI_VENDOR_ID_AMD,
		 		.device = 0x1100}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = k8_domain_scan_bus,
	.phase4_read_resources	 = k8_pci_domain_read_resources,
	.phase4_set_resources	 = k8_pci_domain_set_resources,
	.phase5_enable_resources = k8_pci_domain_enable_resources,
	.ops_pci_bus		 = &pci_cf8_conf1,
};
