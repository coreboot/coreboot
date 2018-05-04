/*
 * This file is part of the coreboot project.
 *
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
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <cpu/amd/amdfam10_sysconf.h>

extern struct device *get_node_pci(u32 nodeid, u32 fn);

#if 0
static int first_time = 1;
#endif

#include "quadcore_id.c"

static u32 get_max_siblings(u32 nodes)
{
	struct device *dev;
	u32 nodeid;
	u32 siblings = 0;

	//get max siblings from all the nodes
	for (nodeid = 0; nodeid < nodes; nodeid++) {
		int j;
		dev = get_node_pci(nodeid, 3);
		j = (pci_read_config32(dev, 0xe8) >> 12) & 3;
		if (siblings < j)
			siblings = j;
	}

	return siblings;
}


static void enable_apic_ext_id(u32 nodes)
{
	struct device *dev;
	u32 nodeid;

	//enable APIC_EXIT_ID all the nodes
	for (nodeid = 0; nodeid < nodes; nodeid++) {
		u32 val;
		dev = get_node_pci(nodeid, 0);
		val = pci_read_config32(dev, 0x68);
		val |= (1 << 17)|(1 << 18);
		pci_write_config32(dev, 0x68, val);
	}
}


u32 get_apicid_base(u32 ioapic_num)
{
	u32 apicid_base;
	u32 siblings;
	u32 nb_cfg_54;

	u32 disable_siblings = !CONFIG_LOGICAL_CPUS;

	get_option(&disable_siblings, "multi_core");

	siblings = get_max_siblings(sysconf.nodes);

	if (sysconf.bsp_apicid > 0) {
		// IOAPIC could start from 0
		return 0;
	} else if (sysconf.enabled_apic_ext_id) {
		// enabled ext id but bsp = 0
		return 1;
	}

	nb_cfg_54 = read_nb_cfg_54();


	//Construct apicid_base

	if ((!disable_siblings) && (siblings > 0)) {
		/* for 8 way dual core, we will used up apicid 16:16, actually
		   16 is not allowed by current kernel and the kernel will try
		   to get one that is small than 16 to make IOAPIC work. I don't
		   know when the kernel can support 256 APIC id.
		   (APIC_EXT_ID is enabled) */

		//4:10 for two way  8:12 for four way 16:16 for eight way
		//Use CONFIG_MAX_PHYSICAL_CPUS instead of nodes
		//for better consistency?
		apicid_base = nb_cfg_54 ? (siblings+1) * sysconf.nodes :
				8 * siblings + sysconf.nodes;

	} else {
		apicid_base = sysconf.nodes;
	}

	if ((apicid_base+ioapic_num-1) > 0xf) {
		// We need to enable APIC EXT ID
		printk(BIOS_SPEW, "if the IOAPIC device doesn't support 256 APIC id,\n you need to set CONFIG_ENABLE_APIC_EXT_ID in MB Option.lb so you can spare 16 id for IOAPIC\n");
		enable_apic_ext_id(sysconf.nodes);
	}

	return apicid_base;
}
