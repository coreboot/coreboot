/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Tyan
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@gmail.com> for Tyan and AMD.
 *
 * Copyright (C) 2007 University of Mannheim
 * Written by Philipp Degler <pdegler@rumms.uni-mannheim.de> for University of Mannheim
 * Copyright (C) 2009 University of Heidelberg
 * Written by Mondrian Nuessle <nuessle@uni-heidelberg.de> for University of Heidelberg
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#if CONFIG_K8_REV_F_SUPPORT
#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0
#endif

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
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "cpu/x86/lapic.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "superio/serverengines/pilot/early_serial.c"
#include "superio/serverengines/pilot/early_init.c"
#include "superio/nsc/pc87417/early_serial.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/debug.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "southbridge/broadcom/bcm5785/early_setup.c"

#define SERIAL_DEV PNP_DEV(0x2e, PILOT_SP1)
#define RTC_DEV PNP_DEV(0x4e, PC87417_RTC)

static void memreset(int controllers, const struct mem_controller *ctrl) { }

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_SWITCH1 0x70
#define SMBUS_SWITCH2 0x72
	 unsigned device = (ctrl->channel0[0]) >> 8;
	 smbus_send_byte(SMBUS_SWITCH1, device & 0x0f);
	 smbus_send_byte(SMBUS_SWITCH2, (device >> 4) & 0x0f );
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	 return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/f.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "lib/generic_sdram.c"
#include <spd.h>
#include "cpu/amd/dualcore/dualcore.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/early_ht.c"

#if 0
#include "ipmi.c"

static void setup_early_ipmi_serial()
{
	unsigned char result;
	char channel_access[]={0x06<<2,0x40,0x04,0x80,0x05};
	char serialmodem_conf[]={0x0c<<2,0x10,0x04,0x08,0x00,0x0f};
	char serial_mux1[]={0x0c<<2,0x12,0x04,0x06};
	char serial_mux2[]={0x0c<<2,0x12,0x04,0x03};
	char serial_mux3[]={0x0c<<2,0x12,0x04,0x07};

//	earlydbg(0x0d);
	//set channel access system only
	ipmi_request(5,channel_access);
//	earlydbg(result);
/*
	//Set serial/modem config
	result=ipmi_request(6,serialmodem_conf);
	earlydbg(result);

	//Set serial mux 1
	result=ipmi_request(4,serial_mux1);
	earlydbg(result);

	//Set serial mux 2
	result=ipmi_request(4,serial_mux2);
	earlydbg(result);

	//Set serial mux 3
	result=ipmi_request(4,serial_mux3);
	earlydbg(result);
*/
//	earlydbg(0x0e);

}
#endif

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
		// first node
		DIMM0, DIMM2, 0, 0,
		DIMM1, DIMM3, 0, 0,
		// second node
		DIMM4, DIMM6, 0, 0,
		DIMM5, DIMM7, 0, 0,
	};

	struct sys_info *sysinfo = &sysinfo_car;
	int needs_reset;
	unsigned bsp_apicid = 0;

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		bcm5785_enable_lpc();
		pc87417_enable_dev(RTC_DEV); /* Enable RTC */
	}

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	pilot_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

//	setup_early_ipmi_serial();
	pilot_early_init(SERIAL_DEV); //config port is being taken from SERIAL_DEV
	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);
	printk(BIOS_DEBUG, "bsp_apicid=%02x\n", bsp_apicid);

#if CONFIG_MEM_TRAIN_SEQ == 1
	set_sysinfo_in_ram(0); // in BSP so could hold all ap until sysinfo is in ram
#endif
	setup_coherent_ht_domain();

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS
	// It is said that we should start core1 after all core0 launched
	/* becase optimize_link_coherent_ht is moved out from setup_coherent_ht_domain,
	 * So here need to make sure last core0 is started, esp for two way system,
	 * (there may be apic id conflicts in that case)
	*/
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	/* it will set up chains and store link pair for optimization later */
	ht_setup_chains_x(sysinfo); // it will init sblnk and sbbusn, nodes, sbdn
	bcm5785_early_setup();

#if CONFIG_SET_FIDVID
	{
		msr_t msr;
		msr=rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "begin msr fid, vid %08x %08x\n", msr.hi, msr.lo);
	}
	enable_fid_change();
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
	init_fidvid_bsp(bsp_apicid);
	// show final fid and vid
	{
		msr_t msr;
		msr=rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "end msr fid, vid %08x %08x\n", msr.hi, msr.lo);
	}
#endif

	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);

	// fidvid change will issue one LDTSTOP and the HT change will be effective too
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
		soft_reset();
	}

	allow_all_aps_stop(bsp_apicid);

	//It's the time to set ctrl in sysinfo now;
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	enable_smbus();

	//do we need apci timer, tsc...., only debug need it for better output
	/* all ap stopped? */
	// init_timer(); // Need to use TMICT to synconize FID/VID

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	post_cache_as_ram();
}
