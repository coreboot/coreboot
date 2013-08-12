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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define RC0 (6<<8)
#define RC1 (7<<8)

#define SMBUS_HUB 0x71

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include <spd.h>
#include "cpu/x86/lapic.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "superio/winbond/w83627dhg/w83627dhg.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "southbridge/amd/rs780/early_setup.c"
#include "southbridge/amd/sb700/sb700.h"
#include "southbridge/amd/sb700/smbus.h"
#include "northbridge/amd/amdk8/debug.c" /* After sb700/early_setup.c! */

#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)
#define GPIO2345_DEV PNP_DEV(0x2e, W83627DHG_GPIO2345_V)

static void memreset(int controllers, const struct mem_controller *ctrl) { }
static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(u32 device, u32 address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
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

	/* Pin 119 ~ 120 is GP21, GP20  */
	reg = pnp_read_config(GPIO2345_DEV, 0x29);
	pnp_write_config(GPIO2345_DEV, 0x29, (reg | 2));

	/* Turn on the Power LED ("Suspend LED" in Super I/O) */
	reg = pnp_read_config(GPIO2345_DEV, 0xf3);
	pnp_write_config(GPIO2345_DEV, 0xf3, (reg | 0x40));

	/* todo document this */
	pnp_write_config(GPIO2345_DEV, 0x2c, 0x1);
	pnp_write_config(GPIO2345_DEV, 0x2d, 0x1);

	/* GPO20 - sideport voltage 1 = 1.82 0 = 1.92
	   GPI21 - unknown input (NC?)
	   GPI22 - unknown input (NC?)
	   GPO23 - mgpuV bit0
	   GP24-27 - PS/2 mouse/keyb (only keyb is connected use flip interface for mouse)
	*/
	pnp_write_config(GPIO2345_DEV, 0x30, 0x07);	/* Enable GPIO 2,3,4. */
	pnp_write_config(GPIO2345_DEV, 0xe3, 0xf6);	/* dir of GPIO2 11110110*/
	pnp_write_config(GPIO2345_DEV, 0xe4, 0x0e);	/* data */
	pnp_write_config(GPIO2345_DEV, 0xe5, 0x00);	/* No inversion */

	/* GPIO30 - unknown output, set to 0
	   GPI31 - unknown input NC?
	   GPI32 - unknown input NC?
	   GPIO33 - unknown output, set to 0.
	   GPI34 - unknown input NC?
	   GPO35 - loadline control 1 = enabled (2 phase clock) 0 = disabled 4 phase clock
	   GPIO36 - input = HT voltage 1.30V output (low) = HT voltage 1.35V
	   GP37 - unknown input NC? */

	pnp_write_config(GPIO2345_DEV, 0xf0, 0xd6);	/* dir of GPIO3 11010110*/
	pnp_write_config(GPIO2345_DEV, 0xf1, 0x96);	/* data */
	pnp_write_config(GPIO2345_DEV, 0xf2, 0x00);	/* No inversion */

	/* GPO40 - mgpuV bit2
	   GPO41 - mgpuV bit1
	   GPO42  - IRTX
	   GPO43  - IRRX
	   GPIO44 - memory voltage bit2 (input/outputlow)
	   GPIO45 - memory voltage bit1 (2.60 (000) - 2.95 (111))
	   GPIO46 - memory voltage bit0
	   GPIO47 - unknown input? */

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
	static const u16 spd_addr[] = { DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0, };
	int needs_reset = 0;
	u32 bsp_apicid = 0;
	msr_t msr;
	struct cpuid_result cpuid1;
	struct sys_info *sysinfo = &sysinfo_car;

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		/* sb7xx_51xx_lpc_port80(); */
		sb7xx_51xx_pci_port80();
	}

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	enable_rs780_dev8();
	sb7xx_51xx_lpc_init();

	sio_init();
	w83627dhg_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	printk(BIOS_DEBUG, "bsp_apicid=0x%x\n", bsp_apicid);

	setup_939a785gmh_resource_map();

	setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS
	/* It is said that we should start core1 after all core0 launched */
	wait_all_core0_started();
	start_other_cores();
#endif
	wait_all_aps_started(bsp_apicid);

	ht_setup_chains_x(sysinfo);

	/* run _early_setup before soft-reset. */
	rs780_early_setup();
	sb7xx_51xx_early_setup();

	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x6) == 0x6) {
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
	sb7xx_51xx_before_pci_init();

	post_cache_as_ram();
}
