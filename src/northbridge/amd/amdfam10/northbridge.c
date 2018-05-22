/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2015 - 2017 Timothy Pearson <tpearson@raptorengineering.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <lib.h>
#include <smbios.h>
#include <cpu/cpu.h>
#include <delay.h>

#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdfam10_sysconf.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/family_10h-family_15h/ram_calc.h>

#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
#include <cpu/amd/multicore.h>
#include <pc80/mc146818rtc.h>
#endif

#include "northbridge.h"
#include "amdfam10.h"
#include "ht_config.h"
#include "chip.h"

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
#include <cpu/amd/model_10xxx_rev.h>
#endif

#if IS_ENABLED(CONFIG_AMD_SB_CIMX)
#include <sb_cimx.h>
#endif

#if IS_ENABLED(CONFIG_DIMM_DDR3)
#include "../amdmct/mct_ddr3/s3utils.h"
#endif

struct amdfam10_sysconf_t sysconf;

#define FX_DEVS NODE_NUMS
static struct device *__f0_dev[FX_DEVS];
struct device *__f1_dev[FX_DEVS];
static struct device *__f2_dev[FX_DEVS];
static struct device *__f4_dev[FX_DEVS];
static unsigned fx_devs = 0;

struct device *get_node_pci(u32 nodeid, u32 fn)
{
#if NODE_NUMS + CONFIG_CDB >= 32
	if ((CONFIG_CDB + nodeid) < 32) {
		return dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB + nodeid, fn));
	} else {
		return dev_find_slot(CONFIG_CBB-1, PCI_DEVFN(CONFIG_CDB + nodeid - 32, fn));
	}

#else
	return dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB + nodeid, fn));
#endif
}

static void get_fx_devs(void)
{
	int i;
	for (i = 0; i < FX_DEVS; i++) {
		__f0_dev[i] = get_node_pci(i, 0);
		__f1_dev[i] = get_node_pci(i, 1);
		__f2_dev[i] = get_node_pci(i, 2);
		__f4_dev[i] = get_node_pci(i, 4);
		if (__f0_dev[i] != NULL && __f1_dev[i] != NULL)
			fx_devs = i+1;
	}
	if (__f1_dev[0] == NULL || __f0_dev[0] == NULL || fx_devs == 0) {
		die("Cannot find 0:0x18.[0|1]\n");
	}
}

static u32 f1_read_config32(unsigned reg)
{
	if (fx_devs == 0)
		get_fx_devs();
	return pci_read_config32(__f1_dev[0], reg);
}

static void f1_write_config32(unsigned reg, u32 value)
{
	int i;
	if (fx_devs == 0)
		get_fx_devs();
	for (i = 0; i < fx_devs; i++) {
		struct device *dev;
		dev = __f1_dev[i];
		if (dev && dev->enabled) {
			pci_write_config32(dev, reg, value);
		}
	}
}

u32 amdfam10_nodeid(struct device *dev)
{
#if NODE_NUMS == 64
	unsigned busn;
	busn = dev->bus->secondary;
	if (busn != CONFIG_CBB) {
		return (dev->path.pci.devfn >> 3) - CONFIG_CDB + 32;
	} else {
		return (dev->path.pci.devfn >> 3) - CONFIG_CDB;
	}

#else
	return (dev->path.pci.devfn >> 3) - CONFIG_CDB;
#endif
}

static void set_vga_enable_reg(u32 nodeid, u32 linkn)
{
	u32 val;

	val =  1 | (nodeid<<4) | (linkn<<12);
	/* it will routing (1)mmio  0xa0000:0xbffff (2) io 0x3b0:0x3bb,
	 0x3c0:0x3df */
	f1_write_config32(0xf4, val);

}

typedef enum {
	HT_ROUTE_CLOSE,
	HT_ROUTE_SCAN,
	HT_ROUTE_FINAL,
} scan_state;

static void ht_route_link(struct bus *link, scan_state mode)
{
	struct bus *parent = link->dev->bus;
	u32 busses;

	if (mode == HT_ROUTE_SCAN) {
		if (parent->subordinate == 0)
			link->secondary = 0;
		else
			link->secondary = parent->subordinate + 1;

		link->subordinate = link->secondary;
	}

	/* Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagated by the bridge if it is
	 * not correctly configured
	 */
	busses = pci_read_config32(link->dev, link->cap + 0x14);
	busses &= ~(0xff << 8);
	busses |= parent->secondary & 0xff;
	if (mode == HT_ROUTE_CLOSE)
		busses |= 0xff << 8;
	else if (mode == HT_ROUTE_SCAN)
		busses |= ((u32) link->secondary & 0xff) << 8;
	else if (mode == HT_ROUTE_FINAL)
		busses |= ((u32) link->secondary & 0xff) << 8;
	pci_write_config32(link->dev, link->cap + 0x14, busses);

	if (mode == HT_ROUTE_FINAL) {
		if (CONFIG_HT_CHAIN_DISTRIBUTE)
			parent->subordinate = ALIGN_UP(link->subordinate, 8) - 1;
		else
			parent->subordinate = link->subordinate;
	}
}

static void amd_g34_fixup(struct bus *link, struct device *dev)
{
	uint32_t nodeid = amdfam10_nodeid(dev);
	uint8_t rev_gte_d = 0;
	uint8_t dual_node = 0;
	uint32_t f3xe8;

	if (cpuid_eax(0x80000001) >= 0x8)
		/* Revision D or later */
		rev_gte_d = 1;

	if (rev_gte_d || is_fam15h()) {
		f3xe8 = pci_read_config32(get_node_pci(0, 3), 0xe8);

		/* Check for dual node capability */
		if (f3xe8 & 0x20000000)
			dual_node = 1;

		if (dual_node) {
			/* Each G34 processor contains a defective HT link.
			* See the BKDG Rev 3.62 section 2.7.1.5 for details.
			*/
			f3xe8 = pci_read_config32(get_node_pci(nodeid, 3), 0xe8);
			uint8_t internal_node_number = ((f3xe8 & 0xc0000000) >> 30);
			uint8_t defective_link_number_1;
			uint8_t defective_link_number_2;
			if (is_fam15h()) {
				defective_link_number_1 = 4;	/* Link 0 Sublink 1 */
				defective_link_number_2 = 7;	/* Link 3 Sublink 1 */
			} else {
				defective_link_number_1 = 6;	/* Link 2 Sublink 1 */
				defective_link_number_2 = 5;	/* Link 1 Sublink 1 */
			}
			if (internal_node_number == 0) {
				/* Node 0 */
				if (link->link_num == 6)	/* Link 2 Sublink 1 */
					printk(BIOS_DEBUG, "amdfam10_scan_chain(): node %d (internal node ID %d): skipping defective HT link\n", nodeid, internal_node_number);
			} else {
				/* Node 1 */
				if (link->link_num == 5)	/* Link 1 Sublink 1 */
					printk(BIOS_DEBUG, "amdfam10_scan_chain(): node %d (internal node ID %d): skipping defective HT link\n", nodeid, internal_node_number);
			}
		}
	}
}

static void amdfam10_scan_chain(struct bus *link)
{
		unsigned int next_unitid;

		/* See if there is an available configuration space mapping
		 * register in function 1.
		 */
		if (get_ht_c_index(link) >= 4)
			return;

		/* Set up the primary, secondary and subordinate bus numbers.
		 * We have no idea how many busses are behind this bridge yet,
		 * so we set the subordinate bus number to 0xff for the moment.
		 */

		ht_route_link(link, HT_ROUTE_SCAN);

		/* set the config map space */
		set_config_map_reg(link);

		/* Now we can scan all of the subordinate busses i.e. the
		 * chain on the hypertranport link
		 */

		next_unitid = hypertransport_scan_chain(link);

		/* Now that nothing is overlapping it is safe to scan the children. */
		pci_scan_bus(link, 0x00, ((next_unitid - 1) << 3) | 7);

		ht_route_link(link, HT_ROUTE_FINAL);

		/* We know the number of busses behind this bridge.  Set the
		 * subordinate bus number to it's real value
		 */
		if (0) {
			/* Clear the extend reg. */
			clear_config_map_reg(link);
		}

		set_config_map_reg(link);

		store_ht_c_conf_bus(link);
}

