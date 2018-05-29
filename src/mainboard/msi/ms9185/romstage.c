/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Tyan
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@gmail.com> for Tyan and AMD.
 *
 * Copyright (C) 2006 MSI
 * Written by bxshi <bingxunshi@gmail.com> for MSI.
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

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include "southbridge/broadcom/bcm5785/early_smbus.c"
#include <northbridge/amd/amdk8/raminit.h>
#include <delay.h>
#include <reset.h>
#include <cpu/x86/lapic.h>

#include <superio/nsc/pc87417/pc87417.h>
#include <cpu/amd/car.h>
#include <cpu/x86/bist.h>
#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, PC87417_SP1)
#define RTC_DEV PNP_DEV(0x2e, PC87417_RTC)

unsigned get_sbdn(unsigned bus);

void memreset(int controllers, const struct mem_controller *ctrl) { }

void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_SWITCH1 0x70
#define SMBUS_SWITCH2 0x72
	unsigned device = (ctrl->channel0[0]) >> 8;
	smbus_send_byte(SMBUS_SWITCH1, device & 0x0f);
	smbus_send_byte(SMBUS_SWITCH2, (device >> 4) & 0x0f );
}

int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "southbridge/broadcom/bcm5785/early_setup.c"
#include <northbridge/amd/amdk8/f.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/early_ht.c"

#define RC0 (0x10 << 8)
#define RC1 (0x01 << 8)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
		//first node
		RC0|DIMM0, RC0|DIMM2, RC0|DIMM4, RC0|DIMM6,
		RC0|DIMM1, RC0|DIMM3, RC0|DIMM5, RC0|DIMM7,
		//second node
		RC1|DIMM0, RC1|DIMM2, RC1|DIMM4, RC1|DIMM6,
		RC1|DIMM1, RC1|DIMM3, RC1|DIMM5, RC1|DIMM7,
	};

	struct sys_info *sysinfo = &sysinfo_car;

	int needs_reset;
	unsigned bsp_apicid = 0;

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		bcm5785_enable_lpc();
		//enable RTC
		pc87417_enable_dev(RTC_DEV);
	}

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	pc87417_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);

	setup_ms9185_resource_map();

	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

	setup_coherent_ht_domain();

	wait_all_core0_started();
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	// It is said that we should start core1 after all core0 launched
	/* becase optimize_link_coherent_ht is moved out from setup_coherent_ht_domain,
	* So here need to make sure last core0 is started, esp for two way system,
	* (there may be apic id conflicts in that case)
	*/
	start_other_cores();
//bx_a010-     wait_all_other_cores_started(bsp_apicid);
#endif

	/* it will set up chains and store link pair for optimization later */
	ht_setup_chains_x(sysinfo); // it will init sblnk and sbbusn, nodes, sbdn

	bcm5785_early_setup();

#if IS_ENABLED(CONFIG_SET_FIDVID)
	{
		msr_t msr;
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "begin msr fid, vid %08x%08x\n", msr.hi, msr.lo);
	}
	enable_fid_change();
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
	init_fidvid_bsp(bsp_apicid);
	// show final fid and vid
	{
		msr_t msr;
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "end   msr fid, vid %08x%08x\n", msr.hi, msr.lo);
	}
#endif

#if 1
	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

	// fidvid change will issue one LDTSTOP and the HT change will be effective too
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
		soft_reset();
	}
#endif
	allow_all_aps_stop(bsp_apicid);

	//It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

	//do we need apci timer, tsc...., only debug need it for better output
	/* all ap stopped? */
//        init_timer(); // Need to use TMICT to synchronize FID/VID

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

}
