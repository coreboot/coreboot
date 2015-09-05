/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 * Foundation, Inc.
 */


#include <arch/cpu.h>
#include <cpu/amd/multicore.h>
#ifdef __PRE_RAM__
#include <cpu/amd/model_10xxx_msr.h>
#endif

//called by bus_cpu_scan too
u32 read_nb_cfg_54(void)
{
	msr_t msr;
	msr = rdmsr(NB_CFG_MSR);
	return ( ( msr.hi >> (54-32)) & 1);
}

u32 get_initial_apicid(void)
{
	return ((cpuid_ebx(1) >> 24) & 0xff);
}

/* Called by amd_siblings (ramstage) as well */
struct node_core_id get_node_core_id(u32 nb_cfg_54)
{
	struct node_core_id id;
	uint8_t apicid;
	uint8_t rev_gte_d = 0;
	uint8_t dual_node = 0;
	uint32_t f3xe8;

#ifdef __PRE_RAM__
	f3xe8 = pci_read_config32(NODE_PCI(0, 3), 0xe8);
#else
	f3xe8 = pci_read_config32(get_node_pci(0, 3), 0xe8);
#endif

	if (cpuid_eax(0x80000001) >= 0x8)
		/* Revision D or later */
		rev_gte_d = 1;

	if (rev_gte_d)
		 /* Check for dual node capability */
		if (f3xe8 & 0x20000000)
			dual_node = 1;

	/* Get the apicid via cpuid(1) ebx[31:24]
	 * The apicid format varies based on processor revision
	 */
	apicid = (cpuid_ebx(1) >> 24) & 0xff;
	if( nb_cfg_54) {
		if (rev_gte_d && dual_node) {
			id.coreid = apicid & 0xf;
			id.nodeid = (apicid & 0x30) >> 4;
		} else if (rev_gte_d && !dual_node) {
			id.coreid = apicid & 0x7;
			id.nodeid = (apicid & 0x38) >> 3;
		} else {
			id.coreid = apicid & 0x3;
			id.nodeid = (apicid & 0x1c) >> 2;
		}
	} else {
		if (rev_gte_d && dual_node) {
			id.coreid = (apicid & 0xf0) >> 4;
			id.nodeid = apicid & 0x3;
		} else if (rev_gte_d && !dual_node) {
			id.coreid = (apicid & 0xe0) >> 5;
			id.nodeid = apicid & 0x7;
		} else {
			id.coreid = (apicid & 0x60) >> 5;
			id.nodeid = apicid & 0x7;
		}
	}

	if (rev_gte_d && dual_node) {
		/* Coreboot expects each separate processor die to be on a different nodeid.
		 * Since the code above returns nodeid 0 even on internal node 1 some fixup is needed...
		 */
		uint8_t core_count = (((f3xe8 & 0x00008000) >> 13) | ((f3xe8 & 0x00003000) >> 12)) + 1;

		id.nodeid = id.nodeid * 2;
		if (id.coreid >= core_count) {
			id.nodeid += 1;
			id.coreid = id.coreid - core_count;
		}
	}

	return id;
}

#ifdef UNUSED_CODE
static u32 get_core_num(void)
{
	return (cpuid_ecx(0x80000008) & 0xff);
}
#endif

struct node_core_id get_node_core_id_x(void)
{
	return get_node_core_id(read_nb_cfg_54());
}