/* Do sb ht chain at first, in case s2885 put sb chain
 * (8131/8111) on link2, but put 8151 on link0.
 */
static void relocate_sb_ht_chain(void)
{
	struct device *dev;
	struct bus *link, *prev = NULL;
	u8 sblink;

	dev = dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB, 0));
	sblink = (pci_read_config32(dev, 0x64)>>8) & 7;
	link = dev->link_list;

	while (link) {
		if (link->link_num == sblink) {
			if (!prev)
				return;
			prev->next = link->next;
			link->next = dev->link_list;
			dev->link_list = link;
			return;
		}
		prev = link;
		link = link->next;
	}
}

static void trim_ht_chain(struct device *dev)
{
	struct bus *link;

	/* Check for connected link. */
	for (link = dev->link_list; link; link = link->next) {
		link->cap = 0x80 + (link->link_num * 0x20);
		link->ht_link_up = ht_is_non_coherent_link(link);
	}
}

static void amdfam10_scan_chains(struct device *dev)
{
	struct bus *link;

#if IS_ENABLED(CONFIG_CPU_AMD_SOCKET_G34_NON_AGESA)
	if (is_fam15h()) {
		uint8_t current_link_number = 0;

		for (link = dev->link_list; link; link = link->next) {
			/* The following links have changed position in Fam15h G34 processors:
			 * Fam10  Fam15
			 * Node 0
			 * L3 --> L1
			 * L0 --> L3
			 * L1 --> L2
			 * L2 --> L0
			 * Node 1
			 * L0 --> L0
			 * L1 --> L3
			 * L2 --> L1
			 * L3 --> L2
			 */
			if (link->link_num == 0)
				link->link_num = 3;
			else if (link->link_num == 1)
				link->link_num = 2;
			else if (link->link_num == 2)
				link->link_num = 0;
			else if (link->link_num == 3)
				link->link_num = 1;
			else if (link->link_num == 5)
				link->link_num = 7;
			else if (link->link_num == 6)
				link->link_num = 5;
			else if (link->link_num == 7)
				link->link_num = 6;

			current_link_number++;
			if (current_link_number > 3)
				current_link_number = 0;
		}
	}
#endif

	/* Do sb ht chain at first, in case s2885 put sb chain (8131/8111) on link2, but put 8151 on link0 */
	trim_ht_chain(dev);

	for (link = dev->link_list; link; link = link->next) {
		if (link->ht_link_up) {
			if (IS_ENABLED(CONFIG_CPU_AMD_MODEL_10XXX))
				amd_g34_fixup(link, dev);
			amdfam10_scan_chain(link);
		}
	}
}


static int reg_useable(unsigned reg, struct device *goal_dev, unsigned goal_nodeid,
			unsigned goal_link)
{
	struct resource *res;
	unsigned nodeid, link = 0;
	int result;
	res = 0;
	for (nodeid = 0; !res && (nodeid < fx_devs); nodeid++) {
		struct device *dev;
		dev = __f0_dev[nodeid];
		if (!dev)
			continue;
		for (link = 0; !res && (link < 8); link++) {
			res = probe_resource(dev, IOINDEX(0x1000 + reg, link));
		}
	}
	result = 2;
	if (res) {
		result = 0;
		if (	(goal_link == (link - 1)) &&
			(goal_nodeid == (nodeid - 1)) &&
			(res->flags <= 1)) {
			result = 1;
		}
	}
	return result;
}

static struct resource *amdfam10_find_iopair(struct device *dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;
	u32 free_reg, reg;
	resource = 0;
	free_reg = 0;
	for (reg = 0xc0; reg <= 0xd8; reg += 0x8) {
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
	if (reg > 0xd8) {
		reg = free_reg; // if no free, the free_reg still be 0
	}

	//Ext conf space
	if (!reg) {
		//because of Extend conf space, we will never run out of reg, but we need one index to differ them. so same node and same link can have multi range
		u32 index = get_io_addr_index(nodeid, link);
		reg = 0x110+ (index<<24) + (4<<20); // index could be 0, 255
	}

		resource = new_resource(dev, IOINDEX(0x1000 + reg, link));

	return resource;
}

static struct resource *amdfam10_find_mempair(struct device *dev, u32 nodeid, u32 link)
{
	struct resource *resource;
	u32 free_reg, reg;
	resource = 0;
	free_reg = 0;
	for (reg = 0x80; reg <= 0xb8; reg += 0x8) {
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
	if (reg > 0xb8) {
		reg = free_reg;
	}

	//Ext conf space
	if (!reg) {
		//because of Extend conf space, we will never run out of reg,
		// but we need one index to differ them. so same node and
		// same link can have multi range
		u32 index = get_mmio_addr_index(nodeid, link);
		reg = 0x110+ (index<<24) + (6<<20); // index could be 0, 63

	}
	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));
	return resource;
}


static void amdfam10_link_read_bases(struct device *dev, u32 nodeid, u32 link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource = amdfam10_find_iopair(dev, nodeid, link);
	if (resource) {
		u32 align;
		align = log2(HT_IO_HOST_ALIGN);
		resource->base	= 0;
		resource->size	= 0;
		resource->align = align;
		resource->gran	= align;
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = amdfam10_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
		resource->flags |= IORESOURCE_BRIDGE;
	}

	/* Initialize the memory constraints on the current bus */
	resource = amdfam10_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
	}
}

static void amdfam10_read_resources(struct device *dev)
{
	u32 nodeid;
	struct bus *link;
	nodeid = amdfam10_nodeid(dev);
	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			amdfam10_link_read_bases(dev, nodeid, link->link_num);
		}
	}
}

static void amdfam10_set_resource(struct device *dev, struct resource *resource,
				u32 nodeid)
{
	resource_t rbase, rend;
	unsigned reg, link_num;
	char buf[50];

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
	if ((resource->index & 0xffff) < 0x1000) {
		return;
	}
	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend  = resource_end(resource);

	/* Get the register and link */
	reg  = resource->index & 0xfff; // 4k
	link_num = IOINDEX_LINK(resource->index);

	if (resource->flags & IORESOURCE_IO) {

		set_io_addr_reg(dev, nodeid, link_num, reg, rbase>>8, rend>>8);
		store_conf_io_addr(nodeid, link_num, reg, (resource->index >> 24), rbase>>8, rend>>8);
	} else if (resource->flags & IORESOURCE_MEM) {
		set_mmio_addr_reg(nodeid, link_num, reg, (resource->index >>24), rbase>>8, rend>>8, sysconf.nodes); // [39:8]
		store_conf_mmio_addr(nodeid, link_num, reg, (resource->index >>24), rbase>>8, rend>>8);
	}
	resource->flags |= IORESOURCE_STORED;
	snprintf(buf, sizeof(buf), " <node %x link %x>",
		 nodeid, link_num);
	report_resource_stored(dev, resource, buf);
}

/**
 * I tried to reuse the resource allocation code in amdfam10_set_resource()
 * but it is too difficult to deal with the resource allocation magic.
 */

