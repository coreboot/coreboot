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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#ifndef SET_NB_CFG_54
	#define SET_NB_CFG_54 1
#endif

#include "cpu/amd/quadcore/quadcore_id.c"

static inline u32 get_core_num_in_bsp(u32 nodeid)
{
	u32 dword;
	dword = pci_read_config32(NODE_PCI(nodeid, 3), 0xe8);
	dword >>= 12;
	dword &= 3;
	return dword;
}

#if SET_NB_CFG_54 == 1
static inline u8 set_apicid_cpuid_lo(void)
{
	// set the NB_CFG[54]=1; why the OS will be happy with that ???
	msr_t msr;
	msr = rdmsr(NB_CFG_MSR);
	msr.hi |= (1<<(54-32)); // InitApicIdCpuIdLo
	wrmsr(NB_CFG_MSR, msr);

	return 1;
}
#else

static inline void set_apicid_cpuid_lo(void) { }

#endif


static inline void real_start_other_core(u32 nodeid, u32 cores)
{
	u32 dword;

	printk_debug("Start other core - nodeid: %02x  cores: %02x\n", nodeid, cores);

	/* set PCI_DEV(0, 0x18+nodeid, 3), 0x44 bit 27 to redirect all MC4
	   accesses and error logging to core0 */
	dword = pci_read_config32(NODE_PCI(nodeid, 3), 0x44);
	dword |= 1 << 27;	// NbMcaToMstCpuEn bit
	pci_write_config32(NODE_PCI(nodeid, 3), 0x44, dword);
	// set PCI_DEV(0, 0x18+nodeid, 0), 0x68 bit 5 to start core1
	dword = pci_read_config32(NODE_PCI(nodeid, 0), 0x68);
	dword |= 1 << 5;
	pci_write_config32(NODE_PCI(nodeid, 0), 0x68, dword);

	if(cores > 1) {
		dword = pci_read_config32(NODE_PCI(nodeid, 0), 0x168);
		dword |= (1 << 0);	// core2
		if(cores > 2) {		// core3
			dword |= (1 << 1);
		}
		pci_write_config32(NODE_PCI(nodeid, 0), 0x168, dword);
	}
}

//it is running on core0 of node0
static inline void start_other_cores(void)
{
	u32 nodes;
	u32 nodeid;

	// disable quad_core
	if (read_option(CMOS_VSTART_quad_core, CMOS_VLEN_quad_core, 0) != 0)  {
		printk_debug("Skip additional core init\n");
		return;
	}

	nodes = get_nodes();

	for (nodeid = 0; nodeid < nodes; nodeid++) {
		u32 cores = get_core_num_in_bsp(nodeid);
		printk_debug("init node: %02x  cores: %02x \n", nodeid, cores);
		if (cores > 0) {
			real_start_other_core(nodeid, cores);
		}
	}

}
