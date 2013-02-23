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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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

//called by amd_siblings too
#define CORE_ID_BIT 2
#define NODE_ID_BIT 6
struct node_core_id get_node_core_id(u32 nb_cfg_54)
{
	struct node_core_id id;
	u32 core_id_bits;

	u32 ApicIdCoreIdSize = (cpuid_ecx(0x80000008)>>12 & 0xf);
	if(ApicIdCoreIdSize) {
		core_id_bits = ApicIdCoreIdSize;
	} else {
		core_id_bits = CORE_ID_BIT; //quad core
	}

	// get the apicid via cpuid(1) ebx[31:24]
	if( nb_cfg_54) {
		// when NB_CFG[54] is set, nodeid = ebx[31:26], coreid = ebx[25:24]
		id.coreid = (cpuid_ebx(1) >> 24) & 0xff;
		id.nodeid = (id.coreid>>core_id_bits);
		id.coreid &= ((1<<core_id_bits)-1);
	} else {
		// when NB_CFG[54] is clear, nodeid = ebx[29:24], coreid = ebx[31:30]
		id.nodeid = (cpuid_ebx(1) >> 24) & 0xff;
		id.coreid = (id.nodeid>>NODE_ID_BIT);
		id.nodeid &= ((1<<NODE_ID_BIT)-1);
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