static void amdfam10_create_vga_resource(struct device *dev, unsigned nodeid)
{
	struct bus *link;
	struct resource *res;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list; link; link = link->next) {
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if IS_ENABLED(CONFIG_MULTIPLE_VGA_ADAPTERS)
			extern struct device *vga_pri; // the primary vga device, defined in device.c
			printk(BIOS_DEBUG, "VGA: vga_pri bus num = %d bus range [%d,%d]\n", vga_pri->bus->secondary,
				link->secondary,link->subordinate);
			/* We need to make sure the vga_pri is under the link */
			if ((vga_pri->bus->secondary >= link->secondary) &&
			    (vga_pri->bus->secondary <= link->subordinate))
#endif
			break;
		}
	}

	/* no VGA card installed */
	if (link == NULL)
		return;

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, link->link_num);
	set_vga_enable_reg(nodeid, link->link_num);

	/* Redirect VGA memory access to MMIO
	 * This signals the Family 10h resource parser
	 * to add a new MMIO mapping to the Range 11
	 * MMIO control registers (starting at F1x1B8),
	 * and also reserves the resource in the E820 map.
	 */
	res = new_resource(dev, IOINDEX(0x1000 + 0x1b8, link->link_num));
	res->base = 0xa0000;
	res->size = 0x20000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	amdfam10_set_resource(dev, res, nodeid);
}

static void amdfam10_set_resources(struct device *dev)
{
	unsigned nodeid;
	struct bus *bus;
	struct resource *res;

	/* Find the nodeid */
	nodeid = amdfam10_nodeid(dev);

	amdfam10_create_vga_resource(dev, nodeid);

	/* Set each resource we have found */
	for (res = dev->resource_list; res; res = res->next) {
		amdfam10_set_resource(dev, res, nodeid);
	}

	for (bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children) {
			assign_resources(bus);
		}
	}
}

static void mcf0_control_init(struct device *dev)
{
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static const char *amdfam10_northbridge_acpi_name(const struct device *dev)
{
	return "";
}
#endif

static struct device_operations northbridge_operations = {
	.read_resources	  = amdfam10_read_resources,
	.set_resources	  = amdfam10_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = mcf0_control_init,
	.scan_bus	  = amdfam10_scan_chains,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt_generator = northbridge_acpi_write_vars,
	.acpi_name = amdfam10_northbridge_acpi_name,
#endif
	.enable		  = 0,
	.ops_pci	  = 0,
};

static const struct pci_driver mcf0_driver __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1200,
};

static void amdfam10_nb_init(void *chip_info)
{
	relocate_sb_ht_chain();
}

static const struct pci_driver mcf0_driver_fam15_model10 __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1400,
};

static const struct pci_driver mcf0_driver_fam15 __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1600,
};

struct chip_operations northbridge_amd_amdfam10_ops = {
	CHIP_NAME("AMD Family 10h/15h Northbridge")
	.enable_dev = 0,
	.init = amdfam10_nb_init,
};

static void amdfam10_domain_read_resources(struct device *dev)
{
	unsigned reg;
	uint8_t nvram;
	uint8_t enable_cc6;

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for (reg = 0x80; reg <= 0xd8; reg+= 0x08) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned nodeid, reg_link;
			struct device *reg_dev;
			if (reg < 0xc0) { // mmio
				nodeid = (limit & 0xf) + (base&0x30);
			} else { // io
				nodeid =  (limit & 0xf) + ((base>>4)&0x30);
			}
			reg_link = (limit >> 4) & 7;
			reg_dev = __f0_dev[nodeid];
			if (reg_dev) {
				/* Reserve the resource  */
				struct resource *res;
				res = new_resource(reg_dev, IOINDEX(0x1000 + reg, reg_link));
				if (res) {
					res->flags = 1;
				}
			}
		}
	}
	/* FIXME: do we need to check extend conf space?
	   I don't believe that much preset value */

	pci_domain_read_resources(dev);

	/* We have MMCONF_SUPPORT, create the resource window. */
	mmconf_resource(dev, 0xc0010058);

	/* Reserve lower DRAM region to force PCI MMIO region to correct location above 0xefffffff */
	ram_resource(dev, 7, 0, rdmsr(TOP_MEM).lo >> 10);

	if (is_fam15h()) {
		enable_cc6 = 0;
		if (get_option(&nvram, "cpu_cc6_state") == CB_SUCCESS)
			enable_cc6 = !!nvram;

		if (enable_cc6) {
			uint8_t node;
			uint8_t interleaved;
			int8_t range;
			int8_t max_range;
			uint8_t max_node;
			uint64_t max_range_limit;
			uint32_t dword;
			uint32_t dword2;
			uint64_t qword;
			uint8_t num_nodes;

			/* Find highest DRAM range (DramLimitAddr) */
			num_nodes = 0;
			max_node = 0;
			max_range = -1;
			interleaved = 0;
			max_range_limit = 0;
			struct device *node_dev;
			for (node = 0; node < FX_DEVS; node++) {
				node_dev = get_node_pci(node, 0);
				/* Test for node presence */
				if ((!node_dev) || (pci_read_config32(node_dev, PCI_VENDOR_ID) == 0xffffffff))
					continue;

				num_nodes++;
				for (range = 0; range < 8; range++) {
					dword = pci_read_config32(get_node_pci(node, 1), 0x40 + (range * 0x8));
					if (!(dword & 0x3))
						continue;

					if ((dword >> 8) & 0x7)
						interleaved = 1;

					dword = pci_read_config32(get_node_pci(node, 1), 0x44 + (range * 0x8));
					dword2 = pci_read_config32(get_node_pci(node, 1), 0x144 + (range * 0x8));
					qword = 0xffffff;
					qword |= ((((uint64_t)dword) >> 16) & 0xffff) << 24;
					qword |= (((uint64_t)dword2) & 0xff) << 40;

					if (qword > max_range_limit) {
						max_range = range;
						max_range_limit = qword;
						max_node = dword & 0x7;
					}
				}
			}

			/* Calculate CC6 storage area size */
			if (interleaved)
				qword = (0x1000000 * num_nodes);
			else
				qword = 0x1000000;

			/* FIXME
			 * The BKDG appears to be incorrect as to the location of the CC6 save region
			 * lower boundary on non-interleaved systems, causing lockups on attempted write
			 * to the CC6 save region.
			 *
			 * For now, work around by allocating the maximum possible CC6 save region size.
			 *
			 * Determine if this is a BKDG error or a setup problem and remove this warning!
			 */
			qword = (0x1 << 27);
			max_range_limit = (((uint64_t)(pci_read_config32(get_node_pci(max_node, 1), 0x124) & 0x1fffff)) << 27) - 1;

			printk(BIOS_INFO, "Reserving CC6 save segment base: %08llx size: %08llx\n", (max_range_limit + 1), qword);

			/* Reserve the CC6 save segment */
			reserved_ram_resource(dev, 8, (max_range_limit + 1) >> 10, qword >> 10);
		}
	}
}

static u32 my_find_pci_tolm(struct bus *bus, u32 tolm)
{
	struct resource *min;
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0

struct hw_mem_hole_info {
	unsigned hole_startk;
	int node_id;
};

static struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
		struct hw_mem_hole_info mem_hole;
		int i;

		mem_hole.hole_startk = CONFIG_HW_MEM_HOLE_SIZEK;
		mem_hole.node_id = -1;

		for (i = 0; i < sysconf.nodes; i++) {
			struct dram_base_mask_t d;
			u32 hole;
			d = get_dram_base_mask(i);
			if (!(d.mask & 1)) continue; // no memory on this node

			hole = pci_read_config32(__f1_dev[i], 0xf0);
			if (hole & 1) { // we find the hole
				mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
				mem_hole.node_id = i; // record the node No with hole
				break; // only one hole
			}
		}

		/* We need to double check if there is special set on base reg and limit reg
		 * are not continuous instead of hole, it will find out its hole_startk.
		 */
		if (mem_hole.node_id==-1) {
			resource_t limitk_pri = 0;
			for (i = 0; i < sysconf.nodes; i++) {
				struct dram_base_mask_t d;
				resource_t base_k, limit_k;
				d = get_dram_base_mask(i);
				if (!(d.base & 1)) continue;

				base_k = ((resource_t)(d.base & 0x1fffff00)) <<9;
				if (base_k > 4 *1024 * 1024) break; // don't need to go to check
				if (limitk_pri != base_k) { // we find the hole
					mem_hole.hole_startk = (unsigned)limitk_pri; // must beblow 4G
					mem_hole.node_id = i;
					break; //only one hole
				}

				limit_k = ((resource_t)((d.mask + 0x00000100) & 0x1fffff00)) << 9;
				limitk_pri = limit_k;
			}
		}
		return mem_hole;
}

