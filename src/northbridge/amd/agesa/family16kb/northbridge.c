/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <lib.h>
#include <cpu/cpu.h>
#include <cbmem.h>

#include <cpu/x86/lapic.h>
#include <cpu/amd/mtrr.h>

#include <Porting.h>
#include <AGESA.h>
#include <Options.h>
#include <Topology.h>
#include <cpu/amd/amdfam16.h>
#include <cpuRegisters.h>
#include "agesawrapper.h"
#include "northbridge.h"

#define MAX_NODE_NUMS (MAX_NODES * MAX_DIES)

#if (defined CONFIG_EXT_CONF_SUPPORT) && CONFIG_EXT_CONF_SUPPORT == 1
#error CONFIG_EXT_CONF_SUPPORT == 1 not support anymore!
#endif

typedef struct dram_base_mask {
	u32 base; //[47:27] at [28:8]
	u32 mask; //[47:27] at [28:8] and enable at bit 0
} dram_base_mask_t;

static unsigned node_nums;
static unsigned sblink;
static device_t __f0_dev[MAX_NODE_NUMS];
static device_t __f1_dev[MAX_NODE_NUMS];
static device_t __f2_dev[MAX_NODE_NUMS];
static device_t __f4_dev[MAX_NODE_NUMS];
static unsigned fx_devs = 0;

static dram_base_mask_t get_dram_base_mask(u32 nodeid)
{
	device_t dev;
	dram_base_mask_t d;
	dev = __f1_dev[0];
	u32 temp;
	temp = pci_read_config32(dev, 0x44 + (nodeid << 3)); //[39:24] at [31:16]
	d.mask = ((temp & 0xfff80000)>>(8+3)); // mask out  DramMask [26:24] too
	temp = pci_read_config32(dev, 0x144 + (nodeid <<3)) & 0xff; //[47:40] at [7:0]
	d.mask |= temp<<21;
	temp = pci_read_config32(dev, 0x40 + (nodeid << 3)); //[39:24] at [31:16]
	d.mask |= (temp & 1); // enable bit
	d.base = ((temp & 0xfff80000)>>(8+3)); // mask out DramBase [26:24) too
	temp = pci_read_config32(dev, 0x140 + (nodeid <<3)) & 0xff; //[47:40] at [7:0]
	d.base |= temp<<21;
	return d;
}

static void set_io_addr_reg(device_t dev, u32 nodeid, u32 linkn, u32 reg,
			u32 io_min, u32 io_max)
{
	u32 i;
	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn<<4) | ((io_max&0xf0)<<(12-4)); //limit
	for (i=0; i<node_nums; i++)
		pci_write_config32(__f1_dev[i], reg+4, tempreg);
	tempreg = 3 /*| ( 3<<4)*/ | ((io_min&0xf0)<<(12-4));	      //base :ISA and VGA ?
#if 0
	// FIXME: can we use VGA reg instead?
	if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
		printk(BIOS_SPEW, "%s, enabling legacy VGA IO forwarding for %s link %s\n",
				__func__, dev_path(dev), link);
		tempreg |= PCI_IO_BASE_VGA_EN;
	}
	if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_NO_ISA) {
		tempreg |= PCI_IO_BASE_NO_ISA;
	}
#endif
	for (i=0; i<node_nums; i++)
		pci_write_config32(__f1_dev[i], reg, tempreg);
}

static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index, u32 mmio_min, u32 mmio_max, u32 nodes)
{
	u32 i;
	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid&0xf) | (linkn<<4) |	 (mmio_max&0xffffff00); //limit
	for (i=0; i<nodes; i++)
		pci_write_config32(__f1_dev[i], reg+4, tempreg);
	tempreg = 3 | (nodeid & 0x30) | (mmio_min&0xffffff00);
	for (i=0; i<node_nums; i++)
		pci_write_config32(__f1_dev[i], reg, tempreg);
}

