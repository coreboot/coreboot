/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
 * Copyright (C) 2008 Jonathan A. Kollasch <jakllsch@kollasch.net>
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

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include "cpu/x86/lapic.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "superio/winbond/w83627thg/early_serial.c"
#include <cpu/amd/model_fxx_rev.h>
#include <console/console.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/nvidia/ck804/early_smbus.h"
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "northbridge/amd/amdk8/debug.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <spd.h>

#if CONFIG_HAVE_OPTION_TABLE
#include "option_table.h"
#endif

#define SERIAL_DEV PNP_DEV(0x4e, W83627THG_SP1)

static void memreset(int controllers, const struct mem_controller *ctrl) { }
static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"
#include "lib/generic_sdram.c"
#include "southbridge/nvidia/ck804/early_setup_ss.h"
#include "southbridge/nvidia/ck804/early_setup_car.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "northbridge/amd/amdk8/early_ht.c"

static void ms7135_set_ram_voltage(void)
{
	u8 b;
	b = read_option(ram_voltage, 0);
	if (b > 4) /* default if above 2.70v */
		b = 0;
	printk(BIOS_INFO, "setting RAM voltage %08x\n", b);
	ck804_smbus_write_byte(1, 0x2f, 0x00, b);
}

static void ms7135_set_nf4_voltage(void)
{
	u8 b;
	b = read_option(nf4_voltage, 0);
	if (b > 2) /* default if above 1.60v */
		b = 0;
	b |= 0x10;
	printk(BIOS_INFO, "setting NF4 voltage %08x\n", b);
	ck804_smbus_write_byte(1, 0x2f, 0x02, b);
}

static void sio_setup(void)
{
	u32 dword;
	u8 byte;

	/* Subject decoding */
	byte = pci_read_config8(PCI_DEV(0, CK804_DEVN_BASE + 1, 0), 0x7b);
	byte |= 0x20;
	pci_write_config8(PCI_DEV(0, CK804_DEVN_BASE + 1, 0), 0x7b, byte);

	/* LPC Positive Decode 0 */
	dword = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE + 1, 0), 0xa0);
	/* Serial 0, Serial 1 */
	dword |= (1 << 0) | (1 << 1);
	pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE + 1, 0), 0xa0, dword);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const u16 spd_addr[] = {
		DIMM0, DIMM1, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
	};

	int needs_reset;
	unsigned bsp_apicid = 0, nodes;
	struct mem_controller ctrl[8];

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		sio_setup();
	}

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx);

	w83627thg_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	needs_reset = setup_coherent_ht_domain();

	wait_all_core0_started();
#if CONFIG_LOGICAL_CPUS
	// It is said that we should start core1 after all core0 launched
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	needs_reset |= ht_setup_chains_x();
	needs_reset |= ck804_early_setup_x();
	if (needs_reset) {
		print_info("ht reset -\n");
		soft_reset();
	}

	allow_all_aps_stop(bsp_apicid);

	nodes = get_nodes();
	//It's the time to set ctrl now;
	fill_mem_ctrl(nodes, ctrl, spd_addr);

	enable_smbus();

	ms7135_set_nf4_voltage();
	ms7135_set_ram_voltage();

#if CONFIG_DEBUG_SMBUS
	dump_spd_registers(&ctrl[0]);
	dump_smbus_registers();
#endif

	sdram_initialize(nodes, ctrl);

	post_cache_as_ram();
}
