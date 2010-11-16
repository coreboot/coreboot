/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#endif

#define RC0 (6<<8)
#define RC1 (7<<8)

#define DIMM0 0x50
#define DIMM1 0x51

#define SMBUS_HUB 0x71

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>

#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "superio/winbond/w83627dhg/w83627dhg_early_serial.c"
#include <usbdebug.h>

#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#include "northbridge/amd/amdk8/setup_resource_map.c"

#include "southbridge/amd/rs780/rs780_early_setup.c"
#include "southbridge/amd/sb700/sb700_early_setup.c"
#include "northbridge/amd/amdk8/debug.c" /* After sb700_early_setup.c! */

#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)
#define GPIO6_DEV PNP_DEV(0x2e, W83627DHG_GPIO6)
#define GPIO2345_DEV PNP_DEV(0x2e, W83627DHG_GPIO2345)

/* CAN'T BE REMOVED! crt0.S will use it. I don't know WHY!*/
static void memreset(int controllers, const struct mem_controller *ctrl)
{
}

/* called in raminit_f.c */
static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
}

/*called in raminit_f.c */
static inline int spd_read_byte(u32 device, u32 address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/amdk8.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"

#include "cpu/amd/dualcore/dualcore.c"


#include "cpu/amd/car/post_cache_as_ram.c"

#include "cpu/amd/model_fxx/init_cpus.c"

#include "cpu/amd/model_fxx/fidvid.c"

#include "northbridge/amd/amdk8/early_ht.c"

static void sio_init(void)
{
	u8 reg;

	pnp_enter_ext_func_mode(GPIO2345_DEV);
	pnp_set_logical_device(GPIO2345_DEV);

	/* Pin 119 ~ 120 GP21, GP20  */
	reg = pnp_read_config(GPIO2345_DEV, 0x29);
	pnp_write_config(GPIO2345_DEV, 0x29, (reg | 2));

	/* todo document this */
	pnp_write_config(GPIO2345_DEV, 0x2c, 0x1);
	pnp_write_config(GPIO2345_DEV, 0x2d, 0x1);


//idx 30 e0 e1 e2 e3 e4 e5 e6  e7 e8 e9 f0 f1 f2 f3 f4  f5 f6 f7 fe
//val 07 XX XX XX f6 0e 00 00  00 00 ff d6 96 00 40 d0  83 00 00 07

//GPO20 - 1 = 1.82 0 = 1.92 sideport voltage
//mGPUV GPO40 | GPO41 | GPIO23 - 000 - 1.45V step 0.05 -- 111 - 1.10V
//DDR voltage 44 45 46

	/* GPO20 - sideport voltage GPO23 - mgpuV */
	pnp_write_config(GPIO2345_DEV, 0x30, 0x07);	/* Enable GPIO 2,3,4. */
	pnp_write_config(GPIO2345_DEV, 0xe3, 0xf6);	/* dir of GPIO2 11110110*/
	pnp_write_config(GPIO2345_DEV, 0xe4, 0x0e);	/* data */
	pnp_write_config(GPIO2345_DEV, 0xe5, 0x00);	/* No inversion */

	/* GPO30 GPO33 GPO35 */
	//GPO35 - loadline control 0 - enabled
	//GPIO30 - unknown
	//GPIO33 - unknown
	pnp_write_config(GPIO2345_DEV, 0xf0, 0xd6);	/* dir of GPIO3 11010110*/
	pnp_write_config(GPIO2345_DEV, 0xf1, 0x96);	/* data */
	pnp_write_config(GPIO2345_DEV, 0xf2, 0x00);	/* No inversion */

	/* GPO40 GPO41 GPO42 GPO43 PO45 */
	pnp_write_config(GPIO2345_DEV, 0xf4, 0xd0);	/* dir of GPIO4 11010000 */
	pnp_write_config(GPIO2345_DEV, 0xf5, 0x83);	/* data */
	pnp_write_config(GPIO2345_DEV, 0xf6, 0x00);	/* No inversion */

	pnp_write_config(GPIO2345_DEV, 0xf7, 0x00);	/* MFC */
	pnp_write_config(GPIO2345_DEV, 0xf8, 0x00);	/* MFC */
	pnp_write_config(GPIO2345_DEV, 0xfe, 0x07);	/* trig type */
	pnp_exit_ext_func_mode(GPIO2345_DEV);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const u16 spd_addr[] = { DIMM0, 0, 0, 0, DIMM1, 0, 0, 0, };
	int needs_reset = 0;
	u32 bsp_apicid = 0;
	msr_t msr;
	struct cpuid_result cpuid1;
	struct sys_info *sysinfo = (struct sys_info *)(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();

		/* sb700_lpc_port80(); */
		sb700_pci_port80();
	}

	if (bist == 0) {
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);
	}

	enable_rs780_dev8();
	sb700_lpc_init();

	sio_init();
	w83627dhg_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	uart_init();

#if CONFIG_USBDEBUG
	sb700_enable_usbdebug(CONFIG_USBDEBUG_DEFAULT_PORT);
	early_usbdebug_init();
#endif

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	printk(BIOS_DEBUG, "bsp_apicid=0x%x\n", bsp_apicid);

	setup_939a785gmh_resource_map();

	setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS==1
	/* It is said that we should start core1 after all core0 launched */
	wait_all_core0_started();
	start_other_cores();
#endif
	wait_all_aps_started(bsp_apicid);

	ht_setup_chains_x(sysinfo);

	/* run _early_setup before soft-reset. */
	rs780_early_setup();
	sb700_early_setup();

	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	cpuid1 = cpuid(0x80000007);
	if( (cpuid1.edx & 0x6) == 0x6 ) {

		/* Read FIDVID_STATUS */
		msr=rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "begin msr fid, vid: hi=0x%x, lo=0x%x\n", msr.hi, msr.lo);

		enable_fid_change();
		enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);
		init_fidvid_bsp(bsp_apicid);

		/* show final fid and vid */
		msr=rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "end msr fid, vid: hi=0x%x, lo=0x%x\n", msr.hi, msr.lo);

	} else {
		printk(BIOS_DEBUG, "Changing FIDVID not supported\n");
	}

	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	rs780_htinit();
	printk(BIOS_DEBUG, "needs_reset=0x%x\n", needs_reset);

	if (needs_reset) {
		print_info("ht reset -\n");
		soft_reset();
	}

	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl now; */
	printk(BIOS_DEBUG, "sysinfo->nodes: %2x  sysinfo->ctrl: %p  spd_addr: %p\n",
		     sysinfo->nodes, sysinfo->ctrl, spd_addr);
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	rs780_before_pci_init();
	sb700_before_pci_init();

	post_cache_as_ram();
}