static device_t get_node_pci(u32 nodeid, u32 fn)
{
#if MAX_NODE_NUMS + CONFIG_CDB >= 32
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
	for (i = 0; i < MAX_NODE_NUMS; i++) {
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
	printk(BIOS_DEBUG, "fx_devs=0x%x\n", fx_devs);
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
	for(i = 0; i < fx_devs; i++) {
		device_t dev;
		dev = __f1_dev[i];
		if (dev && dev->enabled) {
			pci_write_config32(dev, reg, value);
		}
	}
}

static u32 amdfam16_nodeid(device_t dev)
{
#if MAX_NODE_NUMS == 64
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
	/* it will routing
	 * (1)mmio 0xa0000:0xbffff
	 * (2)io   0x3b0:0x3bb, 0x3c0:0x3df
	 */
	f1_write_config32(0xf4, val);

}

/**
 * @return
 *  @retval 2  resoure does not exist, usable
 *  @retval 0  resource exists, not usable
 *  @retval 1  resource exist, resource has been allocated before
 */
static int reg_useable(unsigned reg, device_t goal_dev, unsigned goal_nodeid,
			unsigned goal_link)
{
	struct resource *res;
	unsigned nodeid, link = 0;
	int result;
	res = 0;
	for (nodeid = 0; !res && (nodeid < fx_devs); nodeid++) {
		device_t dev;
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
		if ((goal_link == (link - 1)) &&
			(goal_nodeid == (nodeid - 1)) &&
			(res->flags <= 1)) {
			result = 1;
		}
	}
	return result;
}

static struct resource *amdfam16_find_iopair(device_t dev, unsigned nodeid, unsigned link)
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
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xd8) {
		reg = free_reg; // if no free, the free_reg still be 0
	}

	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));

	return resource;
}

static struct resource *amdfam16_find_mempair(device_t dev, u32 nodeid, u32 link)
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
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xb8) {
		reg = free_reg;
	}

	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));
	return resource;
}

static void amdfam16_link_read_bases(device_t dev, u32 nodeid, u32 link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource = amdfam16_find_iopair(dev, nodeid, link);
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
	resource = amdfam16_find_mempair(dev, nodeid, link);
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
	resource = amdfam16_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
	}

}

static void read_resources(device_t dev)
{
	u32 nodeid;
	struct bus *link;

	nodeid = amdfam16_nodeid(dev);
	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			amdfam16_link_read_bases(dev, nodeid, link->link_num);
		}
	}
}

static void set_resource(device_t dev, struct resource *resource, u32 nodeid)
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
	}
	else if (resource->flags & IORESOURCE_MEM) {
		set_mmio_addr_reg(nodeid, link_num, reg, (resource->index >>24), rbase>>8, rend>>8, node_nums) ;// [39:8]
	}
	resource->flags |= IORESOURCE_STORED;
	sprintf(buf, " <node %x link %x>",
			nodeid, link_num);
	report_resource_stored(dev, resource, buf);
}

/**
 * I tried to reuse the resource allocation code in set_resource()
 * but it is too difficult to deal with the resource allocation magic.
 */

static void create_vga_resource(device_t dev, unsigned nodeid)
{
	struct bus *link;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list; link; link = link->next) {
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if CONFIG_MULTIPLE_VGA_ADAPTERS
			extern device_t vga_pri; // the primary vga device, defined in device.c
			printk(BIOS_DEBUG, "VGA: vga_pri bus num = %d bus range [%d,%d]\n", vga_pri->bus->secondary,
					link->secondary,link->subordinate);
			/* We need to make sure the vga_pri is under the link */
			if((vga_pri->bus->secondary >= link->secondary ) &&
					(vga_pri->bus->secondary <= link->subordinate )
			  )
#endif
				break;
		}
	}

	/* no VGA card installed */
	if (link == NULL)
		return;

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, sblink);
	set_vga_enable_reg(nodeid, sblink);
}

static void set_resources(device_t dev)
{
	unsigned nodeid;
	struct bus *bus;
	struct resource *res;

	/* Find the nodeid */
	nodeid = amdfam16_nodeid(dev);

	create_vga_resource(dev, nodeid); //TODO: do we need this?

	/* Set each resource we have found */
	for (res = dev->resource_list; res; res = res->next) {
		set_resource(dev, res, nodeid);
	}

	for (bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children) {
			assign_resources(bus);
		}
	}
}

