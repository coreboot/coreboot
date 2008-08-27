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

/*
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
#include <cpu/amd/model_fxx_rev.h>
#endif
*/
struct amdk8_sysconf sysconf;

#define FX_DEVS 8
struct device * __f0_dev[FX_DEVS];
struct device * __f1_dev[FX_DEVS];

#if 0
void debug_fx_devs(void)
{
	int i;
	for(i = 0; i < FX_DEVS; i++) {
		struct device * dev;
		dev = __f0_dev[i];
		if (dev) {
			printk(BIOS_DEBUG, "__f0_dev[%d]: %s bus: %p\n",
				i, dev_path(dev), dev->bus);
		}
		dev = __f1_dev[i];
		if (dev) {
			printk(BIOS_DEBUG, "__f1_dev[%d]: %s bus: %p\n",
				i, dev_path(dev), dev->bus);
		}
	}
}
#endif
void get_fx_devs(void)
{
	int i;
	if (__f1_dev[0]) {
		return;
	}
	for(i = 0; i < FX_DEVS; i++) {
		__f0_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 0));
		__f1_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 1));
	}
	if (!__f1_dev[0]) {
		die("Cannot find 0:0x18.1\n");
	}
}

u32 f1_read_config32(unsigned int reg)
{
	get_fx_devs();
	return pci_read_config32(__f1_dev[0], reg);
}

void f1_write_config32(unsigned int reg, u32 value)
{
	int i;
	get_fx_devs();
	for(i = 0; i < FX_DEVS; i++) {
		struct device * dev;
		dev = __f1_dev[i];
		if (dev && dev->enabled) {
			pci_write_config32(dev, reg, value);
		}
	}
}

unsigned int amdk8_nodeid(struct device * dev)
{
	return (dev->path.pci.devfn >> 3) - 0x18;
}


#if CONFIG_HW_MEM_HOLE_SIZEK != 0

struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
		struct hw_mem_hole_info mem_hole;
		int i;

                mem_hole.hole_startk = CONFIG_HW_MEM_HOLE_SIZEK;
		mem_hole.node_id = -1;

                for (i = 0; i < 8; i++) {
                        u32 base;
                        u32 hole;
                        base  = f1_read_config32(0x40 + (i << 3));
                        if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                                continue;
                        }

                        hole = pci_read_config32(__f1_dev[i], 0xf0);
                        if(hole & 1) { // we find the hole 
	                        mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
        	                mem_hole.node_id = i; // record the node No with hole
                	        break; // only one hole
			}
                }

                //We need to double check if there is speical set on base reg and limit reg are not continous instead of hole, it will find out it's hole_startk
                if(mem_hole.node_id==-1) {
                        u32 limitk_pri = 0;
                        for(i=0; i<8; i++) {
                                u32 base, limit;
                                unsigned base_k, limit_k;
                                base  = f1_read_config32(0x40 + (i << 3));
                                if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                                        continue;
                                }

                                base_k = (base & 0xffff0000) >> 2;
                                if(limitk_pri != base_k) { // we find the hole 
	                                mem_hole.hole_startk = limitk_pri;
        	                        mem_hole.node_id = i;
                	                break; //only one hole
				}

	                        limit = f1_read_config32(0x44 + (i << 3));
                	        limit_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
                                limitk_pri = limit_k;
                        }
                }
		
		return mem_hole;
		
}

void disable_hoist_memory(unsigned long hole_startk, int i)
{
        int ii;
        struct device * dev;
        u32 base, limit;
        u32 hoist;
	u32 hole_sizek;


        //1. find which node has hole
        //2. change limit in that node.
        //3. change base and limit in later node
        //4. clear that node f0

	//if there is not mem hole enabled, we need to change it's base instead

	hole_sizek = (4*1024*1024) - hole_startk;

        for(ii=7;ii>i;ii--) {

                base  = f1_read_config32(0x40 + (ii << 3));
                if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                        continue;
                }
		limit = f1_read_config32(0x44 + (ii << 3));
                f1_write_config32(0x44 + (ii << 3),limit - (hole_sizek << 2));
                f1_write_config32(0x40 + (ii << 3),base - (hole_sizek << 2));
        }
        limit = f1_read_config32(0x44 + (i << 3));
        f1_write_config32(0x44 + (i << 3),limit - (hole_sizek << 2));
        dev = __f1_dev[i];
	hoist = pci_read_config32(dev, 0xf0);
	if(hoist & 1) {
		pci_write_config32(dev, 0xf0, 0);
	}
	else {
		base = pci_read_config32(dev, 0x40 + (i << 3));
		f1_write_config32(0x40 + (i << 3),base - (hole_sizek << 2));
	}
		
}

u32 hoist_memory(unsigned long hole_startk, int i)
{
        int ii;
        u32 carry_over;
        struct device * dev;
        u32 base, limit;
        u32 basek;
        u32 hoist;

        carry_over = (4*1024*1024) - hole_startk;

        for(ii=7;ii>i;ii--) {

                base  = f1_read_config32(0x40 + (ii << 3));
                if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                        continue;
                }
		limit = f1_read_config32(0x44 + (ii << 3));
                f1_write_config32(0x44 + (ii << 3),limit + (carry_over << 2));
                f1_write_config32(0x40 + (ii << 3),base + (carry_over << 2));
        }
        limit = f1_read_config32(0x44 + (i << 3));
        f1_write_config32(0x44 + (i << 3),limit + (carry_over << 2));
        dev = __f1_dev[i];
        base  = pci_read_config32(dev, 0x40 + (i << 3));
        basek  = (base & 0xffff0000) >> 2;
	if(basek == hole_startk) {
		//don't need set memhole here, because hole off set will be 0, overflow
		//so need to change base reg instead, new basek will be 4*1024*1024
		base &= 0x0000ffff;
		base |= (4*1024*1024)<<2;
		f1_write_config32(0x40 + (i<<3), base);
	}
	else 
	{
	        hoist = /* hole start address */
        	        ((hole_startk << 10) & 0xff000000) +
                	/* hole address to memory controller address */
	                (((basek + carry_over) >> 6) & 0x0000ff00) +
        	        /* enable */
	                1;
	
        	pci_write_config32(dev, 0xf0, hoist);
	}

        return carry_over;
}
#endif

