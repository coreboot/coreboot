/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#define _MAINOBJECT
#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <cpu.h>
#include <globalvars.h>
#include <amd/k8/k8.h>
#include <southbridge/nvidia/mcp55/mcp55_smbus.h>
#include <mc146818rtc.h>
#include <spd.h>

void hard_reset(void);

void memreset_setup(void)
{
}

void memreset(int controllers, const struct mem_controller *ctrl)
{
}

void activate_spd_rom(const struct mem_controller *ctrl)
{
	/* nothing to do */
}


/** 
  * main for initram for the Gigabyte m57sli.  
  */
int main(void)
{
	void enable_smbus(void);
	void enable_fid_change_on_sb(u16 sbbusn, u16 sbdn);
	void soft_reset_x(unsigned sbbusn, unsigned sbdn);
	int cpu_init_detected(unsigned int nodeid);
	void start_all_cores(void);
	void set_sysinfo_in_ram(unsigned val);
	int mcp55_early_setup_x(void);
	void soft_reset(void);

	struct msr msr;
	static const u16 spd_addr[] = {
		(0xa << 3) | 0, (0xa << 3) | 2, 0, 0,
		(0xa << 3) | 1, (0xa << 3) | 3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
		(0xa << 3) | 4, (0xa << 3) | 6, 0, 0,
		(0xa << 3) | 5, (0xa << 3) | 7, 0, 0,
#endif
	};
	struct sys_info *sysinfo = &(global_vars()->sys_info);

	int needs_reset = 0;
	unsigned bsp_apicid = 0;
	int init_detected;
	struct node_core_id me;

	me = get_node_core_id();
	printk(BIOS_DEBUG, "Hi there from stage1, cpu%d, core%d\n", me.nodeid, me.coreid);

	init_detected = cpu_init_detected(me.nodeid);
	printk(BIOS_DEBUG, "init_detected: %d\n", init_detected);
	/* well, here we are. For starters, we need to know if this is cpu0 core0. 
	 * cpu0 core 0 will do all the DRAM setup. 
	 */
	bsp_apicid = init_cpus(init_detected, sysinfo);

	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

	set_sysinfo_in_ram(0);	// in BSP so could hold all ap until sysinfo is in ram
	setup_coherent_ht_domain();	// routing table and start other core0

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS==1
	// It is said that we should start core1 after all core0 launched
	/* because optimize_link_coherent_ht is moved out from setup_coherent_ht_domain,
	 * So here need to make sure last core0 is started, esp for two way system,
	 * (there may be apic id conflicts in that case)
	 */
	start_all_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	/* it will set up chains and store link pair for optimization later */
	ht_setup_chains_x(sysinfo);	// it will init sblnk and sbbusn, nodes, sbdn

	msr = rdmsr(FIDVID_STATUS);
	printk(BIOS_DEBUG, "begin msr fid, vid %08x:%08x\n", msr.hi, msr.lo);


	enable_fid_change();

	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

	init_fidvid_bsp(bsp_apicid);

	msr = rdmsr(FIDVID_STATUS);
	printk(BIOS_DEBUG, "end msr fid, vid %08x:%08x\n", msr.hi, msr.lo);


	needs_reset |= optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	needs_reset |= mcp55_early_setup_x();

	// fidvid change will issue one LDTSTOP and the HT change will be effective too
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
		soft_reset();
	}
	allow_all_aps_stop(bsp_apicid);

	//It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

	memreset_setup();

	//do we need apci timer, tsc...., only debug need it for better output
	/* all ap stopped? */
//        init_timer(); // Need to use TMICT to synconize FID/VID

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	printk(BIOS_DEBUG, "initram returns\n");
	return 0;
}