static void northbridge_init(struct device *dev)
{
}
#if 0				/* TODO: Check if needed. */
static unsigned scan_chains(device_t dev, unsigned max)
{
	unsigned nodeid;
	struct bus *link;
	device_t io_hub = NULL;
	u32 next_unitid = 0x18;
	nodeid = amdfam16_nodeid(dev);
	if (nodeid == 0) {
		for (link = dev->link_list; link; link = link->next) {
			//if (link->link_num == sblink) { /* devicetree put IO Hub on link_lsit[sblink] */
			if (link->link_num == 0) { /* devicetree put IO Hub on link_lsit[0] */
				io_hub = link->children;
				if (!io_hub || !io_hub->enabled) {
					die("I can't find the IO Hub, or IO Hub not enabled, please check the device tree.\n");
				}
				/* Now that nothing is overlapping it is safe to scan the children. */
				max = pci_scan_bus(link, 0x00, ((next_unitid - 1) << 3) | 7, 0);
			}
		}
	}
	return max;
}
#endif
static struct device_operations northbridge_operations = {
	.read_resources	  = read_resources,
	.set_resources	  = set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = northbridge_init,
	//.scan_bus	  = scan_chains, /* TODO: */
	.enable		  = 0,
	.ops_pci	  = 0,
};

static const struct pci_driver family16_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_16H_MODEL_000F_NB_HT,
};

static const struct pci_driver family10_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_10H_NB_HT,
};

struct chip_operations northbridge_amd_agesa_family16kb_ops = {
	CHIP_NAME("AMD FAM16 Northbridge")
	.enable_dev = 0,
};

static void domain_read_resources(device_t dev)
{
	unsigned reg;

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for (reg = 0x80; reg <= 0xd8; reg+= 0x08) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned nodeid, reg_link;
			device_t reg_dev;
			if (reg<0xc0) { // mmio
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

#if !CONFIG_PCI_64BIT_PREF_MEM
	pci_domain_read_resources(dev);

#else
	struct bus *link;
	struct resource *resource;
	for (link=dev->link_list; link; link = link->next) {
		/* Initialize the system wide io space constraints */
		resource = new_resource(dev, 0|(link->link_num<<2));
		resource->base	= 0x400;
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO;

		/* Initialize the system wide prefetchable memory resources constraints */
		resource = new_resource(dev, 1|(link->link_num<<2));
		resource->limit = 0xfcffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;

		/* Initialize the system wide memory resources constraints */
		resource = new_resource(dev, 2|(link->link_num<<2));
		resource->limit = 0xfcffffffffULL;
		resource->flags = IORESOURCE_MEM;
	}
#endif
}

extern u8 acpi_slp_type;

static void domain_enable_resources(device_t dev)
{
	u32 val;
#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_slp_type == 3)
		agesawrapper_fchs3laterestore();
#endif

	/* Must be called after PCI enumeration and resource allocation */
	printk(BIOS_DEBUG, "\nFam16 - domain_enable_resources: AmdInitMid.\n");
#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_slp_type != 3) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitmid ");
		val = agesawrapper_amdinitmid ();
		if (val)
			printk(BIOS_DEBUG, "error level: %x \n", val);
		else
			printk(BIOS_DEBUG, "passed.\n");
	}
#else
	printk(BIOS_DEBUG, "agesawrapper_amdinitmid ");
	val = agesawrapper_amdinitmid ();
	if (val)
		printk(BIOS_DEBUG, "error level: %x \n", val);
	else
		printk(BIOS_DEBUG, "passed.\n");
#endif

	printk(BIOS_DEBUG, "  ader - leaving domain_enable_resources.\n");
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
	for (i = 0; i < node_nums; i++) {
		dram_base_mask_t d;
		u32 hole;
		d = get_dram_base_mask(i);
		if (!(d.mask & 1)) continue; // no memory on this node
		hole = pci_read_config32(__f1_dev[i], 0xf0);
		if (hole & 2) { // we find the hole
			mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
			mem_hole.node_id = i; // record the node No with hole
			break; // only one hole
		}
	}
	//We need to double check if there is speical set on base reg and limit reg are not continous instead of hole, it will find out it's hole_startk
	if (mem_hole.node_id == -1) {
		resource_t limitk_pri = 0;
		for (i=0; i<node_nums; i++) {
			dram_base_mask_t d;
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
			limit_k = ((resource_t)(((d.mask & ~1) + 0x000FF) & 0x1fffff00)) << 9;
			limitk_pri = limit_k;
		}
	}
	return mem_hole;
}
#endif

#define ONE_MB_SHIFT  20

