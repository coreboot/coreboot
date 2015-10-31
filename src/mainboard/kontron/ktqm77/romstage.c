/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include "gpio.h"

static void pch_enable_lpc(void)
{
	/* Set COM3/COM1 decode ranges: 0x3e8/0x3f8 */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0070);

	/* Enable KBC on 0x06/0x64 (KBC),
	 *        EC on 0x62/0x66 (MC),
	 *        EC on 0x20c-0x20f (GAMEH),
	 *        Super I/O on 0x2e/0x2f (CNF1),
	 *	  COM1/COM3 decode ranges. */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   KBC_LPC_EN | MC_LPC_EN |
			   CNF1_LPC_EN | GAMEH_LPC_EN |
			   COMA_LPC_EN | COMB_LPC_EN);
}

static void rcba_config(void)
{
	u32 reg32;

	southbridge_configure_default_intmap();

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_ALWAYS;
	/* Disable PCI bridge so MRC does not probe this bus */
	reg32 |= PCH_DISABLE_P2P;
	RCBA32(FD) = reg32;
}

static void pnp_enter_ext_func_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0x87, port);
	outb(0x87, port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

static void superio_gpio_config(void)
{
	int lvds_3v = 0; // 0 (5V) or 1 (3V3)
	int dis_bl_inv = 1; // backlight inversion: 1 = disabled, 0 = enabled
	device_t dev = PNP_DEV(0x2e, 0x9);
	pnp_enter_ext_func_mode(dev);
	pnp_write_config(dev, 0x29, 0x02); /* Pins 119, 120 are GPIO21, 20 */
	pnp_write_config(dev, 0x30, 0x03); /* Enable GPIO2+3 */
	pnp_write_config(dev, 0x2a, 0x01); /* Pins 62, 63, 65, 66 are
					      GPIO27, 26, 25, 24 */
	pnp_write_config(dev, 0x2c, 0xc3); /* Pin 90 is GPIO32,
					      Pins 78~85 are UART B */
	pnp_write_config(dev, 0x2d, 0x00); /* Pins 67, 68, 70~73, 75, 77 are
					      GPIO57~50 */
	pnp_set_logical_device(dev);
	/* Values can only be changed, when devices are enabled. */
	pnp_write_config(dev, 0xe3, 0xdd); /* GPIO2 bits 1, 5 are output */
	pnp_write_config(dev, 0xe4, (dis_bl_inv << 5) | (lvds_3v << 1)); /* GPIO2 bits 1, 5 */
	pnp_exit_ext_func_mode(dev);
}

#include <cpu/intel/romstage.h>
void main(unsigned long bist)
{
	int boot_mode = 0;
	int cbmem_was_initted;

	struct pei_data pei_data = {
		.pei_version = PEI_VERSION,
		.mchbar = (uintptr_t)DEFAULT_MCHBAR,
		.dmibar = (uintptr_t)DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBABASE,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, // 0 Mobile, 1 Desktop/Server
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xA0, 0x00,0xA4,0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		.gbe_enable = 1,
		.ddr3lv_support = 0,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			 /* enabled   usb oc pin    length */
			{ 1, 0, 0x0040 }, /* P0: lower left USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P1: upper left USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P2: lower right USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P3: upper right USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P4: lower USB 2.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P5: upper USB 2.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P6: front panel USB 2.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P7: front panel USB 2.0 (OC0) */
			{ 1, 4, 0x0040 }, /* P8: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P9: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P10: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P11: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P12: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P13: internal USB 2.0 (OC4) */
		},
		.usb3 = {
			.mode =			3,	/* Smart Auto? */
			.hs_port_switch_mask =	0xf,	/* All four ports. */
			.preboot_support =	1,	/* preOS driver? */
			.xhci_streams =		1,	/* Enable. */
		},
		.pcie_init = 1,
	};

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
	setup_pch_gpios(&ktqm77_gpio_map);
	superio_gpio_config();

	/* Initialize console device(s) */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected\n");
		boot_mode = 1;

		/* System is not happy after keyboard reset... */
		printk(BIOS_DEBUG, "Issuing CF9 warm reset\n");
		outb(0x6, 0xcf9);
		halt();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	/* Enable PEG10 (1x16) */
	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN,
			   pci_read_config32(PCI_DEV(0, 0, 0), DEVEN) |
			   DEVEN_PEG10);

	boot_mode = southbridge_detect_s3_resume() ? 2 : 0;

	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
	enable_smbus();

	/* Prepare USB controller early in S3 resume */
	if (boot_mode == 2)
		enable_usb_bar();

	post_code(0x39);

	post_code(0x3a);
	pei_data.boot_mode = boot_mode;
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(&pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);
	post_code(0x3c);

	rcba_config();
	post_code(0x3d);

	quick_ram_check();
	post_code(0x3e);

	cbmem_was_initted = !cbmem_recovery(boot_mode==2);
	if (boot_mode!=2)
		save_mrc_data(&pei_data);

	if (boot_mode==2 && !cbmem_was_initted) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		halt();
	}
	northbridge_romstage_finalize(boot_mode==2);

	post_code(0x3f);
}
