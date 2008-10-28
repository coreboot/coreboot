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
#include <config.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>

#define RC0 ((1<<0)<<8)

#define DIMM0 0x50
#define DIMM1 0x51

/* this code is very mainboard dependent, sadly. */
/**
 * no op at present
 */
static void memreset_setup(void)
{
}

/**
 * this is a no op on this platform.
 */
void memreset(int controllers, const struct mem_controller *ctrl)
{
}

void activate_spd_rom(const struct mem_controller *ctrl)
{
}

/**
 * read a byte from spd.
 * @param device device to read from
 * @param address address in the spd ROM
 * @return the value of the byte at that address.
 */
u8 spd_read_byte(u16 device, u8 address)
{
	int do_smbus_read_byte(u16 device, u16 address);
	return do_smbus_read_byte(device, address);
}

/**
  * main for initram for the AMD DBM690T
 * @param init_detected Used to indicate that we have been started via init
 * @returns 0 on success
 * The purpose of this code is to not only get ram going, but get any other cpus/cores going.
 * The two activities are very tightly connected and not really seperable.
 *
  */
/*
 * init_detected is used to determine if we did a soft reset as required by a reprogramming of the
 * hypertransport links. If we did this kind of reset, bit 11 will be set in the MTRRdefType_MSR MSR.
 * That may seem crazy, but there are not lots of places to hide a bit when the CPU does a reset.
 * This value is picked up in assembly, or it should be.
 */
int main(void)
{
	/* sure, we could put this in a .h. It's called precisely once, from this one
	 * place. And it only relates to the initram stage. I think I'll leave it here.
	 * That way we can see the definition without grepping the source tree.
	 */
//	void do_enable_smbus(void);
	void enable_fid_change_on_sb(u16 sbbusn, u16 sbdn);
	void soft_reset(void);
	int cpu_init_detected(unsigned int nodeid);
	void rs690_stage1(void);
	void sb600_stage1(void);
	void rs690_before_pci_init(void);
	void sb600_before_pci_init(void);

	u32 init_detected;
	static const u16 spd_addr[] = {
		//first node
		RC0 | DIMM0,
		RC0 | DIMM1,

	};

	struct sys_info *sysinfo;
	int needs_reset;
	unsigned bsp_apicid = 0;
	struct msr msr;
	struct node_core_id me;

	me = get_node_core_id();
	printk(BIOS_DEBUG, "Hi there from stage1, cpu%d, core%d\n", me.nodeid, me.coreid);
	post_code(POST_START_OF_MAIN);
	sysinfo = &(global_vars()->sys_info);
	init_detected = cpu_init_detected(me.nodeid);
	printk(BIOS_DEBUG, "init_detected: %d\n", init_detected);
	/* well, here we are. For starters, we need to know if this is cpu0 core0.
	 * cpu0 core 0 will do all the DRAM setup.
	 */
	bsp_apicid = init_cpus(init_detected, sysinfo);

//	dump_mem(DCACHE_RAM_BASE+DCACHE_RAM_SIZE-0x200, DCACHE_RAM_BASE+DCACHE_RAM_SIZE);

#if 0
	dump_pci_device(PCI_DEV(0, 0x18, 0));
	dump_pci_device(PCI_DEV(0, 0x19, 0));
#endif

	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

	setup_coherent_ht_domain();	// routing table and start other core0

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS==1
	// It is said that we should start core1 after all core0 launched
	/* becase optimize_link_coherent_ht is moved out from setup_coherent_ht_domain,
	 * So here need to make sure last core0 is started, esp for two way system,
	 * (there may be apic id conflicts in that case)
	 */
	start_all_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	/* it will set up chains and store link pair for optimization later */
	ht_setup_chains_x(sysinfo);	// it will init sblnk and sbbusn, nodes, sbdn

	/* run _early_setup before soft-reset. */
	rs690_stage1();
	sb600_stage1();

	msr = rdmsr(FIDVID_STATUS);
	printk(BIOS_DEBUG, "begin msr fid, vid %08x:%08x\n",
	       msr.hi, msr.lo);

	enable_fid_change();

	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

	init_fidvid_bsp(bsp_apicid);

	msr = rdmsr(FIDVID_STATUS);
	printk(BIOS_DEBUG, "begin msr fid, vid %08x:%08x\n",
	       msr.hi, msr.lo);

#if 1
	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

	// fidvid change will issue one LDTSTOP and the HT change will be effective too
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
//		soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
		soft_reset();
	}
#endif
	allow_all_aps_stop(bsp_apicid);

	//It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

//	do_enable_smbus();

	memreset_setup();

	//do we need apci timer, tsc...., only debug need it for better output
	/* all ap stopped? */
//	init_timer(); // Need to use TMICT to synconize FID/VID

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

#if 0
	print_pci_devices();
#endif

#if 0
//	dump_pci_devices();
	dump_pci_device_index_wait(PCI_DEV(0, 0x18, 2), 0x98);
	dump_pci_device_index_wait(PCI_DEV(0, 0x19, 2), 0x98);
#endif

	rs690_before_pci_init();
	sb600_before_pci_init();
	printk(BIOS_DEBUG, "stage1 returns\n");
	return 0;
}
