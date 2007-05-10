/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include <cpu/amd/lxdef.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/vr.h>
#include "chip.h"
#include "northbridge.h"

/* here is programming for the various MSRs.*/
#define IM_QWAIT 0x100000

#define DMCF_WRITE_SERIALIZE_REQUEST (2<<12) /* 2 outstanding */	/* in high */
#define DMCF_SERIAL_LOAD_MISSES  (2)	/* enabled */

/* these are the 8-bit attributes for controlling RCONF registers */
#define CACHE_DISABLE (1<<0)
#define WRITE_ALLOCATE (1<<1)
#define WRITE_PROTECT (1<<2)
#define WRITE_THROUGH (1<<3)
#define WRITE_COMBINE (1<<4)
#define WRITE_SERIALIZE (1<<5)

/* ram has none of this stuff */
#define RAM_PROPERTIES (0)
#define DEVICE_PROPERTIES (WRITE_SERIALIZE|CACHE_DISABLE)
#define ROM_PROPERTIES (WRITE_SERIALIZE|WRITE_PROTECT|CACHE_DISABLE)
#define MSR_WS_CD_DEFAULT (0x21212121)

/* 1810-1817 give you 8 registers with which to program protection regions */
/* the are region configuration range registers, or RRCF */
/* in msr terms, the are a straight base, top address assign, since they are 4k aligned. */
/* so no left-shift needed for top or base */
#define RRCF_LOW(base,properties) (base|(1<<8)|properties)
#define RRCF_LOW_CD(base)	RRCF_LOW(base, CACHE_DISABLE)

/* build initializer for P2D MSR */
#define P2D_BM(msr, pdid1, bizarro, pbase, pmask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(pbase>>24), .lo=(pbase<<8)|pmask}}
#define P2D_BMO(msr, pdid1, bizarro, poffset, pbase, pmask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(poffset<<8)|(pbase>>24), .lo=(pbase<<8)|pmask}}
#define P2D_R(msr, pdid1, bizarro, pmax, pmin) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(pmax>>12), .lo=(pmax<<20)|pmin}}
#define P2D_RO(msr, pdid1, bizarro, poffset, pmax, pmin) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(poffset<<8)|(pmax>>12), .lo=(pmax<<20)|pmin}}
#define P2D_SC(msr, pdid1, bizarro, wen, ren,pscbase) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(wen), .lo=(ren<<16)|(pscbase>>18)}}
#define IOD_BM(msr, pdid1, bizarro, ibase, imask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(ibase>>12), .lo=(ibase<<20)|imask}}
#define IOD_SC(msr, pdid1, bizarro, en, wen, ren, ibase) {msr, {.hi=(pdid1<<29)|(bizarro<<28), .lo=(en<<24)|(wen<<21)|(ren<<20)|(ibase<<3)}}

#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM)

extern void graphics_init(void);
extern void cpubug(void);
extern void chipsetinit(void);
extern void print_conf(void);
extern uint32_t get_systop(void);

void northbridge_init_early(void);
void setup_realmode_idt(void);
void do_vsmbios(void);

struct msr_defaults {
	int msr_no;
	msr_t msr;
} msr_defaults[] = {
	{
		0x1700, {
	.hi = 0,.lo = IM_QWAIT}}, {
		0x1800, {
	.hi = DMCF_WRITE_SERIALIZE_REQUEST,.lo =
			    DMCF_SERIAL_LOAD_MISSES}},
	    /* 1808 will be done down below, so we have to do 180a->1817 (well, 1813 really) */
	    /* for 180a, for now, we assume VSM will configure it */
	    /* 180b is left at reset value,a0000-bffff is non-cacheable */
	    /* 180c, c0000-dffff is set to write serialize and non-cachable */
	    /* oops, 180c will be set by cpu bug handling in cpubug.c */
	    //{0x180c, {.hi = MSR_WS_CD_DEFAULT, .lo = MSR_WS_CD_DEFAULT}},
	    /* 180d is left at default, e0000-fffff is non-cached */
	    /* we will assume 180e, the ssm region configuration, is left at default or set by VSM */
	    /* we will not set 0x180f, the DMM,yet */
	    //{0x1810, {.hi=0xee7ff000, .lo=RRCF_LOW(0xee000000, WRITE_COMBINE|CACHE_DISABLE)}},
	    //{0x1811, {.hi = 0xefffb000, .lo = RRCF_LOW_CD(0xefff8000)}},
	    //{0x1812, {.hi = 0xefff7000, .lo = RRCF_LOW_CD(0xefff4000)}},
	    //{0x1813, {.hi = 0xefff3000, .lo = RRCF_LOW_CD(0xefff0000)}},
	    /* now for GLPCI routing */
	    /* GLIU0 */
	    P2D_BM(MSR_GLIU0_BASE1, 0x1, 0x0, 0x0, 0xfff80),
	    P2D_BM(MSR_GLIU0_BASE2, 0x1, 0x0, 0x80000, 0xfffe0),
	    P2D_SC(MSR_GLIU0_SHADOW, 0x1, 0x0, 0x0, 0xff03, 0xC0000),
	    /* GLIU1 */
	    P2D_BM(MSR_GLIU1_BASE1, 0x1, 0x0, 0x0, 0xfff80),
	    P2D_BM(MSR_GLIU1_BASE2, 0x1, 0x0, 0x80000, 0xfffe0),
	    P2D_SC(MSR_GLIU1_SHADOW, 0x1, 0x0, 0x0, 0xff03, 0xC0000), {
	0}
};