#endif

#include <cbmem.h>

static void setup_uma_memory(void)
{
#if IS_ENABLED(CONFIG_GFXUMA)
	uint32_t topmem = (uint32_t) bsp_topmem();
	uma_memory_size = get_uma_memory_size(topmem);
	uma_memory_base = topmem - uma_memory_size;	/* TOP_MEM1 */
	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
		    __func__, uma_memory_size, uma_memory_base);
#endif
}

static void amdfam10_domain_set_resources(struct device *dev)
{
	unsigned long mmio_basek;
	u32 pci_tolm;
	int i, idx;
	struct bus *link;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
	u32 reset_memhole = 1;
#endif

	pci_tolm = 0xffffffffUL;
	for (link = dev->link_list; link; link = link->next) {
		pci_tolm = my_find_pci_tolm(link, pci_tolm);
	}

	// FIXME handle interleaved nodes. If you fix this here, please fix
	// amdk8, too.
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

	// FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M
	// MMIO hole. If you fix this here, please fix amdk8, too.
	/* Round the mmio hole to 64M */
	mmio_basek &= ~((64*1024) - 1);

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
/* if the hw mem hole is already set in raminit stage, here we will compare
 * mmio_basek and hole_basek. if mmio_basek is bigger that hole_basek and will
 * use hole_basek as mmio_basek and we don't need to reset hole.
 * otherwise We reset the hole to the mmio_basek
 */

	mem_hole = get_hw_mem_hole_info();

	// Use hole_basek as mmio_basek, and we don't need to reset hole anymore
	if ((mem_hole.node_id !=  -1) && (mmio_basek > mem_hole.hole_startk)) {
		mmio_basek = mem_hole.hole_startk;
		reset_memhole = 0;
	}

#endif

	idx = 0x10;
	for (i = 0; i < sysconf.nodes; i++) {
		struct dram_base_mask_t d;
		resource_t basek, limitk, sizek; // 4 1T
		d = get_dram_base_mask(i);

		if (!(d.mask & 1)) continue;
		basek = ((resource_t)(d.base & 0x1fffff00)) << 9; // could overflow, we may lost 6 bit here
		limitk = ((resource_t)((d.mask + 0x00000100) & 0x1fffff00)) << 9;
		sizek = limitk - basek;

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < ((8*64)+(8*16))) && (sizek > ((8*64)+(16*16)))) {
			ram_resource(dev, (idx | i), basek, ((8*64)+(8*16)) - basek);
			idx += 0x10;
			basek = (8*64)+(16*16);
			sizek = limitk - ((8*64)+(16*16));

		}

		/* split the region to accommodate pci memory space */
		if ((basek < 4*1024*1024) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek > 0) {
					ram_resource(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
				}
				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4*1024*1024) {
				sizek = 0;
			} else {
				basek = 4*1024*1024;
				sizek -= (4*1024*1024 - mmio_basek);
			}
		}

		ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
		printk(BIOS_DEBUG, "%d: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
			     i, mmio_basek, basek, limitk);
	}

#if IS_ENABLED(CONFIG_GFXUMA)
	uma_resource(dev, 7, uma_memory_base >> 10, uma_memory_size >> 10);
#endif

	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			assign_resources(link);
		}
	}
}

static void amdfam10_domain_scan_bus(struct device *dev)
{
	u32 reg;
	int i;
	struct bus *link;
	/* Unmap all of the HT chains */
	for (reg = 0xe0; reg <= 0xec; reg += 4) {
		f1_write_config32(reg, 0);
	}

	for (link = dev->link_list; link; link = link->next) {
		link->secondary = dev->bus->subordinate;
		pci_scan_bus(link, PCI_DEVFN(CONFIG_CDB, 0), 0xff);
		dev->bus->subordinate = link->subordinate;
	}

	/* Tune the hypertransport transaction for best performance.
	 * Including enabling relaxed ordering if it is safe.
	 */
	get_fx_devs();
	for (i = 0; i < fx_devs; i++) {
		struct device *f0_dev;
		f0_dev = __f0_dev[i];
		if (f0_dev && f0_dev->enabled) {
			u32 httc;
			httc = pci_read_config32(f0_dev, HT_TRANSACTION_CONTROL);
			httc &= ~HTTC_RSP_PASS_PW;
			if (!dev->link_list->disable_relaxed_ordering) {
				httc |= HTTC_RSP_PASS_PW;
			}
			printk(BIOS_SPEW, "%s passpw: %s\n",
				dev_path(dev),
				(!dev->link_list->disable_relaxed_ordering)?
				"enabled":"disabled");
			pci_write_config32(f0_dev, HT_TRANSACTION_CONTROL, httc);
		}
	}
}

#if IS_ENABLED(CONFIG_GENERATE_SMBIOS_TABLES)
static int amdfam10_get_smbios_data16(int *count, int handle,
				      unsigned long *current)
{
	struct amdmct_memory_info *mem_info;
	mem_info = cbmem_find(CBMEM_ID_AMDMCT_MEMINFO);
	if (mem_info == NULL)
		return 0;	/* can't find amdmct information in cbmem */

	struct device *dev = get_node_pci(0, 0);
	struct northbridge_amd_amdfam10_config *config = dev->chip_info;

	int node;
	int slot;

	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	int len = sizeof(struct smbios_type16);

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = handle;
	t->length = len - 2;
	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->memory_error_correction = MEMORY_ARRAY_ECC_NONE;
	if ((mem_info->ecc_enabled)
		&& (mem_info->mct_stat.GStatus & (1 << GSB_ECCDIMMs))
		&& !(mem_info->mct_stat.GStatus & (1 << GSB_DramECCDis)))
		/* Single-bit ECC enabled */
		t->memory_error_correction = MEMORY_ARRAY_ECC_SINGLE_BIT;
	t->maximum_capacity = config->maximum_memory_capacity / 1024; /* Convert to kilobytes */
	t->memory_error_information_handle = 0xFFFE;	/* no error information handle available */

	t->number_of_memory_devices = 0;
	/* Check all nodes for installed DIMMs */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++)
		/* Check all slots for installed DIMMs */
		for (slot = 0; slot < MAX_DIMMS_SUPPORTED; slot++)
			if (mem_info->dct_stat[node].DIMMPresent & (1 << slot))
				/* Found an installed DIMM; increment count */
				t->number_of_memory_devices++;

	*current += len;
	*count += 1;
	return len;
}

static uint16_t amdmct_mct_speed_enum_to_mhz(uint8_t speed)
{
	if (is_fam15h()) {
		if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
			switch (speed) {
				case 0x4:
					return 333;
				case 0x6:
					return 400;
				case 0xa:
					return 533;
				case 0xe:
					return 667;
				case 0x12:
					return 800;
				case 0x16:
					return 933;
				default:
					return 0;
			}
		} else {
			return 0;
		}
	} else {
		if (IS_ENABLED(CONFIG_DIMM_DDR2)) {
			switch (speed) {
				case 1:
					return 200;
				case 2:
					return 266;
				case 3:
					return 333;
				case 4:
					return 400;
				case 5:
					return 533;
				default:
					return 0;
			}
		} else if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
			switch (speed) {
				case 3:
					return 333;
				case 4:
					return 400;
				case 5:
					return 533;
				case 6:
					return 667;
				case 7:
					return 800;
				default:
					return 0;
			}
		} else {
			return 0;
		}
	}
}

