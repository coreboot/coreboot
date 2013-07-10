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

#ifndef AMDFAM10_SYSCONF_H
#define AMDFAM10_SYSCONF_H

#include "northbridge/amd/amdfam10/nums.h"

#include <cpu/x86/msr.h>

struct p_state_t {
	unsigned corefreq;
	unsigned power;
	unsigned transition_lat;
	unsigned busmaster_lat;
	unsigned control;
	unsigned status;
};

struct amdfam10_sysconf_t {
	//ht
	unsigned hc_possible_num;
	unsigned pci1234[HC_POSSIBLE_NUM];
	unsigned hcdn[HC_POSSIBLE_NUM];
	unsigned hcid[HC_POSSIBLE_NUM]; //record ht chain type
	unsigned sbdn;
	unsigned sblk;

	unsigned nodes;
	unsigned ht_c_num; // we only can have 32 ht chain at most
	unsigned ht_c_conf_bus[HC_NUMS]; // 4-->32: 4:segn, 8:bus_max, 8:bus_min, 4:linkn, 6: nodeid, 2: enable
	unsigned io_addr_num;
	unsigned conf_io_addr[HC_NUMS];
	unsigned conf_io_addrx[HC_NUMS];
	unsigned mmio_addr_num;
	unsigned conf_mmio_addr[HC_NUMS*2]; // mem and pref mem
	unsigned conf_mmio_addrx[HC_NUMS*2];
	unsigned segbit;
	unsigned hcdn_reg[HC_NUMS]; // it will be used by get_pci1234

	msr_t msr_pstate[NODE_NUMS * 5]; // quad cores all cores in one node should be the same, and p0,..p5
	unsigned needs_update_pstate_msrs;
	struct p_state_t p_state[NODE_NUMS * 5];
	unsigned p_state_num;

	unsigned bsp_apicid;
	int enabled_apic_ext_id;
	unsigned lift_bsp_apicid;
	int apicid_offset;

	void *mb; // pointer for mb related struct

};

extern struct amdfam10_sysconf_t sysconf;

void get_sblk_pci1234(void);
void get_bus_conf(void);
#endif