/* todo: add a resource record. We don't do this here because this may be called when 
  * very little of the platform is actually working.
  */
int sizeram(void)
{
	msr_t msr;
	int sizem = 0;
	unsigned short dimm;

	msr = rdmsr(MC_CF07_DATA);
	printk_debug("sizeram: _MSR MC_CF07_DATA: %08x:%08x\n", msr.hi, msr.lo);

	/* dimm 0 */
	dimm = msr.hi;
	/* installed? */
	if ((dimm & 7) != 7) {
		sizem = 4 << ((dimm >> 12) & 0x0F);
	}

	/* dimm 1 */
	dimm = msr.hi >> 16;
	/* installed? */
	if ((dimm & 7) != 7) {
		sizem += 4 << ((dimm >> 12) & 0x0F);
	}

	printk_debug("sizeram: sizem 0x%xMB\n", sizem);
	return sizem;
}

static void enable_shadow(device_t dev)
{
}

static void northbridge_init(device_t dev)
{
	//msr_t msr;

	printk_spew(">> Entering northbridge.c: %s\n", __FUNCTION__);

	enable_shadow(dev);
	/*
	 * Swiss cheese
	 */
	//msr = rdmsr(MSR_GLIU0_SHADOW);

	//msr.hi |= 0x3;
	//msr.lo |= 0x30000;

	//printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", MSR_GLIU0_SHADOW, msr.hi, msr.lo);
	//printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", MSR_GLIU1_SHADOW, msr.hi, msr.lo);
}

void northbridge_set_resources(struct device *dev)
{
	struct resource *resource, *last;
	unsigned link;
	uint8_t line;

	last = &dev->resource[dev->resources];

	for (resource = &dev->resource[0]; resource < last; resource++) {

		// andrei: do not change the base address, it will make the VSA virtual registers unusable
		//pci_set_resource(dev, resource);
		// FIXME: static allocation may conflict with dynamic mappings!
	}

	for (link = 0; link < dev->links; link++) {
		struct bus *bus;
		bus = &dev->link[link];
		if (bus->children) {
			printk_debug
			    ("my_dev_set_resources: assign_resources %d\n",
			     bus);
			assign_resources(bus);
		}
	}

	/* set a default latency timer */
	pci_write_config8(dev, PCI_LATENCY_TIMER, 0x40);

	/* set a default secondary latency timer */
	if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE) {
		pci_write_config8(dev, PCI_SEC_LATENCY_TIMER, 0x40);
	}

	/* zero the irq settings */
	line = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (line) {
		pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);
	}

	/* set the cache line size, so far 64 bytes is good for everyone */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 64 >> 2);
}

static struct device_operations northbridge_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = northbridge_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = northbridge_init,
	.enable = 0,
	.ops_pci = 0,
};

static struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_LXBRIDGE,
};

static void pci_domain_read_resources(device_t dev)
{
	struct resource *resource;
	printk_spew(">> Entering northbridge.c: %s\n", __FUNCTION__);

	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->limit = 0xffffUL;
	resource->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->limit = 0xffffffffULL;
	resource->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void ram_resource(device_t dev, unsigned long index,
			 unsigned long basek, unsigned long sizek)
{
	struct resource *resource;

	if (!sizek)
		return;

	resource = new_resource(dev, index);
	resource->base = ((resource_t) basek) << 10;
	resource->size = ((resource_t) sizek) << 10;
	resource->flags = IORESOURCE_MEM | IORESOURCE_CACHEABLE |
	    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void pci_domain_set_resources(device_t dev)
{
	int idx;
	device_t mc_dev;

	printk_spew(">> Entering northbridge.c: %s\n", __FUNCTION__);

	mc_dev = dev->link[0].children;
	if (mc_dev) {
		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 1024, (get_systop() - 0x100000) / 1024);	// Systop - 1 MB -> KB
	}

	assign_resources(&dev->link[0]);
}

static void pci_domain_enable(device_t dev)
{

	printk_spew(">> Entering northbridge.c: %s\n", __FUNCTION__);

	// do this here for now -- this chip really breaks our device model
	northbridge_init_early();
	cpubug();
	chipsetinit();

	setup_realmode_idt();

	printk_debug("Before VSA:\n");
	// print_conf();

	do_vsmbios();		// do the magic stuff here, so prepare your tambourine ;)

	printk_debug("After VSA:\n");
	// print_conf();

	graphics_init();
	pci_set_method(dev);
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
	printk_spew(">> Entering northbridge.c: %s\n", __FUNCTION__);

	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = enable_childrens_resources,
	.scan_bus = pci_domain_scan_bus,
	.enable = pci_domain_enable,
};

static void cpu_bus_init(device_t dev)
{
	printk_spew(">> Entering northbridge.c: %s\n", __FUNCTION__);

	initialize_cpus(&dev->link[0]);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources = cpu_bus_noop,
	.set_resources = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init = cpu_bus_init,
	.scan_bus = 0,
};

static void enable_dev(struct device *dev)
{
	printk_spew(">> Entering northbridge.c: %s with path %d\n",
		    __FUNCTION__, dev->path.type);

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

struct chip_operations northbridge_amd_lx_ops = {
	CHIP_NAME("AMD LX Northbridge")
	    .enable_dev = enable_dev,
};