static void setup_uma_memory(void)
{
#if CONFIG_GFXUMA
	uint32_t topmem = (uint32_t) bsp_topmem();
	uint32_t sys_mem;

	/* refer to UMA Size Consideration in Family16h BKDG. */
	/* Please reference MemNGetUmaSizeOR () */
	/*
	 *     Total system memory   UMASize
	 *     >= 2G                 512M
	 *     >=1G                  256M
	 *     <1G                    64M
	 */
	sys_mem = topmem + (16 << ONE_MB_SHIFT);   // Ignore 16MB allocated for C6 when finding UMA size
	if ((bsp_topmem2()>>32) || (sys_mem >= 2048 << ONE_MB_SHIFT)) {
		uma_memory_size = 512 << ONE_MB_SHIFT;
	} else if (sys_mem >= 1024 << ONE_MB_SHIFT) {
		uma_memory_size = 256 << ONE_MB_SHIFT;
	} else {
		uma_memory_size = 64 << ONE_MB_SHIFT;
	}
	uma_memory_base = topmem - uma_memory_size; /* TOP_MEM1 */

	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
			__func__, uma_memory_size, uma_memory_base);

	/* TODO: TOP_MEM2 */
#endif
}


static void domain_set_resources(device_t dev)
{
#if CONFIG_PCI_64BIT_PREF_MEM
	struct resource *io, *mem1, *mem2;
	struct resource *res;
#endif
	unsigned long mmio_basek;
	u32 pci_tolm;
	u64 ramtop = 0;
	int i, idx;
	struct bus *link;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
	u32 reset_memhole = 1;
#endif

#if CONFIG_PCI_64BIT_PREF_MEM

	for (link = dev->link_list; link; link = link->next) {
		/* Now reallocate the pci resources memory with the
		 * highest addresses I can manage.
		 */
		mem1 = find_resource(dev, 1|(link->link_num<<2));
		mem2 = find_resource(dev, 2|(link->link_num<<2));

		printk(BIOS_DEBUG, "base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
				mem1->base, mem1->limit, mem1->size, mem1->align);
		printk(BIOS_DEBUG, "base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
				mem2->base, mem2->limit, mem2->size, mem2->align);

		/* See if both resources have roughly the same limits */
		if (((mem1->limit <= 0xffffffff) && (mem2->limit <= 0xffffffff)) ||
				((mem1->limit > 0xffffffff) && (mem2->limit > 0xffffffff)))
		{
			/* If so place the one with the most stringent alignment first */
			if (mem2->align > mem1->align) {
				struct resource *tmp;
				tmp = mem1;
				mem1 = mem2;
				mem2 = tmp;
			}
			/* Now place the memory as high up as it will go */
			mem2->base = resource_max(mem2);
			mem1->limit = mem2->base - 1;
			mem1->base = resource_max(mem1);
		}
		else {
			/* Place the resources as high up as they will go */
			mem2->base = resource_max(mem2);
			mem1->base = resource_max(mem1);
		}

		printk(BIOS_DEBUG, "base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
				mem1->base, mem1->limit, mem1->size, mem1->align);
		printk(BIOS_DEBUG, "base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
				mem2->base, mem2->limit, mem2->size, mem2->align);
	}

	for (res = &dev->resource_list; res; res = res->next)
	{
		res->flags |= IORESOURCE_ASSIGNED;
		res->flags |= IORESOURCE_STORED;
		report_resource_stored(dev, res, "");
	}
#endif

	pci_tolm = 0xffffffffUL;
	for (link = dev->link_list; link; link = link->next) {
		pci_tolm = find_pci_tolm(link);
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
	for (i = 0; i < node_nums; i++) {
		dram_base_mask_t d;
		resource_t basek, limitk, sizek; // 4 1T

		d = get_dram_base_mask(i);

		if (!(d.mask & 1)) continue;
		basek = ((resource_t)(d.base & 0x1fffff00)) << 9; // could overflow, we may lost 6 bit here
		limitk = ((resource_t)(((d.mask & ~1) + 0x000FF) & 0x1fffff00)) << 9 ;

		sizek = limitk - basek;

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < ((8*64)+(8*16))) && (sizek > ((8*64)+(16*16)))) {
			ram_resource(dev, (idx | i), basek, ((8*64)+(8*16)) - basek);
			idx += 0x10;
			basek = (8*64)+(16*16);
			sizek = limitk - ((8*64)+(16*16));

		}

		//printk(BIOS_DEBUG, "node %d : mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n", i, mmio_basek, basek, limitk);

		/* split the region to accomodate pci memory space */
		if ((basek < 4*1024*1024 ) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek>0) {
					ram_resource(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
					if (!ramtop)
						ramtop = mmio_basek * 1024;
				}
				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4*1024*1024) {
				sizek = 0;
			}
			else {
				uint64_t topmem2 = bsp_topmem2();
				basek = 4*1024*1024;
				sizek = topmem2/1024 - basek;
			}
		}

		ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
		printk(BIOS_DEBUG, "node %d: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
				i, mmio_basek, basek, limitk);
		if (!ramtop)
			ramtop = limitk * 1024;
	}

