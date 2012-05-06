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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define FAM10_SCAN_PCI_BUS 0
#define FAM10_ALLOCATE_IO_RANGE 1

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <lib.h>
#include <spd.h>
#include <cpu/amd/model_10xxx_rev.h>
#include "southbridge/nvidia/mcp55/early_smbus.c" // for enable the FAN
#include "northbridge/amd/amdfam10/raminit.h"
#include "northbridge/amd/amdfam10/amdfam10.h"
#include "cpu/amd/model_10xxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdfam10/reset_test.c"
#include "superio/winbond/w83627hf/early_serial.c"
#include "superio/winbond/w83627hf/early_init.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdfam10/debug.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "northbridge/amd/amdfam10/setup_resource_map.c"
#include "southbridge/nvidia/mcp55/early_ctrl.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)
#define DUMMY_DEV PNP_DEV(0x2e, 0)

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_SWITCH1 0x70
#define SMBUS_SWITCH2 0x72
	smbus_send_byte(SMBUS_SWITCH1, 5 & 0x0f);
	smbus_send_byte(SMBUS_SWITCH2, (5 >> 4) & 0x0f);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdfam10/amdfam10.h"
#include "northbridge/amd/amdfam10/raminit_sysinfo_in_ram.c"
#include "northbridge/amd/amdfam10/pci.c"
#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"
#include "southbridge/nvidia/mcp55/early_setup_ss.h"
#include "southbridge/nvidia/mcp55/early_setup_car.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/microcode/microcode.c"

#if CONFIG_UPDATE_CPU_MICROCODE
#include "cpu/amd/model_10xxx/update_microcode.c"
#endif

#include "cpu/amd/model_10xxx/init_cpus.c"
#include "northbridge/amd/amdfam10/early_ht.c"

static void sio_setup(void)
{
        uint32_t dword;
        uint8_t byte;
        enable_smbus();
//	smbusx_write_byte(1, (0x58>>1), 0, 0x80); /* select bank0 */
	smbusx_write_byte(1, (0x58>>1), 0xb1, 0xff); /* set FAN ctrl to DC mode */

        byte = pci_read_config8(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0x7b);
        byte |= 0x20;
        pci_write_config8(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0x7b, byte);

        dword = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0xa0);
        dword |= (1<<0);
        pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0xa0, dword);

        dword = pci_read_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0xa4);
        dword |= (1<<16);
        pci_write_config32(PCI_DEV(0, MCP55_DEVN_BASE+1 , 0), 0xa4, dword);
}

static const u8 spd_addr[] = {
	//first node
	RC00, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
	//second node
	RC00, DIMM4, DIMM6, 0, 0, DIMM5, DIMM7, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 2
	//third node
	RC02, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	//forth node
	RC03, DIMM4, DIMM6,0 , 0, DIMM5, DIMM7, 0, 0,
#endif
};

#define GPIO1_DEV PNP_DEV(0x2e, W83627HF_GAME_MIDI_GPIO1)
#define GPIO2_DEV PNP_DEV(0x2e, W83627HF_GPIO2)
#define GPIO3_DEV PNP_DEV(0x2e, W83627HF_GPIO3)

