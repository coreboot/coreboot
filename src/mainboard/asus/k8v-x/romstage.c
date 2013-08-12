/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2006 MSI
 * (Written by Bingxun Shi <bingxunshi@gmail.com> for MSI)
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

unsigned int get_sbdn(unsigned bus);

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
#include "cpu/x86/lapic.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "superio/winbond/w83697hf/early_serial.c"
#include "southbridge/via/vt8237r/early_smbus.c"
#include "northbridge/amd/amdk8/debug.c" /* After vt8237r/early_smbus.c! */
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83697HF_SP1)

static void memreset(int controllers, const struct mem_controller *ctrl) { }
static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include <reset.h>
void soft_reset(void)
{
	uint8_t tmp;

	set_bios_reset();
	print_debug("soft reset \n");

	/* PCI reset */
	tmp = pci_read_config8(PCI_DEV(0, 0x11, 0), 0x4f);
	tmp |= 0x01;
	pci_write_config8(PCI_DEV(0, 0x11, 0), 0x4f, tmp);

	while (1) {
		/* daisy daisy ... */
		hlt();
	}
}

#include "southbridge/via/k8t890/early_car.c"
#include "northbridge/amd/amdk8/amdk8.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/resourcemap.c"

unsigned int get_sbdn(unsigned bus)
{
	device_t dev;

	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
					PCI_DEVICE_ID_VIA_VT8237R_LPC), bus);
	return (dev >> 15) & 0x1f;
}

static void sio_init(void)
{
	u8 reg;

	pnp_enter_ext_func_mode(SERIAL_DEV);
	reg = pnp_read_config(SERIAL_DEV, 0x24);
	/* 4 Mbit flash */
	reg = (reg & ~0x30) | 0x20;
	/* We have 24MHz input. */
	reg &= ~0x40;
	/* enable MEMW#, so flash can be written */
	reg |= 0x08;
	pnp_write_config(SERIAL_DEV, 0x24, reg);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
		// Node 0
		DIMM0, DIMM1, DIMM2, 0,
		0, 0, 0, 0,
		// Node 1
		0, 0, 0, 0,
		0, 0, 0, 0,
	};
	unsigned bsp_apicid = 0;
	int needs_reset = 0;
	struct sys_info *sysinfo = &sysinfo_car;

	sio_init();
	w83697hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	enable_rom_decode();

	print_info("now booting... fallback\n");

	/* Is this a CPU only reset? Or is this a secondary CPU? */
	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0. */
		/* Allow the HT devices to be found. */
		enumerate_ht_chain();
	}

	// FIXME why is this executed again? --->
	sio_init();
	w83697hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	enable_rom_decode();
	// <--- FIXME why is this executed again?

	print_info("now booting... real_main\n");

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	/* Halt if there was a built in self test failure. */
	report_bist_failure(bist);

	setup_default_resource_map();
	setup_coherent_ht_domain();
	wait_all_core0_started();

	print_info("now booting... Core0 started\n");

#if CONFIG_LOGICAL_CPUS
	/* It is said that we should start core1 after all core0 launched. */
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif
	init_timer();
	ht_setup_chains_x(sysinfo); /* Init sblnk and sbbusn, nodes, sbdn. */

	needs_reset = optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	needs_reset |= k8t890_early_setup_ht();

	if (needs_reset) {
		print_debug("ht reset -\n");
		soft_reset();
	}

	/* the HT settings needs to be OK, because link freq change may cause HT disconnect */
	vt8237_sb_enable_fid_vid();
	enable_fid_change();
	init_fidvid_bsp(bsp_apicid);

	/* Stop the APs so we can start them later in init. */
	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl now. */
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

	/* this seems to be some GPIO on the SMBus--in any case, setting these
	 * two bits reduces the pullup impedance of the bus lines and is required
	 * in order to be able to read SPD info */
	smbus_write_byte(0x48, 0x07, smbus_read_byte(0x48, 0x07) | 0x80);
	smbus_write_byte(0x4a, 0x07, smbus_read_byte(0x4a, 0x07) | 0x10);

	unsigned char mask;

	mask = 0;
//	mask |= 1 /* AGP voltage 1.7 V (not verified, just vendor BIOS value) */
//	mask |= 2 /* V-Link voltage 2.6 V (not verified either) */
	smbus_write_byte(0x4a, 0x00, (smbus_read_byte(0x4a, 0x00) & ~0x0f) | (0x0f ^ (mask << 2)));
	smbus_write_byte(0x4a, 0x01, (smbus_read_byte(0x4a, 0x01) & ~0x03) | (0x03 ^ mask));

	mask = 25; /* RAM voltage in decivolts, valid range from 25 to 28 */
	mask = 3 - (mask - 25);
	smbus_write_byte(0x4a, 0x02, 0x4f | (mask << 4));
	smbus_write_byte(0x4a, 0x03, 0x04 | mask);

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);
	post_cache_as_ram();
}
