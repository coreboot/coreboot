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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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
#include <superio/smsc/sio1007/chip.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <tpm.h>
#include "gpio.h"

#define SIO_PORT 0x164e

static void pch_enable_lpc(void)
{
	device_t dev = PCH_LPC_DEV;

	/* Set COM1/COM2 decode range */
	pci_write_config16(dev, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO + PS/2 Keyboard/Mouse */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN | KBC_LPC_EN;
	pci_write_config16(dev, LPC_EN, lpc_config);

	/* Map 256 bytes at 0x1600 to the LPC bus. */
	pci_write_config32(dev, LPC_GEN1_DEC, 0xfc1601);

	/* Map a range for the runtime_port registers to the LPC bus. */
	pci_write_config32(dev, LPC_GEN2_DEC, 0xc0181);

	/* Enable COM1 */
	if (sio1007_enable_uart_at(SIO_PORT)) {
		pci_write_config16(dev, LPC_EN,
				   lpc_config | COMA_LPC_EN);
	}
}

static void rcba_config(void)
{
	u32 reg32;

	southbridge_configure_default_intmap();

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_ALWAYS;
	RCBA32(FD) = reg32;
}

// FIXME, this function is generic code that should go to sb/... or
// nb/../early_init.c
static void early_pch_init(void)
{
	u8 reg8;

	// reset rtc power status
	reg8 = pci_read_config8(PCH_LPC_DEV, 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCH_LPC_DEV, 0xa4, reg8);
}

static void setup_sio_gpios(void)
{
	const u16 port = SIO_PORT;
	const u16 runtime_port = 0x180;

	/* Turn on configuration mode. */
	outb(0x55, port);

	/* Set the GPIO direction, polarity, and type. */
	sio1007_setreg(port, 0x31, 1 << 0, 1 << 0);
	sio1007_setreg(port, 0x32, 0 << 0, 1 << 0);
	sio1007_setreg(port, 0x33, 0 << 0, 1 << 0);

	/* Set the base address for the runtime register block. */
	sio1007_setreg(port, 0x30, runtime_port >> 4, 0xff);
	sio1007_setreg(port, 0x21, runtime_port >> 12, 0xff);

	/* Turn on address decoding for it. */
	sio1007_setreg(port, 0x3a, 1 << 1, 1 << 1);

	/* Set the value of GPIO 10 by changing GP1, bit 0. */
	u8 byte;
	byte = inb(runtime_port + 0xc);
	byte |= (1 << 0);
	outb(byte, runtime_port + 0xc);

	/* Turn off address decoding for it. */
	sio1007_setreg(port, 0x3a, 0 << 1, 1 << 1);

	/* Turn off configuration mode. */
	outb(0xaa, port);
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
		.spd_addresses = { 0xa0, 0x00, 0xa4, 0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 0,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			{ 1, 0, 0x0040 }, /* P0: Front port  (OC0) */
			{ 1, 1, 0x0040 }, /* P1: Back port   (OC1) */
			{ 1, 0, 0x0040 }, /* P2: MINIPCIE1   (no OC) */
			{ 1, 0, 0x0040 }, /* P3: MMC         (no OC) */
			{ 1, 2, 0x0040 }, /* P4: Front port  (OC2) */
			{ 0, 0, 0x0000 }, /* P5: Empty */
			{ 0, 0, 0x0000 }, /* P6: Empty */
			{ 0, 0, 0x0000 }, /* P7: Empty */
			{ 1, 4, 0x0040 }, /* P8: Back port   (OC4) */
			{ 1, 4, 0x0040 }, /* P9: MINIPCIE3   (no OC) */
			{ 1, 4, 0x0040 }, /* P10: BLUETOOTH  (no OC) */
			{ 0, 4, 0x0000 }, /* P11: Empty */
			{ 1, 6, 0x0040 }, /* P12: Back port  (OC6) */
			{ 1, 5, 0x0040 }, /* P13: Back port  (OC5) */
		},
	};

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
	setup_pch_gpios(&emeraldlake2_gpio_map);
	setup_sio_gpios();

	/* Early SuperIO setup */
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

	boot_mode = southbridge_detect_s3_resume() ? 2 : 0;

	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
	enable_smbus();

	/* Prepare USB controller early in S3 resume */
	if (boot_mode == 2)
		enable_usb_bar();

	post_code(0x3a);
	pei_data.boot_mode = boot_mode;
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(&pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);
	post_code(0x3b);
	/* Perform some initialization that must run before stage2 */
	early_pch_init();
	post_code(0x3c);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
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
	if (CONFIG_LPC_TPM) {
		init_tpm(boot_mode == 2);
	}
}