static void write_GPIO(void)
{
	pnp_enter_ext_func_mode(GPIO1_DEV);
	pnp_set_logical_device(GPIO1_DEV);
	pnp_write_config(GPIO1_DEV, 0x30, 0x01);
	pnp_write_config(GPIO1_DEV, 0x60, 0x00);
	pnp_write_config(GPIO1_DEV, 0x61, 0x00);
	pnp_write_config(GPIO1_DEV, 0x62, 0x00);
	pnp_write_config(GPIO1_DEV, 0x63, 0x00);
	pnp_write_config(GPIO1_DEV, 0x70, 0x00);
	pnp_write_config(GPIO1_DEV, 0xf0, 0xff);
	pnp_write_config(GPIO1_DEV, 0xf1, 0xff);
	pnp_write_config(GPIO1_DEV, 0xf2, 0x00);
	pnp_exit_ext_func_mode(GPIO1_DEV);

	pnp_enter_ext_func_mode(GPIO2_DEV);
	pnp_set_logical_device(GPIO2_DEV);
	pnp_write_config(GPIO2_DEV, 0x30, 0x01);
	pnp_write_config(GPIO2_DEV, 0xf0, 0xef);
	pnp_write_config(GPIO2_DEV, 0xf1, 0xff);
	pnp_write_config(GPIO2_DEV, 0xf2, 0x00);
	pnp_write_config(GPIO2_DEV, 0xf3, 0x00);
	pnp_write_config(GPIO2_DEV, 0xf5, 0x48);
	pnp_write_config(GPIO2_DEV, 0xf6, 0x00);
	pnp_write_config(GPIO2_DEV, 0xf7, 0xc0);
	pnp_exit_ext_func_mode(GPIO2_DEV);

	pnp_enter_ext_func_mode(GPIO3_DEV);
	pnp_set_logical_device(GPIO3_DEV);
	pnp_write_config(GPIO3_DEV, 0x30, 0x00);
	pnp_write_config(GPIO3_DEV, 0xf0, 0xff);
	pnp_write_config(GPIO3_DEV, 0xf1, 0xff);
	pnp_write_config(GPIO3_DEV, 0xf2, 0xff);
	pnp_write_config(GPIO3_DEV, 0xf3, 0x40);
	pnp_exit_ext_func_mode(GPIO3_DEV);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct sys_info *sysinfo = (struct sys_info *)(CONFIG_DCACHE_RAM_BASE
		+ CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);
	u32 bsp_apicid = 0, val, wants_reset;
	msr_t msr;

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

 	w83627hf_set_clksel_48(DUMMY_DEV);
 	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();
	write_GPIO();
	printk(BIOS_DEBUG, "\n");

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

 val = cpuid_eax(1);
 printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
 printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);
 printk(BIOS_DEBUG, "bsp_apicid = %02x \n", bsp_apicid);
 printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

 /* Setup sysinfo defaults */
 set_sysinfo_in_ram(0);

#if CONFIG_UPDATE_CPU_MICROCODE
 update_microcode(val);
#endif
 post_code(0x33);

 cpuSetAMDMSR();
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
#if CONFIG_LOGICAL_CPUS
 /* Core0 on each node is configured. Now setup any additional cores. */
 printk(BIOS_DEBUG, "start_other_cores()\n");
        start_other_cores();
 post_code(0x37);
        wait_all_other_cores_started(bsp_apicid);
#endif

 post_code(0x38);

#if CONFIG_SET_FIDVID
 msr = rdmsr(0xc0010071);
 printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x \n", msr.hi, msr.lo);

 /* FIXME: The sb fid change may survive the warm reset and only
  * need to be done once.*/

        enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
 post_code(0x39);

 if (!warm_reset_detect(0)) {      // BSP is node 0
   init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
 } else {
   init_fidvid_stage2(bsp_apicid, 0);  // BSP is node 0
        }

 post_code(0x3A);

 /* show final fid and vid */
 msr=rdmsr(0xc0010071);
 printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x \n", msr.hi, msr.lo);
#endif

	init_timer(); // Need to use TMICT to synconize FID/VID

 wants_reset = mcp55_early_setup_x();

 /* Reset for HT, FIDVID, PLL and errata changes to take affect. */
 if (!warm_reset_detect(0)) {
   print_info("...WARM RESET...\n\n\n");
              	soft_reset();
   die("After soft_reset_x - shouldn't see this message!!!\n");
        }

 if (wants_reset)
   printk(BIOS_DEBUG, "mcp55_early_setup_x wanted additional reset!\n");

 post_code(0x3B);

/* It's the time to set ctrl in sysinfo now; */
printk(BIOS_DEBUG, "fill_mem_ctrl()\n");
fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

post_code(0x3D);

//printk(BIOS_DEBUG, "enable_smbus()\n");
//        enable_smbus(); /* enable in sio_setup */

post_code(0x40);

 printk(BIOS_DEBUG, "raminit_amdmct()\n");
 raminit_amdmct(sysinfo);
 post_code(0x41);

 post_cache_as_ram();  // BSP switch stack to ram, copy then execute LB.
 post_code(0x42);  // Should never see this post code.
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
 *	@param[in]  u8  node    = The node on which this chain is located
 *	@param[in]  u8  link    = The link on the host for this chain
 *	@param[out] u8** list   = supply a pointer to a list
 *	@param[out] BOOL result = true to use a manual list
 *				  false to initialize the link automatically
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
