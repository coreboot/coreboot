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
#include <mainboard.h>

#ifdef CONFIG_PCI_64BIT_PREF_MEM
#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM | IORESOURCE_PREFETCH)
#endif

#define FX_DEVS 8
extern struct device * __f0_dev[FX_DEVS];
extern struct device * __f1_dev[FX_DEVS];
void debug_fx_devs(void);
void get_fx_devs(void);
u32 f1_read_config32(unsigned int reg);
void f1_write_config32(unsigned int reg, u32 value);
unsigned int amdk8_nodeid(struct device * dev);

static void k8_ram_resource(struct device * dev, unsigned long index, 
	unsigned long basek, unsigned long sizek)
{
	struct resource *resource;

	if (!sizek) {
		return;
	}
	resource = new_resource(dev, index);
	resource->base  = ((resource_t)basek) << 10;
	resource->size  = ((resource_t)sizek) << 10;
	resource->flags =  IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

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
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

static void k8_pci_domain_read_resources(struct device * dev)
{
	struct resource *resource;
	unsigned reg;

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for(reg = 0x80; reg <= 0xd8; reg+= 0x08) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned nodeid, link;
			struct device * dev;
			nodeid = limit & 7;
			link   = (limit >> 4) & 3;
			dev = __f0_dev[nodeid];
			if (dev) {
				/* Reserve the resource  */
				struct resource *resource;
				resource = new_resource(dev, 0x100 + (reg | link));
				if (resource) {
					resource->flags = 1;
				}
			}
		}
	}
#if CONFIG_PCI_64BIT_PREF_MEM == 0
	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->base  = 0x400;
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

        /* Initialize the system wide memory resources constraints */
        resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
        resource->limit = 0xfcffffffffULL;
        resource->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
#else
        /* Initialize the system wide io space constraints */
        resource = new_resource(dev, 0);
        resource->base  = 0x400;
        resource->limit = 0xffffUL;
        resource->flags = IORESOURCE_IO;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_IO, IORESOURCE_IO);

        /* Initialize the system wide prefetchable memory resources constraints */
        resource = new_resource(dev, 1);
        resource->limit = 0xfcffffffffULL;
        resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_MEM | IORESOURCE_PREFETCH,
                IORESOURCE_MEM | IORESOURCE_PREFETCH);

        /* Initialize the system wide memory resources constraints */
        resource = new_resource(dev, 2);
        resource->limit = 0xfcffffffffULL;
        resource->flags = IORESOURCE_MEM;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_MEM | IORESOURCE_PREFETCH,
                IORESOURCE_MEM);
#endif
}

static void k8_pci_domain_set_resources(struct device * dev)
{
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info get_hw_mem_hole_info(void);
	void disable_hoist_memory(unsigned long hole_startk, int i);
	u32 hoist_memory(unsigned long hole_startk, int i);
#endif
#if CONFIG_PCI_64BIT_PREF_MEM == 1
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

#if 0
        /* Place the IO devices somewhere safe */
        io = find_resource(dev, 0);
        io->base = DEVICE_IO_START;
#endif
#if CONFIG_PCI_64BIT_PREF_MEM == 1
        /* Now reallocate the pci resources memory with the
         * highest addresses I can manage.
         */
        mem1 = find_resource(dev, 1);
        mem2 = find_resource(dev, 2);

#if 1
                printk(BIOS_DEBUG, "base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem1->base, mem1->limit, mem1->size, mem1->align);
                printk(BIOS_DEBUG, "base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem2->base, mem2->limit, mem2->size, mem2->align);
#endif

        /* See if both resources have roughly the same limits */
        if (((mem1->limit <= 0xffffffff) && (mem2->limit <= 0xffffffff)) ||
                ((mem1->limit > 0xffffffff) && (mem2->limit > 0xffffffff)))
        {
                /* If so place the one with the most stringent alignment first
                 */
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

#if 1
                printk(BIOS_DEBUG, "base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem1->base, mem1->limit, mem1->size, mem1->align);
                printk(BIOS_DEBUG, "base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem2->base, mem2->limit, mem2->size, mem2->align);
#endif

        last = &dev->resource[dev->resources];
        for(resource = &dev->resource[0]; resource < last; resource++)
        {
#if 1
                resource->flags |= IORESOURCE_ASSIGNED;
                resource->flags &= ~IORESOURCE_STORED;
#endif
                compute_allocate_resource(&dev->link[0], resource,
                        BRIDGE_IO_MASK, resource->flags & BRIDGE_IO_MASK);

                resource->flags |= IORESOURCE_STORED;
                report_resource_stored(dev, resource, "");

        }
#endif


	pci_tolm = find_pci_tolm(&dev->link[0]);

#warning "FIXME handle interleaved nodes"
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

#if 1
#warning "FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M MMIO hole"
	/* Round the mmio hold to 64M */
	mmio_basek &= ~((64*1024) - 1);
#endif

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
    /* if the hw mem hole is already set in raminit stage, here we will compare mmio_basek and hole_basek
     * if mmio_basek is bigger that hole_basek and will use hole_basek as mmio_basek and we don't need to reset hole.
     * otherwise We reset the hole to the mmio_basek
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
			k8_ram_resource(dev, (idx | i), basek, ((8*64)+(8*16)) - basek);
			idx += 0x10;
			basek = (8*64)+(16*16);
			sizek = limitk - ((8*64)+(16*16));
			
		}

	
		printk(BIOS_DEBUG, "node %d : mmio_basek=%08x, basek=%08lx, limitk=%08x\n", i, mmio_basek, basek, limitk); //yhlu 
			
		/* See if I need to split the region to accomodate pci memory space */
		if ( (basek < 4*1024*1024 ) && (limitk > mmio_basek) ) {
			if (basek <= mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - basek;
				if(pre_sizek>0) {
					k8_ram_resource(dev, (idx | i), basek, pre_sizek);
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
		k8_ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
	}
	phase4_assign_resources(&dev->link[0]);
}

static unsigned int k8_domain_scan_bus(struct device * dev, unsigned int max)
{
	unsigned reg;
	int i;
	/* Unmap all of the HT chains */
	for(reg = 0xe0; reg <= 0xec; reg += 4) {
		f1_write_config32(reg, 0);
	}
	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0x18, 0), 0xff, max);  
	
	/* Tune the hypertransport transaction for best performance.
	 * Including enabling relaxed ordering if it is safe.
	 */
	get_fx_devs();
	for(i = 0; i < FX_DEVS; i++) {
		struct device * f0_dev;
		f0_dev = __f0_dev[i];
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
	return max;
}

struct device_operations k8apic_ops = {
	.id = {.type = DEVICE_ID_APIC_CLUSTER,
		{.pci_domain = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = 0x1100}}},
	.constructor		 = default_device_constructor,
	.phase3_scan			= k8_domain_scan_bus,
	.phase4_read_resources	 = k8_pci_domain_read_resources,
	.phase4_set_resources	 = k8_pci_domain_set_resources,
	.phase5_enable_resources = enable_childrens_resources,
	.ops_pci		 = &pci_dev_ops_pci,
	.ops_pci_bus      = &pci_cf8_conf1,
};