#if CONFIG_GFXUMA
	set_top_of_ram(uma_memory_base);
	uma_resource(dev, 7, uma_memory_base >> 10, uma_memory_size >> 10);
#else
	set_top_of_ram(ramtop);
#endif

	for(link = dev->link_list; link; link = link->next) {
		if (link->children) {
			assign_resources(link);
		}
	}
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = domain_set_resources,
	.enable_resources = domain_enable_resources,
	.init		  = NULL,
	.scan_bus	  = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
};

static void sysconf_init(device_t dev) // first node
{
	sblink = (pci_read_config32(dev, 0x64)>>8) & 7; // don't forget sublink1
	node_nums = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1; //NodeCnt[2:0]
}

static void add_more_links(device_t dev, unsigned total_links)
{
	struct bus *link, *last = NULL;
	int link_num;

	for (link = dev->link_list; link; link = link->next)
		last = link;

	if (last) {
		int links = total_links - last->link_num;
		link_num = last->link_num;
		if (links > 0) {
			link = malloc(links*sizeof(*link));
			if (!link)
				die("Couldn't allocate more links!\n");
			memset(link, 0, links*sizeof(*link));
			last->next = link;
		}
	}
	else {
		link_num = -1;
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

static u32 cpu_bus_scan(device_t dev, u32 max)
{
	struct bus *cpu_bus;
	device_t dev_mc;
#if CONFIG_CBB
	device_t pci_domain;
#endif
	int i,j;
	int coreid_bits;
	int core_max = 0;
	unsigned ApicIdCoreIdSize;
	unsigned core_nums;
	int siblings = 0;
	unsigned int family;

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
#if CONFIG_CBB && (MAX_NODE_NUMS > 32)
	if (node_nums>32) { // need to put node 32 to node 63 to bus 0xfe
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

	/* Get Max Number of cores(MNC) */
	coreid_bits = (cpuid_ecx(AMD_CPUID_ASIZE_PCCOUNT) & 0x0000F000) >> 12;
	core_max = 1 << (coreid_bits & 0x000F); //mnc

	ApicIdCoreIdSize = ((cpuid_ecx(0x80000008)>>12) & 0xF);
	if (ApicIdCoreIdSize) {
		core_nums = (1 << ApicIdCoreIdSize) - 1;
	} else {
		core_nums = 3; //quad core
	}

	/* Find which cpus are present */
	cpu_bus = dev->link_list;
	for (i = 0; i < node_nums; i++) {
		device_t cdb_dev;
		unsigned busn, devn;
		struct bus *pbus;

		busn = CONFIG_CBB;
		devn = CONFIG_CDB + i;
		pbus = dev_mc->bus;
#if CONFIG_CBB && (MAX_NODE_NUMS > 32)
		if (i >= 32) {
			busn--;
			devn -= 32;
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
			for(fn = 0; fn <= 5; fn++) { //FBDIMM?
				cdb_dev = pci_probe_dev(NULL, pbus,
							PCI_DEVFN(devn, fn));
			}
			cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 0));
		} else {
			/* Ok, We need to set the links for that device.
			 * otherwise the device under it will not be scanned
			 */
			int linknum;
#if CONFIG_HT3_SUPPORT
			linknum = 8;
#else
			linknum = 4;
#endif
			add_more_links(cdb_dev, linknum);
		}

		family = cpuid_eax(1);
		family = (family >> 20) & 0xFF;
		if (family == 1) { //f10
			u32 dword;
			cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 3));
			dword = pci_read_config32(cdb_dev, 0xe8);
			siblings = ((dword & BIT15) >> 13) | ((dword & (BIT13 | BIT12)) >> 12);
		} else if (family == 7) {//f16
			cdb_dev = dev_find_slot(busn, PCI_DEVFN(devn, 5));
			if (cdb_dev && cdb_dev->enabled) {
				siblings = pci_read_config32(cdb_dev, 0x84);
				siblings &= 0xFF;
			}
		} else {
			siblings = 0; //default one core
		}
		int enable_node = cdb_dev && cdb_dev->enabled;
		printk(BIOS_SPEW, "%s family%xh, core_max=0x%x, core_nums=0x%x, siblings=0x%x\n",
				dev_path(cdb_dev), 0x0f + family, core_max, core_nums, siblings);

		for (j = 0; j <= siblings; j++ ) {
			extern CONST OPTIONS_CONFIG_TOPOLOGY ROMDATA TopologyConfiguration;
			u32 modules = TopologyConfiguration.PlatformNumberOfModules;
			u32 lapicid_start = 0;

			/*
			 * APIC ID calucation is tightly coupled with AGESA v5 code.
			 * This calculation MUST match the assignment calculation done
			 * in LocalApicInitializationAtEarly() function.
			 * And reference GetLocalApicIdForCore()
			 *
			 * Apply apic enumeration rules
			 * For systems with >= 16 APICs, put the IO-APICs at 0..n and
			 * put the local-APICs at m..z
			 *
			 * This is needed because many IO-APIC devices only have 4 bits
			 * for their APIC id and therefore must reside at 0..15
                         */
#ifndef CFG_PLAT_NUM_IO_APICS /* defined in mainboard buildOpts.c */
#define CFG_PLAT_NUM_IO_APICS 3
#endif
			if ((node_nums * core_max) + CFG_PLAT_NUM_IO_APICS >= 0x10) {
				lapicid_start = (CFG_PLAT_NUM_IO_APICS - 1) / core_max;
				lapicid_start = (lapicid_start + 1) * core_max;
				printk(BIOS_SPEW, "lpaicid_start=0x%x ", lapicid_start);
			}
			u32 apic_id = (lapicid_start * (i/modules + 1)) + ((i % modules) ? (j + (siblings + 1)) : j);
			printk(BIOS_SPEW, "node 0x%x core 0x%x apicid=0x%x\n",
					i, j, apic_id);

			device_t cpu = add_cpu_device(cpu_bus, apic_id, enable_node);
			if (cpu)
				amd_cpu_topology(cpu, i, j);
		} //j
	}
	return max;
}

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static void cpu_bus_noop(device_t dev)
{
}

