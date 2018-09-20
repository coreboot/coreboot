/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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

#define SYSTEM_TYPE 0	/* SERVER */

/* used by incoherent_ht */
#define FAM10_SCAN_PCI_BUS 0
#define FAM10_ALLOCATE_IO_RANGE 0

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <timestamp.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <lib.h>
#include <spd.h>
#include <cpu/x86/lapic.h>
#include <commonlib/loglevel.h>
#include <cpu/x86/bist.h>
#include <cpu/amd/car.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627hf/w83627hf.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdht/ht_wrapper.h>
#include <cpu/amd/family_10h-family_15h/init_cpus.h>
#include <arch/early_variables.h>
#include <cbmem.h>
#include "southbridge/amd/amd8111/early_smbus.c"
#include "southbridge/amd/amd8111/early_ctrl.c"

#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

void activate_spd_rom(const struct mem_controller *ctrl);
int spd_read_byte(unsigned int device, unsigned int address);
extern struct sys_info sysinfo_car;

static void memreset_setup(void)
{
	/* GPIO on amd8111 to enable MEMRST ???? */
	outb((1 << 2)|(1 << 0), SMBUS_IO_BASE + 0xc0 + 16);	/* REVC_MEMRST_EN = 1 */
	outb((1 << 2)|(0 << 0), SMBUS_IO_BASE + 0xc0 + 17);
}

void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x18
	int ret,i;
	u8 device = ctrl->spd_switch_addr;

	printk(BIOS_DEBUG, "switch i2c to : %02x for node %02x\n", device, ctrl->node_id);

	/* the very first write always get COL_STS = 1 and ABRT_STS = 1, so try another time*/
	i = 2;
	do {
		ret = smbus_write_byte(SMBUS_HUB, 0x01, (1<<(device & 0x7)));
	} while ((ret != 0) && (i-->0));
	smbus_write_byte(SMBUS_HUB, 0x03, 0);
}

int spd_read_byte(u32 device, u32 address)
{
	return smbus_read_byte(device, address);
}

static const u8 spd_addr[] = {
	/* first node */
	RC00, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
	/* second node */
	RC01, DIMM0, DIMM2, DIMM4, DIMM6, DIMM1, DIMM3, DIMM5, DIMM7,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 2
	/* third node */
	RC02, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	/* forth node */
	RC03, DIMM0, DIMM2, DIMM4, DIMM6, DIMM1, DIMM3, DIMM5, DIMM7,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 4
	RC04, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC05, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 6
	RC06, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC07, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 8
	RC08, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC09, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC10, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC11, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 12
	RC12, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC13, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC14, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC15, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 16
	RC16, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC17, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC18, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC19, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 20
	RC20, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC21, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC22, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC23, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 24
	RC24, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC25, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC26, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC27, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC28, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC29, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC30, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC31, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 32
	RC32, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC33, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC34, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC35, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC36, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC37, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC38, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC39, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC40, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC41, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC42, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC43, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC44, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC45, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC46, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC47, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 48
	RC48, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC49, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC50, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC51, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC52, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC53, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC54, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC55, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC56, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC57, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC58, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC59, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC60, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC61, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC62, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC63, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
};

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct sys_info *sysinfo = &sysinfo_car;
	u32 bsp_apicid = 0, val;
	msr_t msr;

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		/* mov bsp to bus 0xff when > 8 nodes */
		set_bsp_node_CHtExtNodeCfgEn();
		enumerate_ht_chain();
	}

	post_code(0x30);

	if (bist == 0) {
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo); /* mmconf is inited in init_cpus */
		/* All cores run this but the BSP(node0,core0) is the only core that returns. */
	}

	post_code(0x32);

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);
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
	   It would be nice to fixup prink spinlocks for ROM XIP mode.
	   I think it could be done by putting the spinlock flag in the cache
	   of the BSP located right after sysinfo.
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
	printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);

	/* FIXME: The sb fid change may survive the warm reset and only
	   need to be done once.*/
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

	post_code(0x39);

	if (!warm_reset_detect(0)) {			/* BSP is node 0 */
		init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
	} else {
		init_fidvid_stage2(bsp_apicid, 0);	/* BSP is node 0 */
	}

	post_code(0x3A);

	/* show final fid and vid */
	msr = rdmsr(0xc0010071);
	printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x\n", msr.hi, msr.lo);
 #endif

	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		soft_reset_x(sysinfo->sbbusn, sysinfo->sbdn);
		die("After soft_reset_x - shouldn't see this message!!!\n");
	}

	post_code(0x3B);

	/* FIXME:  Move this to chipset init.
	enable cf9 for hard reset */
	printk(BIOS_DEBUG, "enable_cf9_x()\n");
	enable_cf9_x(sysinfo->sbbusn, sysinfo->sbdn);
	post_code(0x3C);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl()\n");
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	post_code(0x3D);

	printk(BIOS_DEBUG, "enable_smbus()\n");
	enable_smbus();
	post_code(0x3E);

	memreset_setup();
	post_code(0x40);

	timestamp_add_now(TS_BEFORE_INITRAM);
	printk(BIOS_DEBUG, "raminit_amdmct()\n");
	raminit_amdmct(sysinfo);
	timestamp_add_now(TS_AFTER_INITRAM);

	cbmem_initialize_empty();
	post_code(0x41);

	amdmct_cbmem_store_info(sysinfo);

	post_code(0x42);
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
