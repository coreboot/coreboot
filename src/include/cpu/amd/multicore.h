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

#ifndef CPU_AMD_QUADCORE_H
#define CPU_AMD_QUADCORE_H

u32 read_nb_cfg_54(void);

struct node_core_id {
	u32 nodeid;
	u32 coreid;
};

// it can be used to get unitid and coreid it running only
struct node_core_id get_node_core_id(u32 nb_cfg_54);
struct node_core_id get_node_core_id_x(void);

#if !defined(__PRE_RAM__)
struct device;
u32 get_apicid_base(u32 ioapic_num);
void amd_sibling_init(struct device *cpu);
#else
void wait_all_core0_started(void);
void wait_all_other_cores_started(u32 bsp_apicid);
void wait_all_aps_started(u32 bsp_apicid);
void allow_all_aps_stop(u32 bsp_apicid);
#endif
u32 get_initial_apicid(void);

#endif /* CPU_AMD_QUADCORE_H */
