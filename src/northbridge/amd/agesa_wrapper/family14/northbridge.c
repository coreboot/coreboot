/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
#include <bitops.h>
#include <cpu/cpu.h>

#include <cpu/x86/lapic.h>

#include "chip.h"
#include "northbridge.h"


//#define FX_DEVS NODE_NUMS
#define FX_DEVS 1

static device_t __f0_dev[FX_DEVS];
static device_t __f1_dev[FX_DEVS];
static device_t __f2_dev[FX_DEVS];
static device_t __f4_dev[FX_DEVS];
static unsigned fx_devs=0;


device_t get_node_pci(u32 nodeid, u32 fn)
{
    return dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB + nodeid, fn));
}


static void get_fx_devs(void)
{
    int i;
    for(i = 0; i < FX_DEVS; i++) {
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
    for(i = 0; i < fx_devs; i++) {
        device_t dev;
        dev = __f1_dev[i];
        if (dev && dev->enabled) {
            pci_write_config32(dev, reg, value);
        }
    }
}


static u32 amdfam14_nodeid(device_t dev)
{
    return (dev->path.pci.devfn >> 3) - CONFIG_CDB;
}


#include "amdfam14_conf.c"


static void northbridge_init(device_t dev)
{
  printk(BIOS_DEBUG, "Northbridge init\n");
}


static void set_vga_enable_reg(u32 nodeid, u32 linkn)
{
    u32 val;

    val =  1 | (nodeid<<4) | (linkn<<12);
    /* it will routing (1)mmio  0xa0000:0xbffff (2) io 0x3b0:0x3bb,
     0x3c0:0x3df */
    f1_write_config32(0xf4, val);

}


static int reg_useable(unsigned reg, device_t goal_dev, unsigned goal_nodeid,
            unsigned goal_link)
{
    struct resource *res;
    unsigned nodeid, link = 0;
    int result;
    res = 0;
    for(nodeid = 0; !res && (nodeid < fx_devs); nodeid++) {
        device_t dev;
        dev = __f0_dev[nodeid];
        if (!dev)
            continue;
        for(link = 0; !res && (link < 8); link++) {
            res = probe_resource(dev, IOINDEX(0x1000 + reg, link));
        }
    }
    result = 2;
    if (res) {
        result = 0;
        if (    (goal_link == (link - 1)) &&
            (goal_nodeid == (nodeid - 1)) &&
            (res->flags <= 1)) {
            result = 1;
        }
    }
    return result;
}

static struct resource *amdfam14_find_iopair(device_t dev, unsigned nodeid, unsigned link)
{
    struct resource *resource;
    u32 result, reg;
    resource = 0;
    reg = 0;
        result = reg_useable(0xc0, dev, nodeid, link);
        if (result >= 1) {
            /* I have been allocated this one */
            reg = 0xc0;
    }

    //Ext conf space
    if(!reg) {
        //because of Extend conf space, we will never run out of reg, but we need one index to differ them. so same node and same link can have multi range
        u32 index = get_io_addr_index(nodeid, link);
        reg = 0x110+ (index<<24) + (4<<20); // index could be 0, 255
    }

        resource = new_resource(dev, IOINDEX(0x1000 + reg, link));

    return resource;
}

static struct resource *amdfam14_find_mempair(device_t dev, u32 nodeid, u32 link)
{
    struct resource *resource;
    u32 free_reg, reg;
    resource = 0;
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

    //Ext conf space
    if(!reg) {
        //because of Extend conf space, we will never run out of reg,
        // but we need one index to differ them. so same node and
        // same link can have multi range
        u32 index = get_mmio_addr_index(nodeid, link);
        reg = 0x110+ (index<<24) + (6<<20); // index could be 0, 63

    }
    resource = new_resource(dev, IOINDEX(0x1000 + reg, link));
    return resource;
}


static void amdfam14_link_read_bases(device_t dev, u32 nodeid, u32 link)
{
    struct resource *resource;

    /* Initialize the io space constraints on the current bus */
    resource = amdfam14_find_iopair(dev, nodeid, link);
    if (resource) {
        u32 align;
#if CONFIG_EXT_CONF_SUPPORT == 1
        if((resource->index & 0x1fff) == 0x1110) { // ext
            align = 8;
        }
        else
#endif
            align = log2(HT_IO_HOST_ALIGN);
        resource->base  = 0;
        resource->size  = 0;
        resource->align = align;
        resource->gran  = align;
        resource->limit = 0xffffUL;
        resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
    }

    /* Initialize the prefetchable memory constraints on the current bus */
    resource = amdfam14_find_mempair(dev, nodeid, link);
    if (resource) {
        resource->base = 0;
        resource->size = 0;
        resource->align = log2(HT_MEM_HOST_ALIGN);
        resource->gran = log2(HT_MEM_HOST_ALIGN);
        resource->limit = 0xffffffffffULL;
        resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
        resource->flags |= IORESOURCE_BRIDGE;

#if CONFIG_EXT_CONF_SUPPORT == 1
        if((resource->index & 0x1fff) == 0x1110) { // ext
            normalize_resource(resource);
        }
#endif

    }

    /* Initialize the memory constraints on the current bus */
    resource = amdfam14_find_mempair(dev, nodeid, link);
    if (resource) {
        resource->base = 0;
        resource->size = 0;
        resource->align = log2(HT_MEM_HOST_ALIGN);
        resource->gran = log2(HT_MEM_HOST_ALIGN);
        resource->limit = 0xffffffffffULL;
        resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
#if CONFIG_EXT_CONF_SUPPORT == 1
        if((resource->index & 0x1fff) == 0x1110) { // ext
            normalize_resource(resource);
        }
#endif
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

        struct dram_base_mask_t d;
        u32 hole;
        d = get_dram_base_mask(0);
        if(d.mask & 1) {
            hole = pci_read_config32(__f1_dev[0], 0xf0);
            if(hole & 1) { // we find the hole
                mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
                mem_hole.node_id = 0; // record the node No with hole
            }
        }

#if 0
        // We need to double check if there is speical set on base reg and limit reg 
            // are not continous instead of hole, it will find out it's hole_startk
        if(mem_hole.node_id==-1) {
            resource_t limitk_pri = 0;
            struct dram_base_mask_t d;
            resource_t base_k, limit_k;
            d = get_dram_base_mask(0);
            if(d.base & 1) {
                base_k = ((resource_t)(d.base & 0x1fffff00)) <<9;
                if(base_k <= 4 *1024 * 1024) {
                    if(limitk_pri != base_k) { // we find the hole
                        mem_hole.hole_startk = (unsigned)limitk_pri; // must be below 4G
                        mem_hole.node_id = 0;
                    }
                }

                limit_k = ((resource_t)((d.mask + 0x00000100) & 0x1fffff00)) << 9;
                limitk_pri = limit_k;
            }
        }
#endif
        
        return mem_hole;
}
#endif

#if CONFIG_WRITE_HIGH_TABLES==1
#define HIGH_TABLES_SIZE 64 // maximum size of high tables in KB
extern uint64_t high_tables_base, high_tables_size;
#endif

#if CONFIG_GFXUMA == 1
extern uint64_t uma_memory_base, uma_memory_size;

static void add_uma_resource(struct device *dev, int index)
{
    struct resource *resource;

    printk(BIOS_DEBUG, "\nFam14h - Adding UMA memory.\n");

    resource = new_resource(dev, index);
    resource->base = (resource_t) uma_memory_base;
    resource->size = (resource_t) uma_memory_size;
    resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
        IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}
#endif

static void read_resources(device_t dev)
{
    u32 nodeid;
    struct bus *link;

    printk(BIOS_DEBUG, "\nFam14h - read_resources.\n");

    nodeid = amdfam14_nodeid(dev);
    for(link = dev->link_list; link; link = link->next) {
        if (link->children) {
            amdfam14_link_read_bases(dev, nodeid, link->link_num);
        }
    }
}


static void set_resource(device_t dev, struct resource *resource,
                u32 nodeid)
{
    resource_t rbase, rend;
    unsigned reg, link_num;
    char buf[50];

    printk(BIOS_DEBUG, "\nFam14h - set_resource.\n");

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
        set_mmio_addr_reg(nodeid, link_num, reg, (resource->index >>24), rbase>>8, rend>>8, 1) ;// [39:8]
    }
    resource->flags |= IORESOURCE_STORED;
    sprintf(buf, " <node %x link %x>",
        nodeid, link_num);
    report_resource_stored(dev, resource, buf);
}


#if CONFIG_CONSOLE_VGA_MULTI == 1
extern device_t vga_pri;    // the primary vga device, defined in device.c
#endif

static void create_vga_resource(device_t dev, unsigned nodeid)
{
    struct bus *link;

    printk(BIOS_DEBUG, "\nFam14h - create_vga_resource.\n");

    /* find out which link the VGA card is connected,
     * we only deal with the 'first' vga card */
    for (link = dev->link_list; link; link = link->next) {
        if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if CONFIG_CONSOLE_VGA_MULTI == 1
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

    printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, link->link_num);
    set_vga_enable_reg(nodeid, link->link_num);
}


static void set_resources(device_t dev)
{
    unsigned nodeid;
    struct bus *bus;
    struct resource *res;

    printk(BIOS_DEBUG, "\nFam14h - set_resources.\n");
 
    /* Find the nodeid */
    nodeid = amdfam14_nodeid(dev);

    create_vga_resource(dev, nodeid);

    /* Set each resource we have found */
    for(res = dev->resource_list; res; res = res->next) {
        set_resource(dev, res, nodeid);
    }

    for(bus = dev->link_list; bus; bus = bus->next) {
        if (bus->children) {
            assign_resources(bus);
        }
    }
}


/* Domain/Root Complex related code */

static void domain_read_resources(device_t dev)
{
    unsigned reg;

    printk(BIOS_DEBUG, "\nFam14h - domain_read_resources.\n");

    /* Find the already assigned resource pairs */
    get_fx_devs();
    for(reg = 0x80; reg <= 0xc0; reg+= 0x08) {
        u32 base, limit;
        base  = f1_read_config32(reg);
        limit = f1_read_config32(reg + 0x04);
        /* Is this register allocated? */
        if ((base & 3) != 0) {
            unsigned nodeid, reg_link;
            device_t reg_dev;
            if(reg<0xc0) { // mmio
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

#if CONFIG_PCI_64BIT_PREF_MEM == 0
    pci_domain_read_resources(dev);
#else
    struct bus *link;
    struct resource *resource;
    for(link=dev->link_list; link; link = link->next) {
        /* Initialize the system wide io space constraints */
        resource = new_resource(dev, 0|(link->link_num<<2));
        resource->base  = 0x400;
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


static void domain_set_resources(device_t dev)
{
    printk(BIOS_DEBUG, "\nFam14h - domain_set_resources.\n");
    printk(BIOS_DEBUG, "  amsr - incoming dev = %08lx\n",dev);

#if CONFIG_PCI_64BIT_PREF_MEM == 1
    struct resource *io, *mem1, *mem2;
    struct resource *res;
#endif
    unsigned long mmio_basek;
    u32 pci_tolm;
    int i, idx;
    struct bus *link;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
    struct hw_mem_hole_info mem_hole;
    u32 reset_memhole = 1;
#endif

#if CONFIG_PCI_64BIT_PREF_MEM == 1

printk(BIOS_DEBUG, "adsr - CONFIG_PCI_64BIT_PREF_MEM is true.\n");
    for(link = dev->link_list; link; link = link->next) {
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

        printk(BIOS_DEBUG, "base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
            mem1->base, mem1->limit, mem1->size, mem1->align);
        printk(BIOS_DEBUG, "base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
            mem2->base, mem2->limit, mem2->size, mem2->align);
    }

    for(res = &dev->resource_list; res; res = res->next)
    {
        res->flags |= IORESOURCE_ASSIGNED;
        res->flags |= IORESOURCE_STORED;
        report_resource_stored(dev, res, "");
    }
#endif

    pci_tolm = 0xffffffffUL;
    for(link = dev->link_list; link; link = link->next) {
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

    struct dram_base_mask_t d;
    resource_t basek, limitk, sizek; // 4 1T

    d = get_dram_base_mask(0);

    if (d.mask & 1) {
        basek = ((resource_t)(d.base)) << 8;
        limitk = (resource_t)((d.mask << 8) | 0xFFFFFF);
printk(BIOS_DEBUG, "adsr: (before) basek = %llx, limitk = %llx.\n",basek,limitk);

        /* Convert these values to multiples of 1K for ease of math. */
        basek >>= 10;
        limitk >>= 10;
        sizek = limitk - basek + 1;

printk(BIOS_DEBUG, "adsr: (after) basek = %llx, limitk = %llx, sizek = %llx.\n",basek,limitk,sizek);

        /* see if we need a hole from 0xa0000 to 0xbffff */
        if ((basek < 640) && (sizek > 768)) {
printk(BIOS_DEBUG, "adsr - 0xa0000 to 0xbffff resource.\n");
            ram_resource(dev, (idx | 0), basek, 640 - basek);
            idx += 0x10;
            basek = 768;
            sizek = limitk - 768;
        }


printk(BIOS_DEBUG, "adsr: mmio_basek=%08x, basek=%08x, limitk=%08x\n",  mmio_basek, basek, limitk);

        /* split the region to accomodate pci memory space */
        if ( (basek < 4*1024*1024 ) && (limitk > mmio_basek) ) {
            if (basek <= mmio_basek) {
                unsigned pre_sizek;
                pre_sizek = mmio_basek - basek;
                if(pre_sizek>0) {
                    ram_resource(dev, idx, basek, pre_sizek);
                    idx += 0x10;
                    sizek -= pre_sizek;
#if CONFIG_WRITE_HIGH_TABLES==1
                    if (high_tables_base==0) {
                    /* Leave some space for ACPI, PIRQ and MP tables */
#if CONFIG_GFXUMA == 1
                        high_tables_base = uma_memory_base - (HIGH_TABLES_SIZE * 1024);
#else
                        high_tables_base = (mmio_basek - HIGH_TABLES_SIZE) * 1024;
#endif
                        high_tables_size = HIGH_TABLES_SIZE * 1024;
                        printk(BIOS_DEBUG, " split: %dK table at =%08llx\n", HIGH_TABLES_SIZE,
                                 high_tables_base);
                    }
#endif
                }

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

        ram_resource(dev, (idx | 0), basek, sizek);
        idx += 0x10;
#if CONFIG_WRITE_HIGH_TABLES==1
        printk(BIOS_DEBUG, "%d: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
                 0, mmio_basek, basek, limitk);
        if (high_tables_base==0) {
        /* Leave some space for ACPI, PIRQ and MP tables */
#if CONFIG_GFXUMA == 1
            high_tables_base = uma_memory_base - (HIGH_TABLES_SIZE * 1024);
            printk(BIOS_DEBUG, "  adsr - uma_memory_base = %x.\n",uma_memory_base);
#else
            high_tables_base = (limitk - HIGH_TABLES_SIZE) * 1024;
#endif
            high_tables_size = HIGH_TABLES_SIZE * 1024;
        }
#endif
    }
printk(BIOS_DEBUG, "  adsr - mmio_basek = %x.\n",mmio_basek);
printk(BIOS_DEBUG, "  adsr - high_tables_size = %x.\n",high_tables_size);

#if CONFIG_GFXUMA == 1
    printk(BIOS_DEBUG, "adsr - adding uma resource.\n");
    add_uma_resource(dev, 7);
#endif

    for(link = dev->link_list; link; link = link->next) {
        if (link->children) {
            assign_resources(link);
        }
    }
printk(BIOS_DEBUG, "  adsr - leaving this lovely routine.\n");
}


static void domain_enable_resources(device_t dev)
{
  u32 val;
  /* Must be called after PCI enumeration and resource allocation */
  printk(BIOS_DEBUG, "\nFam14h - domain_enable_resources: AmdInitMid.\n");
  val = agesawrapper_amdinitmid (); 
  if(val) {
    printk(BIOS_DEBUG, "agesawrapper_amdinitmid failed: %x \n", val);
  }
  
  printk(BIOS_DEBUG, "  ader - leaving domain_enable_resources.\n");
}


/* Bus related code */


static void cpu_bus_read_resources(device_t dev)
{
    printk(BIOS_DEBUG, "\nFam14h - cpu_bus_read_resources.\n");

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

    printk(BIOS_DEBUG, "\nFam14h - cpu_bus_set_resources.\n");

    if (resource) {
        report_resource_stored(dev, resource, " <mmconfig>");
    }
    pci_dev_set_resources(dev);
}
 
static void cpu_bus_init(device_t dev)
{
	struct device_path cpu_path;
	device_t cpu;
    int apic_id;

    initialize_cpus(dev->link_list);

	/* Build the AP cpu device path(s) */
    for (apic_id = 1; apic_id < CONFIG_MAX_CPUS; apic_id++) {
	    cpu_path.type = DEVICE_PATH_APIC;
	    cpu_path.apic.apic_id = apic_id;
    	cpu = alloc_dev(dev->link_list, &cpu_path);
        if (!cpu) return;
        cpu->enabled = 1;
    	cpu->path.apic.node_id = 0;
    	cpu->path.apic.core_id = apic_id;
    }
}


/* North Bridge Structures */

static struct device_operations northbridge_operations = {
    .read_resources   = read_resources,
    .set_resources    = set_resources,
  .enable_resources = pci_dev_enable_resources,
  .init             = northbridge_init,
  .enable           = 0,
  .ops_pci          = 0,
};


static const struct pci_driver northbridge_driver __pci_driver = {
    .ops = &northbridge_operations,
    .vendor = PCI_VENDOR_ID_AMD,
    .device = 0x1510,
};


struct chip_operations northbridge_amd_agesa_wrapper_family14_ops = {
    CHIP_NAME("AMD Family 14h Northbridge")
    .enable_dev = 0,
};


/* Root Complex Structures */


static struct device_operations pci_domain_ops = {
    .read_resources   = domain_read_resources,
    .set_resources    = domain_set_resources,
    .enable_resources = domain_enable_resources,
    .init             = NULL,
    .scan_bus         = pci_domain_scan_bus,
};


static struct device_operations cpu_bus_ops = {
    .read_resources   = cpu_bus_read_resources,
    .set_resources    = cpu_bus_set_resources,
    .enable_resources = NULL,
    .init             = cpu_bus_init,
    .scan_bus         = 0,
};


static void root_complex_enable_dev(struct device *dev)
{
    /* Set the operations if it is a special bus type */
    if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
        dev->ops = &pci_domain_ops;
    }
    else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
        dev->ops = &cpu_bus_ops;
    }
}


struct chip_operations northbridge_amd_agesa_wrapper_family14_root_complex_ops = {
    CHIP_NAME("AMD Family 14h Root Complex")
    .enable_dev = root_complex_enable_dev,
};