static int amdfam10_get_smbios_data17(int *count, int handle, int parent_handle,
				      unsigned long *current)
{
	struct amdmct_memory_info *mem_info;
	mem_info = cbmem_find(CBMEM_ID_AMDMCT_MEMINFO);
	if (mem_info == NULL)
		return 0;       /* can't find amdmct information in cbmem */

	int single_len;
	int len = 0;
	int node;
	int slot;

	/* Check all nodes for installed DIMMs */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		/* Get configured RAM bus speed */
		uint16_t speed;
		speed = amdmct_mct_speed_enum_to_mhz(mem_info->dct_stat[node].Speed);

		/* Get maximum RAM bus speed */
		uint16_t max_speed;
		max_speed = amdmct_mct_speed_enum_to_mhz(mem_info->dct_stat[node].DIMMAutoSpeed);

		/* Check all slots for installed DIMMs */
		for (slot = 0; slot < MAX_DIMMS_SUPPORTED; slot++) {
			if (mem_info->dct_stat[node].DIMMPresent & (1 << slot)) {
				/* Found an installed DIMM;  populate tables */
				struct smbios_type17 *t = (struct smbios_type17 *)*current;
				char string_buffer[256];

				/* Initialize structure */
				memset(t, 0, sizeof(struct smbios_type17));

				/* Calculate the total module size in bytes:
				* Primary data width * 2^(#rows) * 2^(#cols) * #banks * #ranks
				*/
				uint8_t width, rows, cols, banks, ranks;
				uint64_t chip_size;
				uint32_t chip_width;
				rows = mem_info->dct_stat[node].DimmRows[slot];
				cols = mem_info->dct_stat[node].DimmCols[slot];
				ranks = mem_info->dct_stat[node].DimmRanks[slot];
				banks = mem_info->dct_stat[node].DimmBanks[slot];
#if IS_ENABLED(CONFIG_DIMM_DDR3)
				chip_size = mem_info->dct_stat[node].DimmChipSize[slot];
				chip_width = mem_info->dct_stat[node].DimmChipWidth[slot];
#else
				chip_size = 0;
				chip_width = 0;
#endif
				uint64_t dimm_size_bytes;
				if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
					width = mem_info->dct_stat[node].DimmWidth[slot];
					dimm_size_bytes = ((width / chip_width) * chip_size * ranks) / 8;
				} else {
					width = 8;
					dimm_size_bytes = width * (1ULL << rows) * (1ULL << cols) * banks * ranks;
				}

				memset(t, 0, sizeof(struct smbios_type17));
				t->type = SMBIOS_MEMORY_DEVICE;
				t->handle = handle;
				t->phys_memory_array_handle = parent_handle;
				t->length = sizeof(struct smbios_type17) - 2;
				if (dimm_size_bytes > 0x800000000) {
					t->size = 0x7FFF;
					t->extended_size = dimm_size_bytes >> 16;
				} else {
					t->size = dimm_size_bytes / (1024*1024);
					t->size &= (~0x8000);	/* size specified in megabytes */
				}
				t->total_width = t->data_width = 64;
				if (mem_info->dct_stat[node].DimmECCPresent & (1 << slot))
					t->total_width += 8;
				t->attributes = 0;
				t->attributes |= ranks & 0xf;	/* rank number is stored in the lowest 4 bits of the attributes field */
				t->form_factor = MEMORY_FORMFACTOR_DIMM;
				if (mem_info->dct_stat[node].Dual_Node_Package) {
					snprintf(string_buffer, sizeof(string_buffer), "NODE %d DIMM_%s%d", node >> 1,
						(mem_info->dct_stat[node].Internal_Node_ID)?((slot & 0x1)?"D":"C"):((slot & 0x1)?"B":"A"), (slot >> 1) + 1);
				} else {
					snprintf(string_buffer, sizeof(string_buffer), "NODE %d DIMM_%s%d", node, (slot & 0x1)?"B":"A", (slot >> 1) + 1);
				}
				t->device_locator = smbios_add_string(t->eos, string_buffer);
				if (IS_ENABLED(CONFIG_DIMM_DDR2))
					t->memory_type = MEMORY_TYPE_DDR2;
				else if (IS_ENABLED(CONFIG_DIMM_DDR3))
					t->memory_type = MEMORY_TYPE_DDR3;
				t->type_detail = MEMORY_TYPE_DETAIL_SYNCHRONOUS;
				if (mem_info->dct_stat[node].DimmRegistered[slot])
					t->type_detail |= MEMORY_TYPE_DETAIL_REGISTERED;
				else
					t->type_detail |= MEMORY_TYPE_DETAIL_UNBUFFERED;
				t->speed = max_speed;
				t->clock_speed = speed;
				smbios_fill_dimm_manufacturer_from_id(mem_info->dct_stat[node].DimmManufacturerID[slot], t);
				t->part_number = smbios_add_string(t->eos, mem_info->dct_stat[node].DimmPartNumber[slot]);
				if (mem_info->dct_stat[node].DimmSerialNumber[slot] == 0) {
					t->serial_number = smbios_add_string(t->eos, "None");
				} else {
					snprintf(string_buffer, sizeof(string_buffer), "%08X", mem_info->dct_stat[node].DimmSerialNumber[slot]);
					t->serial_number = smbios_add_string(t->eos, string_buffer);
				}
				if (IS_ENABLED(CONFIG_DIMM_DDR2)) {
					/* JEDEC specifies 1.8V only, so assume that the memory is configured for 1.8V */
					t->minimum_voltage = 1800;
					t->maximum_voltage = 1800;
					t->configured_voltage = 1800;
				} else if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
#if IS_ENABLED(CONFIG_DIMM_DDR3)
					/* Find the maximum and minimum supported voltages */
					uint8_t supported_voltages = mem_info->dct_stat[node].DimmSupportedVoltages[slot];
					uint8_t configured_voltage = mem_info->dct_stat[node].DimmConfiguredVoltage[slot];

					if (supported_voltages & 0x8)
						t->minimum_voltage = 1150;
					else if (supported_voltages & 0x4)
						t->minimum_voltage = 1250;
					else if (supported_voltages & 0x2)
						t->minimum_voltage = 1350;
					else if (supported_voltages & 0x1)
						t->minimum_voltage = 1500;

					if (supported_voltages & 0x1)
						t->maximum_voltage = 1500;
					else if (supported_voltages & 0x2)
						t->maximum_voltage = 1350;
					else if (supported_voltages & 0x4)
						t->maximum_voltage = 1250;
					else if (supported_voltages & 0x8)
						t->maximum_voltage = 1150;

					if (configured_voltage & 0x8)
						t->configured_voltage = 1150;
					else if (configured_voltage & 0x4)
						t->configured_voltage = 1250;
					else if (configured_voltage & 0x2)
						t->configured_voltage = 1350;
					else if (configured_voltage & 0x1)
						t->configured_voltage = 1500;
#endif
				}
				t->memory_error_information_handle = 0xFFFE;	/* no error information handle available */
				single_len = t->length + smbios_string_table_len(t->eos);
				len += single_len;
				*current += single_len;
				handle++;
				*count += 1;
			}
		}
	}

	return len;
}

static int amdfam10_get_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len;
	int count = 0;
	len = amdfam10_get_smbios_data16(&count, *handle, current);
	len += amdfam10_get_smbios_data17(&count, *handle + 1, *handle, current);
	*handle += count;
	return len;
}
#endif

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static const char *amdfam10_domain_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	return NULL;
}
#endif

static struct device_operations pci_domain_ops = {
	.read_resources	  = amdfam10_domain_read_resources,
	.set_resources	  = amdfam10_domain_set_resources,
	.enable_resources = NULL,
	.init		  = NULL,
	.scan_bus	  = amdfam10_domain_scan_bus,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name	  = amdfam10_domain_acpi_name,
#endif
#if IS_ENABLED(CONFIG_GENERATE_SMBIOS_TABLES)
	.get_smbios_data  = amdfam10_get_smbios_data,
#endif
};

