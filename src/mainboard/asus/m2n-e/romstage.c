/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
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
#include "southbridge/nvidia/mcp55/early_smbus.c"
#include <northbridge/amd/amdk8/raminit.h>
#include <delay.h>
#include <lib.h>
#include <spd.h>
#include <cpu/x86/lapic.h>
#include "northbridge/amd/amdk8/reset_test.c"
#include <superio/ite/common/ite.h>
#include <superio/ite/it8716f/it8716f.h>
#include <cpu/x86/bist.h>
#include "northbridge/amd/amdk8/debug.c"
#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, IT8716F_SP1)
#define CLKIN_DEV PNP_DEV(0x2e, IT8716F_GPIO)

unsigned get_sbdn(unsigned bus);

unsigned get_sbdn(unsigned bus)
{
	pci_devfn_t dev;

	/* Find the device. */
	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_NVIDIA,
				       PCI_DEVICE_ID_NVIDIA_MCP55_HT), bus);

	return (dev >> 15) & 0x1f;
}

static void memreset(int controllers, const struct mem_controller *ctrl) {}
static inline void activate_spd_rom(const struct mem_controller *ctrl) {}

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

#include "southbridge/nvidia/mcp55/early_ctrl.c"
#include <northbridge/amd/amdk8/f.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "lib/generic_sdram.c"
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"
#include <southbridge/nvidia/mcp55/early_setup_ss.h>
#include "southbridge/nvidia/mcp55/early_setup_car.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "northbridge/amd/amdk8/early_ht.c"

/* FIXME
 * Dummy method to allow build
 * Determine if this board / CPU should support
 * FID/VID and implement proper support if so
 */
#if IS_ENABLED(CONFIG_SET_FIDVID)
void init_fidvid_ap(u32 bsp_apicid, u32 apicid) { }
#endif

static void sio_setup(void)
{
	u8 byte;
	u32 dword;
	pci_devfn_t dev = PCI_DEV(0, MCP55_DEVN_BASE + 1, 0); /* LPC */

	/* Subject decoding */
	byte = pci_read_config8(dev, 0x7b);
	byte |= (1 << 5);
	pci_write_config8(dev, 0x7b, byte);

	/* LPC Positive Decode 0 */
	dword = pci_read_config32(dev, 0xa0);
	dword |= (1 << 0); /* COM1 */
	pci_write_config32(dev, 0xa0, dword);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const u16 spd_addr[] = {
		DIMM0, DIMM2, 0, 0,	/* Channel A (DIMM_A1, DIMM_A2) */
		DIMM1, DIMM3, 0, 0,	/* Channel B (DIMM_B1, DIMM_B2) */
	};

	struct sys_info *sysinfo = &sysinfo_car;
	int needs_reset = 0;
	unsigned bsp_apicid = 0;

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Allow the HT devices to be found. */
		enumerate_ht_chain();
		sio_setup();
	}

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	ite_conf_clkin(CLKIN_DEV, ITE_UART_CLK_PREDIVIDE_24);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	setup_mb_resource_map();
	report_bist_failure(bist);
	console_init();

	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo, sysinfo + 1);
	printk(BIOS_DEBUG, "bsp_apicid=0x%02x\n", bsp_apicid);

	/* In BSP so could hold all AP until sysinfo is in RAM. */
	set_sysinfo_in_ram(0);

	setup_coherent_ht_domain(); /* Routing table and start other core0. */
	wait_all_core0_started();

#if CONFIG_LOGICAL_CPUS
	/*
	 * It is said that we should start core1 after all core0 launched
	 * becase optimize_link_coherent_ht is moved out from
	 * setup_coherent_ht_domain, so here need to make sure last core0 is
	 * started, esp for two way system (there may be APIC ID conflicts in
	 * that case).
	 */
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	/* Set up chains and store link pair for optimization later. */
	ht_setup_chains_x(sysinfo); /* Init sblnk and sbbusn, nodes, sbdn. */

	/* TODO: FIDVID */

	init_timer(); /* Need to use TMICT to synchronize FID/VID. */

	needs_reset |= optimize_link_coherent_ht();
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	needs_reset |= mcp55_early_setup_x();

	/*
	 * FIDVID change will issue one LDTSTOP and the HT change will be
	 * effective too.
	 */
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
		soft_reset();
	}
	allow_all_aps_stop(bsp_apicid);

	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	enable_smbus();

	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);

	/* BSP switch stack to RAM and copy sysinfo RAM now. */
	post_cache_as_ram();
}
