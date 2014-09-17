/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit_native.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include "southbridge/intel/bd82x6x/gpio.h"
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include "gpio.h"
#include <cbfs.h>
#include <cpu/intel/romstage.h>

static void pch_enable_lpc(void)
{
	/* X230 EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
			   COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, 0x0c06a1);

	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	pci_write_config32(PCH_LPC_DEV, 0xac,
			   0x80010000);
}

static void rcba_config(void)
{
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  WLAN   INTA -> PIRQB
	 * D28IP_P2IP  ETH0   INTB -> PIRQF
	 * D28IP_P3IP  SDCARD INTC -> PIRQD
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQF
	 * D31IP_SIP   SATA   INTA -> PIRQB (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQH
	 * D31IP_TTIP  THRT   INTC -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQA (MSI)
	 *
	 * Trackpad interrupt is edge triggered and cannot be shared.
	 * TRACKPAD                -> PIRQG

	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
			(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P1IP) | (INTB << D28IP_P2IP) |
			(INTC << D28IP_P3IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQB, PIRQF, PIRQD, PIRQE);
	DIR_ROUTE(D27IR, PIRQA, PIRQH, PIRQA, PIRQB);
	DIR_ROUTE(D26IR, PIRQF, PIRQE, PIRQG, PIRQH);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	RCBA32(FD) = 0x17f81fe3;
	RCBA32(BUC) = 0;
}

void main(unsigned long bist)
{
	int s3resume = 0;
	spd_raw_data spd[4];

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		outb(0x6, 0xcf9);
		hlt ();
	}

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

//	setup_pch_gpios(&t530_gpio_map);
	outl(0x3962a5ff, DEFAULT_GPIOBASE + GPIO_USE_SEL);
	outl(0x8ebf6aff, DEFAULT_GPIOBASE + GP_IO_SEL);
	outl(0x66917ebb, DEFAULT_GPIOBASE + GP_LVL);
	outl(0x00000000, DEFAULT_GPIOBASE + GPO_BLINK);
	outl(0x00002002, DEFAULT_GPIOBASE + GPI_INV);
	outl(0x02ff08fe, DEFAULT_GPIOBASE + GPIO_USE_SEL2);
	outl(0x1f47f7fd, DEFAULT_GPIOBASE + GP_IO_SEL2);
	outl(0xffbeff43, DEFAULT_GPIOBASE + GP_LVL2);
	outl(0x000000ff, DEFAULT_GPIOBASE + GPIO_USE_SEL3);
	outl(0x00000fff, DEFAULT_GPIOBASE + GP_IO_SEL3);
	outl(0x00000f4f, DEFAULT_GPIOBASE + GP_LVL3);

	early_usb_init((struct southbridge_usb_port []) {
			{ 1, 1, 0 },
			{ 1, 1, 1 },
			{ 1, 2, 3 },
			{ 1, 1, -1 },
			{ 1, 1, -1 },
			{ 1, 0, -1 },
			{ 0, 0, -1 },
			{ 1, 2, -1 },
			{ 1, 0, -1 },
			{ 1, 1, 5 },
			{ 1, 0, -1 },
			{ 1, 0, -1 },
			{ 1, 3, -1 },
			{ 1, 1, -1 },
	         });

	/* Initialize console device(s) */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	s3resume = southbridge_detect_s3_resume();

	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
	enable_smbus();

	post_code(0x39);

	post_code(0x3a);
	timestamp_add_now(TS_BEFORE_INITRAM);

	memset (spd, 0, sizeof (spd));
	read_spd (&spd[0], 0x50);
	read_spd (&spd[2], 0x51);

	init_dram_ddr3 (spd, 1, TCK_800MHZ, s3resume);

	timestamp_add_now(TS_AFTER_INITRAM);
	post_code(0x3c);

	rcba_config();
	post_code(0x3d);

	northbridge_romstage_finalize(s3resume);

	post_code(0x3f);
	timestamp_add_now(TS_END_ROMSTAGE);
}