static void sysconf_init(struct device *dev) // first node
{
	sysconf.sblk = (pci_read_config32(dev, 0x64)>>8) & 7; // don't forget sublink1
	sysconf.segbit = 0;
	sysconf.ht_c_num = 0;

	unsigned ht_c_index;

	for (ht_c_index = 0; ht_c_index < 32; ht_c_index++) {
		sysconf.ht_c_conf_bus[ht_c_index] = 0;
	}

	sysconf.nodes = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1;
#if CONFIG_MAX_PHYSICAL_CPUS > 8
	sysconf.nodes += (((pci_read_config32(dev, 0x160)>>4) & 7)<<3);
#endif

	sysconf.enabled_apic_ext_id = 0;
	sysconf.lift_bsp_apicid = 0;

	/* Find the bootstrap processors apicid */
	sysconf.bsp_apicid = lapicid();
	sysconf.apicid_offset = sysconf.bsp_apicid;

#if IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID)
	if (pci_read_config32(dev, 0x68) & (HTTC_APIC_EXT_ID|HTTC_APIC_EXT_BRD_CST))
	{
		sysconf.enabled_apic_ext_id = 1;
	}
	#if (CONFIG_APIC_ID_OFFSET > 0)
	if (sysconf.enabled_apic_ext_id) {
		if (sysconf.bsp_apicid == 0) {
			/* bsp apic id is not changed */
			sysconf.apicid_offset = CONFIG_APIC_ID_OFFSET;
		} else {
			sysconf.lift_bsp_apicid = 1;
		}
	}
	#endif
#endif
}

static void add_more_links(struct device *dev, unsigned total_links)
{
	struct bus *link, *last = NULL;
	int link_num = -1;

	for (link = dev->link_list; link; link = link->next) {
		if (link_num < link->link_num)
			link_num = link->link_num;
		last = link;
	}

	if (last) {
		int links = total_links - (link_num + 1);
		if (links > 0) {
			link = malloc(links*sizeof(*link));
			if (!link)
				die("Couldn't allocate more links!\n");
			memset(link, 0, links*sizeof(*link));
			last->next = link;
		}
	} else {
		link = malloc(total_links*sizeof(*link));
		memset(link, 0, total_links*sizeof(*link));
		dev->link_list = link;
	}

	for (link_num = link_num + 1; link_num < total_links; link_num++) {
		link->link_num = link_num;
		link->dev = dev;
		link->next = link + 1;
		last = link;
		link = link->next;
	}
	last->next = NULL;
}

static void remap_bsp_lapic(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct device *cpu;
	u32 bsp_lapic_id = lapicid();

	if (bsp_lapic_id) {
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = 0;
		cpu = find_dev_path(cpu_bus, &cpu_path);
		if (cpu)
			cpu->path.apic.apic_id = bsp_lapic_id;
	}
}

static void cpu_bus_scan(struct device *dev)
{
	struct bus *cpu_bus;
	struct device *dev_mc;
#if CONFIG_CBB
	struct device *pci_domain;
#endif
	int nvram = 0;
	int i,j;
	int nodes;
	unsigned nb_cfg_54;
	unsigned siblings;
	int cores_found;
	int disable_siblings;
	uint8_t disable_cu_siblings = 0;
	unsigned ApicIdCoreIdSize;

	nb_cfg_54 = 0;
	ApicIdCoreIdSize = (cpuid_ecx(0x80000008)>>12 & 0xf);
	if (ApicIdCoreIdSize) {
		siblings = (1<<ApicIdCoreIdSize)-1;
	} else {
		siblings = 3; //quad core
	}

	disable_siblings = !CONFIG_LOGICAL_CPUS;
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	get_option(&disable_siblings, "multi_core");
#endif

	// How can I get the nb_cfg_54 of every node's nb_cfg_54 in bsp???
	nb_cfg_54 = read_nb_cfg_54();

#if CONFIG_CBB
	dev_mc = dev_find_slot(0, PCI_DEVFN(CONFIG_CDB, 0)); //0x00
	if (dev_mc && dev_mc->bus) {
		printk(BIOS_DEBUG, "%s found", dev_path(dev_mc));
		pci_domain = dev_mc->bus->dev;
		if (pci_domain && (pci_domain->path.type == DEVICE_PATH_DOMAIN)) {
			printk(BIOS_DEBUG, "\n%s move to ",dev_path(dev_mc));
			dev_mc->bus->secondary = CONFIG_CBB; // move to 0xff
			printk(BIOS_DEBUG, "%s",dev_path(dev_mc));

		} else {
			printk(BIOS_DEBUG, " but it is not under pci_domain directly ");
		}
		printk(BIOS_DEBUG, "\n");
	}
	dev_mc = dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB, 0));
	if (!dev_mc) {
		dev_mc = dev_find_slot(0, PCI_DEVFN(0x18, 0));
		if (dev_mc && dev_mc->bus) {
			printk(BIOS_DEBUG, "%s found\n", dev_path(dev_mc));
			pci_domain = dev_mc->bus->dev;
			if (pci_domain && (pci_domain->path.type == DEVICE_PATH_DOMAIN)) {
				if ((pci_domain->link_list) && (pci_domain->link_list->children == dev_mc)) {
					printk(BIOS_DEBUG, "%s move to ",dev_path(dev_mc));
					dev_mc->bus->secondary = CONFIG_CBB; // move to 0xff
					printk(BIOS_DEBUG, "%s\n",dev_path(dev_mc));
					while (dev_mc) {
						printk(BIOS_DEBUG, "%s move to ",dev_path(dev_mc));
						dev_mc->path.pci.devfn -= PCI_DEVFN(0x18,0);
						printk(BIOS_DEBUG, "%s\n",dev_path(dev_mc));
						dev_mc = dev_mc->sibling;
					}
				}
			}
		}
	}

#endif

	dev_mc = dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB, 0));
	if (!dev_mc) {
		printk(BIOS_ERR, "%02x:%02x.0 not found", CONFIG_CBB, CONFIG_CDB);
		die("");
	}

	sysconf_init(dev_mc);

	nodes = sysconf.nodes;

#if CONFIG_CBB && (NODE_NUMS > 32)
	if (nodes > 32) { // need to put node 32 to node 63 to bus 0xfe
		if (pci_domain->link_list && !pci_domain->link_list->next) {
			struct bus *new_link = new_link(pci_domain);
			pci_domain->link_list->next = new_link;
			new_link->link_num = 1;
			new_link->dev = pci_domain;
			new_link->children = 0;
			printk(BIOS_DEBUG, "%s links now 2\n", dev_path(pci_domain));
		}
		pci_domain->link_list->next->secondary = CONFIG_CBB - 1;
	}