static void cpu_bus_read_resources(device_t dev)
{
#if CONFIG_MMCONF_SUPPORT
	struct resource *resource = new_resource(dev, 0xc0010058);
	resource->base = CONFIG_MMCONF_BASE_ADDRESS;
	resource->size = CONFIG_MMCONF_BUS_NUMBER * 4096*256;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED |  IORESOURCE_ASSIGNED;
#endif
}

static void cpu_bus_set_resources(device_t dev)
{
	struct resource *resource = find_resource(dev, 0xc0010058);
	if (resource) {
		report_resource_stored(dev, resource, " <mmconfig>");
	}
	pci_dev_set_resources(dev);
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = cpu_bus_read_resources,
	.set_resources	  = cpu_bus_set_resources,
	.enable_resources = cpu_bus_noop,
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

struct chip_operations northbridge_amd_agesa_family16kb_root_complex_ops = {
	CHIP_NAME("AMD FAM16 Root Complex")
	.enable_dev = root_complex_enable_dev,
};

/*********************************************************************
 * Change the vendor / device IDs to match the generic VBIOS header. *
 *********************************************************************/
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch(vendev) {
	case 0x10029830:
	case 0x10029831:
	case 0x10029832:
	case 0x10029833:
	case 0x10029834:
	case 0x10029835:
	case 0x10029836:
	case 0x10029837:
	case 0x10029838:
	case 0x10029839:
	case 0x1002983A:
	case 0x1002983D:
		new_vendev = 0x10029830;  // This is the default value in AMD-generated VBIOS
		break;
	default:
		break;
	}

	if (vendev != new_vendev)
		printk(BIOS_NOTICE, "Mapping PCI device %8x to %8x\n", vendev, new_vendev);

	return new_vendev;
}
