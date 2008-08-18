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

#include <config.h>
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

static unsigned int cpu_bus_scan(struct device * dev, unsigned int max)
{
	struct bus *cpu_bus;
	struct device * dev_mc;
	int bsp_apicid;
	int i,j;
	unsigned nb_cfg_54;
	unsigned siblings;
	int e0_later_single_core; 
	int disable_siblings;

	nb_cfg_54 = 0;
	sysconf.enabled_apic_ext_id = 0;
	sysconf.lift_bsp_apicid = 0;
	siblings = 0;

	/* Find the bootstrap processors apicid */
	bsp_apicid = lapicid();
	sysconf.apicid_offset = bsp_apicid;

	disable_siblings = !CONFIG_LOGICAL_CPUS;
#if CONFIG_LOGICAL_CPUS == 1
	get_option(&disable_siblings, "dual_core");
#endif

	// for pre_e0, nb_cfg_54 can not be set, ( even set, when you read it still be 0)
	// How can I get the nb_cfg_54 of every node' nb_cfg_54 in bsp??? and differ d0 and e0 single core

	nb_cfg_54 = read_nb_cfg_54();

	dev_mc = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	if (!dev_mc) {
		die("0:18.0 not found?");
	}

	sysconf.nodes = ((pci_read_config32(dev_mc, 0x60)>>4) & 7) + 1;
	

	if (pci_read_config32(dev_mc, 0x68) & (HTTC_APIC_EXT_ID|HTTC_APIC_EXT_BRD_CST))
	{
		sysconf.enabled_apic_ext_id = 1;
		if(bsp_apicid == 0) {
			/* bsp apic id is not changed */
			sysconf.apicid_offset = CONFIG_APIC_ID_OFFSET;
		} else 
		{
			sysconf.lift_bsp_apicid = 1;
		}	
		
	}

	/* Find which cpus are present */
	cpu_bus = &dev->link[0];
	for(i = 0; i < sysconf.nodes; i++) {
		struct device * dev, *cpu;
		struct device_path cpu_path;

		/* Find the cpu's pci device */
		dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));
		if (!dev) {
			/* If I am probing things in a weird order
			 * ensure all of the cpu's pci devices are found.
			 */
			int j;
			struct device * dev_f0;
			for(j = 0; j <= 3; j++) {
				dev = pci_probe_dev(NULL, dev_mc->bus,
					PCI_DEVFN(0x18 + i, j));
			}
			/* Ok, We need to set the links for that device.
			 * otherwise the device under it will not be scanned
			 */
			dev_f0 = dev_find_slot(0, PCI_DEVFN(0x18+i,0));
			if(dev_f0) {
				dev_f0->links = 3;
				for(j=0;j<3;j++) {
					dev_f0->link[j].link = j;
					dev_f0->link[j].dev = dev_f0;
				}
			}

		}

#warning clean this mess up
		e0_later_single_core = 0;
		if (dev && dev->enabled) {
			j = pci_read_config32(dev, 0xe8);
			j = (j >> 12) & 3; // dev is func 3
			printk(BIOS_DEBUG, "  %s siblings=%d\n", dev_path(dev), j);

			if(nb_cfg_54) {
				// For e0 single core if nb_cfg_54 is set, apicid will be 0, 2, 4.... 
				//  ----> you can mixed single core e0 and dual core e0 at any sequence
				// That is the typical case

		                if(j == 0 ){
				       	e0_later_single_core = 1; // we are all f0 or later now. is_cpu_f0_in_bsp(i);  // We can read cpuid(1) from Func3
		                } else {
		                       e0_later_single_core = 0;
               			}
				if(e0_later_single_core) { 
					printk(BIOS_DEBUG, "\tFound Rev E or Rev F later single core\r\n");

					j=1; 
				}
	
				if(siblings > j ) {
				}
				else {
					siblings = j;
				}
			} else {
				siblings = j;
  			}
		}
		
		unsigned jj;
		if(e0_later_single_core || disable_siblings) {
			jj = 0;
		} else 
		{
			jj = siblings;
		}
#if 0	
		jj = 0; // if create cpu core1 path in amd_siblings by core0
#endif
	
                for (j = 0; j <=jj; j++ ) {
		
			/* Build the cpu device path */
			cpu_path.type = DEVICE_PATH_APIC;
			cpu_path.apic.apic_id = i * (nb_cfg_54?(siblings+1):1) + j * (nb_cfg_54?1:8);
			
			/* See if I can find the cpu */
			cpu = find_dev_path(cpu_bus, &cpu_path);
			
			/* Enable the cpu if I have the processor */
			if (dev && dev->enabled) {
				/* this is a CPU */
				struct device_id did;
				did.type = DEVICE_ID_CPU;
#warning fill cpuid; right now it is zero
				if (!cpu) {
					cpu = alloc_dev(cpu_bus, &cpu_path, &did);
				}
				if (cpu) {
					cpu->enabled = 1;
				}
			}
		
			/* Disable the cpu if I don't have the processor */
			if (cpu && (!dev || !dev->enabled)) {
				cpu->enabled = 0;
			}

			/* Report what I have done */
			if (cpu) {
				cpu->path.apic.node_id = i;
				cpu->path.apic.core_id = j;
                                if(sysconf.enabled_apic_ext_id) {
                                	if(sysconf.lift_bsp_apicid) { 
						cpu->path.apic.apic_id += sysconf.apicid_offset;
					} else 
					{
                                               if (cpu->path.apic.apic_id != 0) 
                                                       cpu->path.apic.apic_id += sysconf.apicid_offset;
                                        }
				}
				printk(BIOS_DEBUG, "CPU: %s %s\n",
					dev_path(cpu), cpu->enabled?"enabled":"disabled");
			}

		} //j
	}
	return max;
}

static void cpu_bus_init(struct device * dev)
{
	/* unclear if we need this any more. CPUs are in the device tree. */
	//	initialize_cpus(&dev->link[0]);
	//so what do we do? This is a first for v3. 
}

static void cpu_bus_noop(struct device * dev) 
{
}
