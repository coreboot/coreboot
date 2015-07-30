/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
#include <pc80/mc146818rtc.h>
#include <northbridge/amd/amdht/ht_wrapper.c>
#if CONFIG_HAVE_OPTION_TABLE
#include "option_table.h"
#endif

#include "cpu/amd/quadcore/quadcore_id.c"

/* get_boot_apic_id and wait_cpu_state located in init_cpus.c */
uint32_t get_boot_apic_id(uint8_t node, uint32_t core);
uint32_t wait_cpu_state(uint32_t apicid, uint32_t state, uint32_t state2);

static u32 get_core_num_in_bsp(u32 nodeid)
{
	u32 dword;
	if (is_fam15h()) {
		/* Family 15h moved CmpCap to F5x84 [7:0] */
		dword = pci_read_config32(NODE_PCI(nodeid, 5), 0x84);
		dword &= 0xff;
	} else {
		dword = pci_read_config32(NODE_PCI(nodeid, 3), 0xe8);
		dword >>= 12;
		/* Bit 15 is CmpCap[2] since Revision D. */
		if ((cpuid_ecx(0x80000008) & 0xff) > 3)
	    	dword = ((dword & 8) >> 1) | (dword & 3);
		else
	    	dword &= 3;
	}
	return dword;
}

static u8 set_apicid_cpuid_lo(void)
{
	// set the NB_CFG[54]=1; why the OS will be happy with that ???
	msr_t msr;
	msr = rdmsr(NB_CFG_MSR);
	msr.hi |= (1<<(54-32)); // InitApicIdCpuIdLo
	wrmsr(NB_CFG_MSR, msr);

	return 1;
}

static void real_start_other_core(uint32_t nodeid, uint32_t cores)
{
	ssize_t i;
	uint32_t dword;

	printk(BIOS_DEBUG, "Start other core - nodeid: %02x  cores: %02x\n", nodeid, cores);

	/* set PCI_DEV(0, 0x18+nodeid, 3), 0x44 bit 27 to redirect all MC4
	   accesses and error logging to core0 */
	dword = pci_read_config32(NODE_PCI(nodeid, 3), 0x44);
	dword |= 1 << 30;	/* SyncFloodOnDramAdrParErr=1 */
	dword |= 1 << 27;	/* NbMcaToMstCpuEn=1 */
	dword |= 1 << 21;	/* SyncFloodOnAnyUcErr=1 */
	dword |= 1 << 20;	/* SyncFloodOnWDT=1 */
	dword |= 1 << 2;	/* SyncFloodOnDramUcEcc=1 */
	pci_write_config32(NODE_PCI(nodeid, 3), 0x44, dword);
	if (is_fam15h()) {
		uint32_t core_activation_flags = 0;
		uint32_t active_cores = 0;

		/* Set PCI_DEV(0, 0x18+nodeid, 0), 0x1dc bits 7:1 to start cores */
		dword = pci_read_config32(NODE_PCI(nodeid, 0), 0x1dc);
		for (i = 1; i < cores + 1; i++) {
			core_activation_flags |= 1 << i;
		}

		/* Start the first core of each compute unit */
		active_cores |= core_activation_flags & 0x55;
		pci_write_config32(NODE_PCI(nodeid, 0), 0x1dc, dword | active_cores);

		/* Each core shares a single set of MTRR registers with
		 * another core in the same compute unit, therefore, it
		 * is important that one core in each CU starts in advance
		 * of the other in order to avoid one core stomping all over
		 * the other core's settings.
		 */

		/* Wait for the first core of each compute unit to start... */
		uint32_t timeout;
		for (i = 1; i < cores + 1; i++) {
			if (!(i & 0x1)) {
				uint32_t ap_apicid = get_boot_apic_id(nodeid, i);
				timeout = wait_cpu_state(ap_apicid, F10_APSTATE_ASLEEP, F10_APSTATE_ASLEEP);
			}
		}

		/* Start the second core of each compute unit */
		active_cores |= core_activation_flags & 0xaa;
		pci_write_config32(NODE_PCI(nodeid, 0), 0x1dc, dword | active_cores);
	} else {
		// set PCI_DEV(0, 0x18+nodeid, 0), 0x68 bit 5 to start core1
		dword = pci_read_config32(NODE_PCI(nodeid, 0), 0x68);
		dword |= 1 << 5;
		pci_write_config32(NODE_PCI(nodeid, 0), 0x68, dword);

		if (cores > 1) {
			dword = pci_read_config32(NODE_PCI(nodeid, 0), 0x168);
			for (i = 0; i < cores - 1; i++) {
				dword |= 1 << i;
			}
			pci_write_config32(NODE_PCI(nodeid, 0), 0x168, dword);
		}
	}
}

#if (!IS_ENABLED(CONFIG_CPU_AMD_MODEL_10XXX))
//it is running on core0 of node0
static void start_other_cores(void)
{
	u32 nodes;
	u32 nodeid;

	// disable multi_core
	if (read_option(multi_core, 0) != 0)  {
		printk(BIOS_DEBUG, "Skip additional core init\n");
		return;
	}

	nodes = get_nodes();

	for (nodeid = 0; nodeid < nodes; nodeid++) {
		u32 cores = get_core_num_in_bsp(nodeid);
		printk(BIOS_DEBUG, "init node: %02x  cores: %02x pass 1\n", nodeid, cores);
		if (cores > 0) {
			real_start_other_core(nodeid, cores);
		}
	}
}
#endif
