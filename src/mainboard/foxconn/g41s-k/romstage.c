/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
 * Copyright (C) 2017 Samuel Holland <samuel@sholland.org>
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

#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <northbridge/intel/x4x/iomap.h>
#include <northbridge/intel/x4x/x4x.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8720f/it8720f.h>
#include <lib.h>
#include <timestamp.h>

#define LPC_DEV PCI_DEV(0, 0x1f, 0)
#define SERIAL_DEV PNP_DEV(0x2e, IT8720F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8720F_GPIO)

static void mb_lpc_setup(void)
{
	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(LPC_DEV, GPIO_BASE, (DEFAULT_GPIOBASE | 1));
	pci_write_config8(LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	/* Set up GPIOs on Super I/O. */
	ite_reg_write(GPIO_DEV, 0x25, 0x01);
	ite_reg_write(GPIO_DEV, 0x26, 0x04);
	ite_reg_write(GPIO_DEV, 0x27, 0x00);
	ite_reg_write(GPIO_DEV, 0x28, 0x40);
	ite_reg_write(GPIO_DEV, 0x29, 0x01);
	ite_reg_write(GPIO_DEV, 0x73, 0x00);
	ite_reg_write(GPIO_DEV, 0x74, 0x00);
	ite_reg_write(GPIO_DEV, 0xb1, 0x04);
	ite_reg_write(GPIO_DEV, 0xb8, 0x20);
	ite_reg_write(GPIO_DEV, 0xbb, 0x01);
	ite_reg_write(GPIO_DEV, 0xc0, 0x00);
	ite_reg_write(GPIO_DEV, 0xc3, 0x01);
	ite_reg_write(GPIO_DEV, 0xcb, 0x01);
	ite_reg_write(GPIO_DEV, 0xf5, 0x28);
	ite_reg_write(GPIO_DEV, 0xf6, 0x12);
	ite_enable_3vsbsw(GPIO_DEV);

	/* Set up IRQ routing. */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x3241;
	RCBA16(D29IR) = 0x0237;

	/* Enable IOAPIC. */
	RCBA8(OIC) = 0x03;
	RCBA8(OIC);

	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_PCIE4 | FD_PCIE3 | FD_INTLAN |
		FD_ACMOD | FD_ACAUD | FD_PATA | 1;
	RCBA32(CG) = 0x00000001;
}

static void ich7_enable_lpc(void)
{
	pci_write_config16(LPC_DEV, LPC_IO_DEC, 0x0010);
	pci_write_config16(LPC_DEV, LPC_EN, CNF1_LPC_EN | KBC_LPC_EN |
		COMB_LPC_EN | COMA_LPC_EN);

	/* Decode 64 bytes at 0x0a00 to LPC for Super I/O EC and GPIO. */
	pci_write_config32(LPC_DEV, 0x84, 0x003c0a01);
}

void mainboard_romstage_entry(unsigned long bist)
{
	//                          ch0      ch1
	const u8 spd_addrmap[4] = { 0x50, 0, 0, 0 };
	u8 boot_path = 0;
	u8 s3_resume;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	/* Disable watchdog timer. */
	RCBA32(GCS) = RCBA32(GCS) | 0x20;

	/* Set up southbridge and Super I/O GPIOs. */
	ich7_enable_lpc();
	mb_lpc_setup();
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	report_bist_failure(bist);
	enable_smbus();

	x4x_early_init();

	s3_resume = southbridge_detect_s3_resume();
	if (s3_resume)
		boot_path = BOOT_PATH_RESUME;
	if (MCHBAR32(PMSTS_MCHBAR) & PMSTS_WARM_RESET)
		boot_path = BOOT_PATH_WARM_RESET;

	printk(BIOS_DEBUG, "Initializing memory\n");
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(boot_path, spd_addrmap);
	timestamp_add_now(TS_AFTER_INITRAM);
	quick_ram_check();
	printk(BIOS_DEBUG, "Memory initialized\n");

	x4x_late_init(s3_resume);

	printk(BIOS_DEBUG, "x4x late init complete\n");

}