#endif
	/* Find which cpus are present */
	cpu_bus = dev->link_list;

	/* Always use the devicetree node with lapic_id 0 for BSP. */
	remap_bsp_lapic(cpu_bus);

	if (get_option(&nvram, "compute_unit_siblings") == CB_SUCCESS)
		disable_cu_siblings = !!nvram;

	if (disable_cu_siblings)
		printk(BIOS_DEBUG, "Disabling siblings on each compute unit as requested\n");

	for (i = 0; i < nodes; i++) {
		struct device *cdb_dev;
		unsigned busn, devn;
		struct bus *pbus;

		uint8_t fam15h = 0;
		uint8_t rev_gte_d = 0;
		uint8_t dual_node = 0;
		uint32_t f3xe8;
		uint32_t family;
		uint32_t model;

		busn = CONFIG_CBB;
		devn = CONFIG_CDB+i;
		pbus = dev_mc->bus;
#if CONFIG_CBB && (NODE_NUMS > 32)
		if (i >= 32) {
			busn--;
			devn-=32;
			pbus = pci_domain->link_list->next;
		}
#endif

		/* Find the cpu's pci device */
		cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 0));
		if (!cdb_dev) {
			/* If I am probing things in a weird order
			 * ensure all of the cpu's pci devices are found.
			 */
			int fn;
			for (fn = 0; fn <= 5; fn++) { //FBDIMM?
				cdb_dev = pci_probe_dev(NULL, pbus,
					PCI_DEVFN(devn, fn));
			}
		}


		/* Ok, We need to set the links for that device.
		 * otherwise the device under it will not be scanned
		 */
		cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 0));
		if (cdb_dev)
			add_more_links(cdb_dev, 4);

		cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 4));
		if (cdb_dev)
			add_more_links(cdb_dev, 4);

		f3xe8 = pci_read_config32(get_node_pci(0, 3), 0xe8);

		family = model = cpuid_eax(0x80000001);
		model = ((model & 0xf0000) >> 12) | ((model & 0xf0) >> 4);

		if (is_fam15h()) {
			/* Family 15h or later */
			fam15h = 1;
			nb_cfg_54 = 1;
		}

		if ((model >= 0x8) || fam15h)
			/* Revision D or later */
			rev_gte_d = 1;

		if (rev_gte_d)
			/* Check for dual node capability */
			if (f3xe8 & 0x20000000)
				dual_node = 1;

		cores_found = 0; // one core
		if (fam15h)
			cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 5));
		else
			cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 3));
		int enable_node = cdb_dev && cdb_dev->enabled;
		if (enable_node) {
			if (fam15h) {
				cores_found = pci_read_config32(cdb_dev, 0x84) & 0xff;
			} else {
				j = pci_read_config32(cdb_dev, 0xe8);
				cores_found = (j >> 12) & 3; // dev is func 3
				if (siblings > 3)
					cores_found |= (j >> 13) & 4;
			}
			printk(BIOS_DEBUG, "  %s siblings=%d\n", dev_path(cdb_dev), cores_found);
		}

		if (siblings > cores_found)
			siblings = cores_found;

		u32 jj;
		if (disable_siblings) {
			jj = 0;
		} else
		{
			jj = cores_found;
		}

		for (j = 0; j <=jj; j++) {
			u32 apic_id;

			if (dual_node) {
				apic_id = 0;
				if (fam15h) {
					apic_id |= ((i >> 1) & 0x3) << 5;			/* Node ID */
					apic_id |= ((i & 0x1) * (siblings + 1)) + j;		/* Core ID */
				} else {
					if (nb_cfg_54) {
						apic_id |= ((i >> 1) & 0x3) << 4;			/* Node ID */
						apic_id |= ((i & 0x1) * (siblings + 1)) + j;		/* Core ID */
					} else {
						apic_id |= i & 0x3;					/* Node ID */
						apic_id |= (((i & 0x1) * (siblings + 1)) + j) << 4;	/* Core ID */
					}
				}
			} else {
				if (fam15h) {
					apic_id = 0;
					apic_id |= (i & 0x7) << 4;	/* Node ID */
					apic_id |= j & 0xf;		/* Core ID */
				} else {
					apic_id = i * (nb_cfg_54?(siblings+1):1) + j * (nb_cfg_54?1:64); // ?
				}
			}

#if IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID) && (CONFIG_APIC_ID_OFFSET > 0)
			if (sysconf.enabled_apic_ext_id) {
				if (apic_id != 0 || sysconf.lift_bsp_apicid) {
					apic_id += sysconf.apicid_offset;
				}
			}
#endif
			if (disable_cu_siblings && (j & 0x1))
				continue;

			struct device *cpu = add_cpu_device(cpu_bus, apic_id, enable_node);
			if (cpu)
				amd_cpu_topology(cpu, i, j);
		}
	}
}

static void detect_and_enable_probe_filter(struct device *dev)
{
	uint32_t dword;

	uint8_t nvram;
	uint8_t enable_probe_filter;

	/* Check to see if the probe filter is allowed */
	enable_probe_filter = 1;
	if (get_option(&nvram, "probe_filter") == CB_SUCCESS)
		enable_probe_filter = !!nvram;

	if (!enable_probe_filter)
		return;

	uint8_t fam15h = 0;
	uint8_t rev_gte_d = 0;
	unsigned nb_cfg_54;
	uint32_t family;
	uint32_t model;

	family = model = cpuid_eax(0x80000001);
	model = ((model & 0xf0000) >> 12) | ((model & 0xf0) >> 4);

	if (is_fam15h()) {
		/* Family 15h or later */
		fam15h = 1;
		nb_cfg_54 = 1;
	}

	if ((model >= 0x8) || fam15h)
		/* Revision D or later */
		rev_gte_d = 1;

	if (rev_gte_d && (sysconf.nodes > 1)) {
		/* Enable the probe filter */
		uint8_t i;
		uint8_t pfmode = 0x0;

		uint32_t f3x58[MAX_NODES_SUPPORTED];
		uint32_t f3x5c[MAX_NODES_SUPPORTED];

		printk(BIOS_DEBUG, "Enabling probe filter\n");

		/* Disable L3 and DRAM scrubbers and configure system for probe filter support */
		for (i = 0; i < sysconf.nodes; i++) {
			struct device *f2x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 2));
			struct device *f3x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));

			f3x58[i] = pci_read_config32(f3x_dev, 0x58);
			f3x5c[i] = pci_read_config32(f3x_dev, 0x5c);
			pci_write_config32(f3x_dev, 0x58, f3x58[i] & ~((0x1f << 24) | 0x1f));
			pci_write_config32(f3x_dev, 0x5c, f3x5c[i] & ~0x1);

			dword = pci_read_config32(f2x_dev, 0x1b0);
			dword &= ~(0x7 << 8);	/* CohPrefPrbLmt = 0x0 */
			pci_write_config32(f2x_dev, 0x1b0, dword);

			msr_t msr = rdmsr_amd(BU_CFG2_MSR);
			msr.hi |= 1 << (42 - 32);
			wrmsr_amd(BU_CFG2_MSR, msr);

			if (is_fam15h()) {
				uint8_t subcache_size = 0x0;
				uint8_t pref_so_repl = 0x0;
				uint32_t f3x1c4 = pci_read_config32(f3x_dev, 0x1c4);
				if ((f3x1c4 & 0xffff) == 0xcccc) {
					subcache_size = 0x1;
					pref_so_repl = 0x2;
					pfmode = 0x3;
				} else {
					pfmode = 0x2;
				}

				dword = pci_read_config32(f3x_dev, 0x1d4);
				dword |= 0x1 << 29;	/* PFLoIndexHashEn = 0x1 */
				dword &= ~(0x3 << 20);	/* PFPreferredSORepl = pref_so_repl */
				dword |= (pref_so_repl & 0x3) << 20;
				dword |= 0x1 << 17;	/* PFWayHashEn = 0x1 */
				dword |= 0xf << 12;	/* PFSubCacheEn = 0xf */
				dword &= ~(0x3 << 10);	/* PFSubCacheSize3 = subcache_size */
				dword |= (subcache_size & 0x3) << 10;
				dword &= ~(0x3 << 8);	/* PFSubCacheSize2 = subcache_size */
				dword |= (subcache_size & 0x3) << 8;
				dword &= ~(0x3 << 6);	/* PFSubCacheSize1 = subcache_size */
				dword |= (subcache_size & 0x3) << 6;
				dword &= ~(0x3 << 4);	/* PFSubCacheSize0 = subcache_size */
				dword |= (subcache_size & 0x3) << 4;
				dword &= ~(0x3 << 2);	/* PFWayNum = 0x2 */
				dword |= 0x2 << 2;
				pci_write_config32(f3x_dev, 0x1d4, dword);
			} else {
				pfmode = 0x2;

				dword = pci_read_config32(f3x_dev, 0x1d4);
				dword |= 0x1 << 29;	/* PFLoIndexHashEn = 0x1 */
				dword &= ~(0x3 << 20);	/* PFPreferredSORepl = 0x2 */
				dword |= 0x2 << 20;
				dword |= 0xf << 12;	/* PFSubCacheEn = 0xf */
				dword &= ~(0x3 << 10);	/* PFSubCacheSize3 = 0x0 */
				dword &= ~(0x3 << 8);	/* PFSubCacheSize2 = 0x0 */
				dword &= ~(0x3 << 6);	/* PFSubCacheSize1 = 0x0 */
				dword &= ~(0x3 << 4);	/* PFSubCacheSize0 = 0x0 */
				dword &= ~(0x3 << 2);	/* PFWayNum = 0x2 */
				dword |= 0x2 << 2;
				pci_write_config32(f3x_dev, 0x1d4, dword);
			}
		}

		udelay(40);

		disable_cache();
		wbinvd();

		/* Enable probe filter */
		for (i = 0; i < sysconf.nodes; i++) {
			struct device *f3x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));

			dword = pci_read_config32(f3x_dev, 0x1c4);
			dword |= (0x1 << 31);	/* L3TagInit = 1 */
			pci_write_config32(f3x_dev, 0x1c4, dword);
			do {
			} while (pci_read_config32(f3x_dev, 0x1c4) & (0x1 << 31));

			dword = pci_read_config32(f3x_dev, 0x1d4);
			dword &= ~0x3;		/* PFMode = pfmode */
			dword |= pfmode & 0x3;
			pci_write_config32(f3x_dev, 0x1d4, dword);
			do {
			} while (!(pci_read_config32(f3x_dev, 0x1d4) & (0x1 << 19)));
		}

		if (is_fam15h()) {
			printk(BIOS_DEBUG, "Enabling ATM mode\n");

			/* Enable ATM mode */
			for (i = 0; i < sysconf.nodes; i++) {
				struct device *f0x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 0));
				struct device *f3x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));

				dword = pci_read_config32(f0x_dev, 0x68);
				dword |= (0x1 << 12);	/* ATMModeEn = 1 */
				pci_write_config32(f0x_dev, 0x68, dword);

				dword = pci_read_config32(f3x_dev, 0x1b8);
				dword |= (0x1 << 27);	/* L3ATMModeEn = 1 */
				pci_write_config32(f3x_dev, 0x1b8, dword);
			}
		}

		enable_cache();

		/* Reenable L3 and DRAM scrubbers */
		for (i = 0; i < sysconf.nodes; i++) {
			struct device *f3x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));

			pci_write_config32(f3x_dev, 0x58, f3x58[i]);
			pci_write_config32(f3x_dev, 0x5c, f3x5c[i]);
		}

	}
}

