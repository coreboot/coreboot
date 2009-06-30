/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2006 MSI
 * (Written by Bingxun Shi <bingxunshi@gmail.com> for MSI)
 * Copyright (C) 2008 Rudolf Marek <r.marek@assembler.cz> 
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

#define ASSEMBLY 1
#define __ROMCC__

#define RAMINIT_SYSINFO 1

#define CACHE_AS_RAM_ADDRESS_DEBUG 0

unsigned int get_sbdn(unsigned bus);

/* Used by raminit. */
#define QRANK_DIMM_SUPPORT 1

/* Used by init_cpus and fidvid */
#define K8_SET_FIDVID 1

/* If we want to wait for core1 done before DQS training, set it to 0. */
#define K8_SET_FIDVID_CORE0_ONLY 1

#if CONFIG_K8_REV_F_SUPPORT == 1
#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0
#endif

/* #define DEBUG_SMBUS 1 */

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "superio/ite/it8712f/it8712f_early_serial.c"
#include "southbridge/via/vt8237r/vt8237r_early_smbus.c"
#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, IT8712F_SP1)
#define WATCHDOG_DEV PNP_DEV(0x2e, IT8712F_GPIO)

static void memreset_setup(void)
{
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

void activate_spd_rom(const struct mem_controller *ctrl)
{
}

#define K8_4RANK_DIMM_SUPPORT 1


#include "southbridge/via/k8t890/k8t890_early_car.c"
#include "northbridge/amd/amdk8/amdk8.h"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "sdram/generic_sdram.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "cpu/amd/car/copy_and_run.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"

#define SB_VFSMAF 0

/* this function might fail on some K8 CPUs with errata #181 */
static void ldtstop_sb(void)
{
	print_debug("toggle LDTSTP#\r\n");
	u8 reg = inb (VT8237R_ACPI_IO_BASE + 0x5c);
	reg = reg ^ (1 << 0);
	outb(reg, VT8237R_ACPI_IO_BASE + 0x5c);
	reg = inb(VT8237R_ACPI_IO_BASE + 0x15);
	print_debug("done\r\n");
}


#include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/resourcemap.c"

#warning No hard_reset implemented for this board!
void hard_reset(void)
{
	print_info("NO HARD RESET. FIX ME!\n");
}

void soft_reset(void)
{
	uint8_t tmp;

	set_bios_reset();
	print_debug("soft reset \r\n");

	/* PCI reset */
	tmp = pci_read_config8(PCI_DEV(0, 0x11, 0), 0x4f);
	tmp |= 0x01;
	/* FIXME from S3 set bit1 to disable USB reset VT8237A/S */
	pci_write_config8(PCI_DEV(0, 0x11, 0), 0x4f, tmp);

	while (1) {
		/* daisy daisy ... */
		hlt();
	}
}

unsigned int get_sbdn(unsigned bus)
{
	device_t dev;

	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
					PCI_DEVICE_ID_VIA_VT8237R_LPC), bus);
	return (dev >> 15) & 0x1f;
}

void sio_init(void)
{

}

void real_main(unsigned long bist, unsigned long cpu_init_detectedx);

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	real_main(bist, cpu_init_detectedx);
}

void real_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	msr_t msr;
	static const uint16_t spd_addr[] = {
		(0xa << 3) | 0, (0xa << 3) | 2, 0, 0,
		(0xa << 3) | 1, (0xa << 3) | 3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
		(0xa << 3) | 4, (0xa << 3) | 6, 0, 0,
		(0xa << 3) | 5, (0xa << 3) | 7, 0, 0,
#endif
	};
	unsigned bsp_apicid = 0;
	int needs_reset = 0;
	struct sys_info *sysinfo =
	    (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);
	char *p;
	u8 reg;

	sio_init();
	it8712f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	it8712f_kill_watchdog();
	it8712f_enable_3vsbsw();
	uart_init();
	console_init();
	enable_rom_decode();

	print_info("now booting... real_main\r\n");


	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	/* Halt if there was a built in self test failure. */
	report_bist_failure(bist);
	setup_default_resource_map();
	setup_coherent_ht_domain();
	wait_all_core0_started();

	print_info("now booting... Core0 started\r\n");

#if CONFIG_LOGICAL_CPUS==1
	/* It is said that we should start core1 after all core0 launched. */
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif
	init_timer();
	ht_setup_chains_x(sysinfo); /* Init sblnk and sbbusn, nodes, sbdn. */

	needs_reset = optimize_link_coherent_ht();
	print_debug_hex8(needs_reset);
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	print_debug_hex8(needs_reset);
	needs_reset |= k8t890_early_setup_ht();
	print_debug_hex8(needs_reset);

	vt8237_early_network_init(NULL);
	vt8237_early_spi_init();

	if (needs_reset) {
		print_debug_hex8(needs_reset);

		print_debug("Xht reset -\r\n");
		soft_reset();
		print_debug("NO reset\r\n");

	}


	/* the HT settings needs to be OK, because link freq chnage may cause HT disconnect */
	/* allow LDT STOP asserts */
	vt8237_sb_enable_fid_vid();

	enable_fid_change();
	print_debug("after enable_fid_change\r\n");

	init_fidvid_bsp(bsp_apicid);

	/* Stop the APs so we can start them later in init. */
	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl now. */
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	enable_smbus();
	memreset_setup();
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);
	post_cache_as_ram();
}
