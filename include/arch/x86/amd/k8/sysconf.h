/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#ifndef AMD_K8_SYSCONF_H
#define AMD_K8_SYSCONF_H

#define HC_POSSIBLE_NUM 8

struct amdk8_sysconf{
	//ht
	unsigned nodes;
	unsigned hc_possible_num;
	unsigned pci1234[HC_POSSIBLE_NUM];
	unsigned hcdn[HC_POSSIBLE_NUM];
	unsigned hcid[HC_POSSIBLE_NUM]; //record ht chain type
	unsigned sbdn;
	unsigned sblk;

	unsigned hcdn_reg[4]; // it will be used by get_sblk_pci1234

	int enabled_apic_ext_id;
	unsigned lift_bsp_apicid;
	int apicid_offset;

	void *mb; // pointer for mb releated struct
	
};

extern struct amdk8_sysconf sysconf;

#endif