static void detect_and_enable_cache_partitioning(struct device *dev)
{
	uint8_t i;
	uint32_t dword;

	uint8_t nvram;
	uint8_t enable_l3_cache_partitioning;

	/* Check to see if cache partitioning is allowed */
	enable_l3_cache_partitioning = 0;
	if (get_option(&nvram, "l3_cache_partitioning") == CB_SUCCESS)
		enable_l3_cache_partitioning = !!nvram;

	if (!enable_l3_cache_partitioning)
		return;

	if (is_fam15h()) {
		printk(BIOS_DEBUG, "Enabling L3 cache partitioning\n");

		uint32_t f5x80;
		uint8_t cu_enabled;
		uint8_t compute_unit_count = 0;

		uint32_t f3xe8;
		uint8_t dual_node = 0;

		for (i = 0; i < sysconf.nodes; i++) {
			struct device *f3x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));
			struct device *f4x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 4));
			struct device *f5x_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 5));

			f3xe8 = pci_read_config32(f3x_dev, 0xe8);

			/* Check for dual node capability */
			if (f3xe8 & 0x20000000)
				dual_node = 1;

			/* Determine the number of active compute units on this node */
			f5x80 = pci_read_config32(f5x_dev, 0x80);
			cu_enabled = f5x80 & 0xf;
			if (cu_enabled == 0x1)
				compute_unit_count = 1;
			if (cu_enabled == 0x3)
				compute_unit_count = 2;
			if (cu_enabled == 0x7)
				compute_unit_count = 3;
			if (cu_enabled == 0xf)
				compute_unit_count = 4;

			/* Disable BAN mode */
			dword = pci_read_config32(f3x_dev, 0x1b8);
			dword &= ~(0x7 << 19);	/* L3BanMode = 0x0 */
			pci_write_config32(f3x_dev, 0x1b8, dword);

			/* Set up cache mapping */
			dword = pci_read_config32(f4x_dev, 0x1d4);
			if (compute_unit_count == 1) {
				dword |= 0xf;		/* ComputeUnit0SubCacheEn = 0xf */
			}
			if (compute_unit_count == 2) {
				dword &= ~(0xf << 4);	/* ComputeUnit1SubCacheEn = 0xc */
				dword |= (0xc << 4);
				dword &= ~0xf;		/* ComputeUnit0SubCacheEn = 0x3 */
				dword |= 0x3;
			}
			if (compute_unit_count == 3) {
				dword &= ~(0xf << 8);	/* ComputeUnit2SubCacheEn = 0x8 */
				dword |= (0x8 << 8);
				dword &= ~(0xf << 4);	/* ComputeUnit1SubCacheEn = 0x4 */
				dword |= (0x4 << 4);
				dword &= ~0xf;		/* ComputeUnit0SubCacheEn = 0x3 */
				dword |= 0x3;
			}
			if (compute_unit_count == 4) {
				dword &= ~(0xf << 12);	/* ComputeUnit3SubCacheEn = 0x8 */
				dword |= (0x8 << 12);
				dword &= ~(0xf << 8);	/* ComputeUnit2SubCacheEn = 0x4 */
				dword |= (0x4 << 8);
				dword &= ~(0xf << 4);	/* ComputeUnit1SubCacheEn = 0x2 */
				dword |= (0x2 << 4);
				dword &= ~0xf;		/* ComputeUnit0SubCacheEn = 0x1 */
				dword |= 0x1;
			}
			pci_write_config32(f4x_dev, 0x1d4, dword);

			/* Enable cache partitioning */
			pci_write_config32(f4x_dev, 0x1d4, dword);
			if (compute_unit_count == 1) {
				dword &= ~(0xf << 26);	/* MaskUpdateForComputeUnit = 0x1 */
				dword |= (0x1 << 26);
			} else if (compute_unit_count == 2) {
				dword &= ~(0xf << 26);	/* MaskUpdateForComputeUnit = 0x3 */
				dword |= (0x3 << 26);
			} else if (compute_unit_count == 3) {
				dword &= ~(0xf << 26);	/* MaskUpdateForComputeUnit = 0x7 */
				dword |= (0x7 << 26);
			} else if (compute_unit_count == 4) {
				dword |= (0xf << 26);	/* MaskUpdateForComputeUnit = 0xf */
			}
			pci_write_config32(f4x_dev, 0x1d4, dword);
		}
	}
}

static void cpu_bus_init(struct device *dev)
{
	detect_and_enable_probe_filter(dev);
	detect_and_enable_cache_partitioning(dev);
	initialize_cpus(dev->link_list);
#if IS_ENABLED(CONFIG_AMD_SB_CIMX)
	sb_After_Pci_Init();
	sb_Mid_Post_Init();
#endif
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = DEVICE_NOOP,
	.set_resources	  = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init		  = cpu_bus_init,
	.scan_bus	  = cpu_bus_scan,
};

static void root_complex_enable_dev(struct device *dev)
{
	static int done = 0;

	/* Do not delay UMA setup, as a device on the PCI bus may evaluate
	   the global uma_memory variables already in its enable function. */
	if (!done) {
		setup_bsp_ramtop();
		setup_uma_memory();
		done = 1;
	}

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void root_complex_finalize(void *chip_info) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME) && IS_ENABLED(CONFIG_DIMM_DDR3)
	save_mct_information_to_nvram();
#endif
}

struct chip_operations northbridge_amd_amdfam10_root_complex_ops = {
	CHIP_NAME("AMD Family 10h/15h Root Complex")
	.enable_dev = root_complex_enable_dev,
	.final = root_complex_finalize,
};
