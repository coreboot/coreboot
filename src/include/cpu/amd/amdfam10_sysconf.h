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

#ifndef AMDFAM10_SYSCONF_H
#define AMDFAM10_SYSCONF_H

#include "northbridge/amd/amdfam10/nums.h"

#include <cpu/x86/msr.h>

struct p_state_t {
	unsigned int corefreq;
	unsigned int power;
	unsigned int transition_lat;
	unsigned int busmaster_lat;
	unsigned int control;
	unsigned int status;
};

struct amdfam10_sysconf_t {
	//ht
	unsigned int hc_possible_num;
	unsigned int pci1234[HC_POSSIBLE_NUM];
	unsigned int hcdn[HC_POSSIBLE_NUM];
	unsigned int hcid[HC_POSSIBLE_NUM]; //record ht chain type
	unsigned int sbdn;
	unsigned int sblk;

	unsigned int nodes;
	unsigned int ht_c_num; // we only can have 32 ht chain at most
	unsigned int ht_c_conf_bus[HC_NUMS]; // 4-->32: 4:segn, 8:bus_max, 8:bus_min, 4:linkn, 6: nodeid, 2: enable
	unsigned int io_addr_num;
	unsigned int conf_io_addr[HC_NUMS];
	unsigned int conf_io_addrx[HC_NUMS];
	unsigned int mmio_addr_num;
	unsigned int conf_mmio_addr[HC_NUMS*2]; // mem and pref mem
	unsigned int conf_mmio_addrx[HC_NUMS*2];
	unsigned int segbit;
	unsigned int hcdn_reg[HC_NUMS]; // it will be used by get_pci1234

	msr_t msr_pstate[NODE_NUMS * 5]; // quad cores all cores in one node should be the same, and p0,..p5
	unsigned int needs_update_pstate_msrs;

	unsigned int bsp_apicid;
	int enabled_apic_ext_id;
	unsigned int lift_bsp_apicid;
	int apicid_offset;

	void *mb; // pointer for mb related struct

};

extern struct amdfam10_sysconf_t sysconf;

void get_sblk_pci1234(void);
void get_bus_conf(void);
#endif
