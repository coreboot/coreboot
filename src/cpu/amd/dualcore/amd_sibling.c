/*
 * This file is part of the coreboot project.
 *
 * 2004.12 yhlu add dual core support
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
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/multicore.h>
#include <device/device.h>
#include <device/pci.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/model_fxx_rev.h>
#include <cpu/amd/amdk8_sysconf.h>

static int disable_siblings = !CONFIG_LOGICAL_CPUS;

#include "dualcore_id.c"

static int get_max_siblings(int nodes)
{
	struct device *dev;
	int nodeid;
	int siblings=0;

	//get max siblings from all the nodes
	for (nodeid=0; nodeid<nodes; nodeid++){
		int j;
		dev = dev_find_slot(0, PCI_DEVFN(0x18+nodeid, 3));
		j = (pci_read_config32(dev, 0xe8) >> 12) & 3;
		if (siblings < j) {
			siblings = j;
		}
	}

	return siblings;
}

static void enable_apic_ext_id(int nodes)
{
	struct device *dev;
	int nodeid;

	//enable APIC_EXIT_ID all the nodes
	for (nodeid=0; nodeid<nodes; nodeid++){
		uint32_t val;
		dev = dev_find_slot(0, PCI_DEVFN(0x18+nodeid, 0));
		val = pci_read_config32(dev, 0x68);
		val |= (1 << 17)|(1 << 18);
		pci_write_config32(dev, 0x68, val);
	}
}


unsigned get_apicid_base(unsigned ioapic_num)
{
	struct device *dev;
	int nodes;
	unsigned apicid_base;
	int siblings;
	unsigned nb_cfg_54;
	int bsp_apic_id = lapicid(); // bsp apicid

	get_option(&disable_siblings, "multi_core");

	//get the nodes number
	dev = dev_find_slot(0, PCI_DEVFN(0x18,0));
	nodes = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1;

	siblings = get_max_siblings(nodes);

	if (bsp_apic_id > 0) { // IOAPIC could start from 0
		return 0;
	} else if (pci_read_config32(dev, 0x68) & ( (1 << 17) | (1 << 18)) )  { // enabled ext id but bsp = 0
		return 1;
	}

	nb_cfg_54 = read_nb_cfg_54();

#if 0
	//it is for all e0 single core and nc_cfg_54 low is set, but in the romstage.c stage we do not set that bit for it.
	if (nb_cfg_54 && (!disable_siblings) && (siblings == 0)) {
		//we need to check if e0 single core is there
		int i;
		for (i=0; i<nodes; i++) {
			if (is_e0_later_in_bsp(i)) {
				siblings = 1;
				break;
			}
		}
	}
#endif

	//Construct apicid_base

	if ((!disable_siblings) && (siblings>0) ) {
		/* for 8 way dual core, we will used up apicid 16:16, actually 16 is not allowed by current kernel
		and the kernel will try to get one that is small than 16 to make IOAPIC work.
		I don't know when the kernel can support 256 APIC id. (APIC_EXT_ID is enabled) */

		//4:10 for two way  8:12 for four way 16:16 for eight way
		//Use CONFIG_MAX_PHYSICAL_CPUS instead of nodes for better consistency?
		apicid_base = nb_cfg_54 ? (siblings+1) * nodes :  8 * siblings + nodes;

	}
	else {
		apicid_base = nodes;
	}

	if ((apicid_base+ioapic_num-1)>0xf) {
		// We need to enable APIC EXT ID
		printk(BIOS_INFO, "if the IOAPIC device doesn't support 256 APIC id,\n you need to set CONFIG_ENABLE_APIC_EXT_ID in romstage.c so you can spare 16 id for IOAPIC\n");
		enable_apic_ext_id(nodes);
	}

	return apicid_base;
}
