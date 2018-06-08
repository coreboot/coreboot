/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
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
 */

#define FAM10_SCAN_PCI_BUS 0
#define FAM10_ALLOCATE_IO_RANGE 1

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <timestamp.h>
#include <lib.h>
#include <spd.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <delay.h>
#include <cpu/x86/lapic.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627hf/w83627hf.h>
#include <cpu/x86/bist.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdht/ht_wrapper.h>
#include <cpu/amd/family_10h-family_15h/init_cpus.h>
#include <arch/early_variables.h>
#include <cbmem.h>

#include <southbridge/nvidia/mcp55/mcp55.h> // for enable the FAN

#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"
#include <southbridge/nvidia/mcp55/early_setup_ss.h>
#include "southbridge/nvidia/mcp55/early_setup_car.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)
#define SUPERIO_DEV PNP_DEV(0x2e, 0)

void activate_spd_rom(const struct mem_controller *ctrl);
int spd_read_byte(unsigned device, unsigned address);
extern struct sys_info sysinfo_car;

void activate_spd_rom(const struct mem_controller *ctrl) { }

inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

unsigned get_sbdn(unsigned bus)
{
	pci_devfn_t dev;

	/* Find the device. */
	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_NVIDIA,
				       PCI_DEVICE_ID_NVIDIA_MCP55_HT), bus);

	return (dev >> 15) & 0x1f;
}

static void sio_setup(void)
{
	uint32_t dword;
	uint8_t byte;

	enable_smbus();
	// smbusx_write_byte(1, (0x58 >> 1), 0, 0x80); /* select bank0 */
	/* set FAN ctrl to DC mode */
	smbusx_write_byte(1, (0x58 >> 1), 0xb1, 0xff);

	byte = pci_read_config8(PCI_DEV(0, MCP55_DEVN_BASE + 1, 0), 0x7b);
	byte |= 0x20;
	pci_write_config8(PCI_DEV(0, MCP55_DEVN_BASE + 1, 0), 0x7b, byte);

	dword = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE + 1, 0), 0xa0);
	dword |= (1 << 0);
	pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE + 1, 0), 0xa0, dword);

	dword = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE + 1, 0), 0xa4);
	dword |= (1 << 16);
	pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE + 1, 0), 0xa4, dword);
}

static const u8 spd_addr[] = {
	//first node
	RC00, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
	//second node
	RC00, DIMM4, DIMM6, 0, 0, DIMM5, DIMM7, 0, 0,
#endif
};

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct sys_info *sysinfo = &sysinfo_car;
	u32 bsp_apicid = 0, val, wants_reset;
	msr_t msr;

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		set_bsp_node_CHtExtNodeCfgEn();
		enumerate_ht_chain();
		sio_setup();
	}

	post_code(0x30);

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	post_code(0x32);

	winbond_set_clksel_48(SUPERIO_DEV);
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n", sysinfo, sysinfo + 1);
	printk(BIOS_DEBUG, "bsp_apicid = %02x\n", bsp_apicid);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	/* Setup sysinfo defaults */
	set_sysinfo_in_ram(0);

	update_microcode(val);

	post_code(0x33);

	cpuSetAMDMSR(0);
	post_code(0x34);

	amd_ht_init(sysinfo);
	post_code(0x35);

	/* Setup nodes PCI space and start core 0 AP init. */
	finalize_node_setup(sysinfo);

	/* Setup any mainboard PCI settings etc. */
	setup_mb_resource_map();
	post_code(0x36);

	/* wait for all the APs core0 started by finalize_node_setup. */

	/* FIXME: A bunch of cores are going to start output to serial at once.
	 * It would be nice to fixup prink spinlocks for ROM XIP mode.
	 * I think it could be done by putting the spinlock flag in the cache
	 * of the BSP located right after sysinfo.
	 */

	wait_all_core0_started();
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	/* Core0 on each node is configured. Now setup any additional cores. */
	printk(BIOS_DEBUG, "start_other_cores()\n");
	start_other_cores(bsp_apicid);
	post_code(0x37);
	wait_all_other_cores_started(bsp_apicid);
#endif

	post_code(0x38);

#if IS_ENABLED(CONFIG_SET_FIDVID)
	msr = rdmsr(0xc0010071);
	printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x\n",
		msr.hi, msr.lo);

	/* FIXME: The sb fid change may survive the warm reset and only
	 * need to be done once.*/

	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
	post_code(0x39);

	if (!warm_reset_detect(0)) {	// BSP is node 0
		init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
	} else {
		init_fidvid_stage2(bsp_apicid, 0);	// BSP is node 0
	}

	post_code(0x3A);

	/* show final fid and vid */
	msr = rdmsr(0xc0010071);
	printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x\n",
	       msr.hi, msr.lo);
#endif

	init_timer(); // Need to use TMICT to synchronize FID/VID

	wants_reset = mcp55_early_setup_x();

	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		soft_reset();
		die("After soft_reset - shouldn't see this message!!!\n");
	}

	if (wants_reset)
		printk(BIOS_DEBUG, "mcp55_early_setup_x wants additional reset!\n");

	post_code(0x3B);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl()\n");
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	post_code(0x3D);

	// printk(BIOS_DEBUG, "enable_smbus()\n");
	// enable_smbus(); /* enable in sio_setup */

	post_code(0x40);

	timestamp_add_now(TS_BEFORE_INITRAM);
	printk(BIOS_DEBUG, "raminit_amdmct()\n");
	raminit_amdmct(sysinfo);
	timestamp_add_now(TS_AFTER_INITRAM);

	cbmem_initialize_empty();
	post_code(0x41);

	amdmct_cbmem_store_info(sysinfo);

}

/**
 * BOOL AMD_CB_ManualBUIDSwapList(u8 Node, u8 Link, u8 **List)
 * Description:
 *	This routine is called every time a non-coherent chain is processed.
 *	BUID assignment may be controlled explicitly on a non-coherent chain. Provide a
 *	swap list. The first part of the list controls the BUID assignment and the
 *	second part of the list provides the device to device linking.  Device orientation
 *	can be detected automatically, or explicitly.  See documentation for more details.
 *
 *	Automatic non-coherent init assigns BUIDs starting at 1 and incrementing sequentially
 *	based on each device's unit count.
 *
 * Parameters:
 *	@param[in]  node   = The node on which this chain is located
 *	@param[in]  link   = The link on the host for this chain
 *	@param[out] List   = supply a pointer to a list
 */
BOOL AMD_CB_ManualBUIDSwapList (u8 node, u8 link, const u8 **List)
{
	static const u8 swaplist[] = { 0xFF, CONFIG_HT_CHAIN_UNITID_BASE, CONFIG_HT_CHAIN_END_UNITID_BASE, 0xFF };
	/* If the BUID was adjusted in early_ht we need to do the manual override */
	if ((CONFIG_HT_CHAIN_UNITID_BASE != 0) && (CONFIG_HT_CHAIN_END_UNITID_BASE != 0)) {
		printk(BIOS_DEBUG, "AMD_CB_ManualBUIDSwapList()\n");
		if ((node == 0) && (link == 0)) {	/* BSP SB link */
			*List = swaplist;
			return 1;
		}
	}

	return 0;
}
