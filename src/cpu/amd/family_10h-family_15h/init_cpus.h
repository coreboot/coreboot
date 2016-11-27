/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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

#ifndef INIT_CPUS_H
#define INIT_CPUS_H

#include <stdlib.h>
#include <console/console.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/multicore.h>
#include <reset.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include "defaults.h"

#define NODE_HT(x) NODE_PCI(x,0)
#define NODE_MP(x) NODE_PCI(x,1)
#define NODE_MC(x) NODE_PCI(x,3)
#define NODE_LC(x) NODE_PCI(x,4)

unsigned int get_sbdn(unsigned bus);
void cpuSetAMDMSR(uint8_t node_id);

typedef void (*process_ap_t) (u32 apicid, void *gp);

uint32_t get_boot_apic_id(uint8_t node, uint32_t core);
u32 init_cpus(u32 cpu_init_detectedx, struct sys_info *sysinfo);
uint8_t set_apicid_cpuid_lo(void);
void real_start_other_core(uint32_t nodeid, uint32_t cores);
void finalize_node_setup(struct sys_info *sysinfo);
uint32_t wait_cpu_state(uint32_t apicid, uint32_t state, uint32_t state2);
void start_other_cores(uint32_t bsp_apicid);
u32 get_core_num_in_bsp(u32 nodeid);

void update_microcode(u32 cpu_deviceid);

/* fidvid.c */
void init_fidvid_stage2(u32 apicid, u32 nodeid);
void prep_fid_change(void);
int init_fidvid_bsp(u32 bsp_apicid, u32 nodes);

#endif
