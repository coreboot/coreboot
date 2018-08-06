/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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
#include <stdlib.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <superio/ite/it8718f/it8718f.h>
#include <superio/ite/common/ite.h>
#include <lib.h>
#include <arch/stages.h>
#include <northbridge/intel/x4x/iomap.h>
#include <timestamp.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8718F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8718F_GPIO)
#define EC_DEV PNP_DEV(0x2e, IT8718F_EC)
#define SUPERIO_DEV PNP_DEV(0x2e, 0)

/* Early mainboard specific GPIO setup.
 * We should use standard gpio.h eventually
 */

static void mb_gpio_init(void)
{
	pci_devfn_t dev;

	/* Southbridge GPIOs. */
	dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(dev, GPIO_BASE, (DEFAULT_GPIOBASE | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	/* Set default GPIOs on superio */
	ite_reg_write(GPIO_DEV, 0x25, 0x00);
	ite_reg_write(GPIO_DEV, 0x26, 0xc7);
	ite_reg_write(GPIO_DEV, 0x27, 0x80);
	ite_reg_write(GPIO_DEV, 0x28, 0x41);
	ite_reg_write(GPIO_DEV, 0x29, 0x0a);
	ite_reg_write(GPIO_DEV, 0x2c, 0x01);
	ite_reg_write(GPIO_DEV, 0x62, 0x08);
	ite_reg_write(GPIO_DEV, 0x72, 0x00);
	ite_reg_write(GPIO_DEV, 0x73, 0x00);
	ite_reg_write(GPIO_DEV, 0xb8, 0x00);
	ite_reg_write(GPIO_DEV, 0xbb, 0x40);
	ite_reg_write(GPIO_DEV, 0xc0, 0x00);
	ite_reg_write(GPIO_DEV, 0xc1, 0xc7);
	ite_reg_write(GPIO_DEV, 0xc2, 0x80);
	ite_reg_write(GPIO_DEV, 0xc3, 0x01);
	ite_reg_write(GPIO_DEV, 0xc4, 0x0a);
	ite_reg_write(GPIO_DEV, 0xc8, 0x00);
	ite_reg_write(GPIO_DEV, 0xc9, 0x04);
	ite_reg_write(GPIO_DEV, 0xcb, 0x00);
	ite_reg_write(GPIO_DEV, 0xcc, 0x02);
	ite_reg_write(GPIO_DEV, 0xf0, 0x10);
	ite_reg_write(GPIO_DEV, 0xf1, 0x40);
	ite_reg_write(GPIO_DEV, 0xf6, 0x26);
	ite_reg_write(GPIO_DEV, 0xfc, 0x52);

	ite_reg_write(EC_DEV, 0xf0, 0x80);
	ite_reg_write(EC_DEV, 0xf1, 0x00);
	ite_reg_write(EC_DEV, 0xf2, 0x0a);
	ite_reg_write(EC_DEV, 0xf3, 0x80);
	ite_reg_write(EC_DEV, 0x70, 0x00); // Don't use IRQ9
	ite_reg_write(EC_DEV, 0x30, 0x01); // Enable

	/* IRQ routing */
	RCBA32(D31IP) = 0x00002210;
	RCBA32(D30IP) = 0x00002100;
	RCBA32(D29IP) = 0x10004321;
	RCBA32(D28IP) = 0x00214321;
	RCBA32(D27IP) = 0x00000001;
	RCBA32(D31IR) = 0x00410032;
	RCBA32(D29IR) = 0x32100237;
	RCBA32(D27IR) = 0x00000000;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;
	RCBA8(OIC);

	RCBA32(GCS) = 0x00190464;
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_PCIE4 | FD_PCIE3 | FD_ACMOD
		| FD_ACAUD | 1;
	RCBA32(CG) = 0x00000000;
	RCBA32(0x3430) = 0x00000001;
	RCBA32(0x3e00) = 0xff000001;
	RCBA32(0x3e08) = 0x00000080;
	RCBA32(0x3e0c) = 0x00800000;
	RCBA32(0x3e40) = 0xff000001;
	RCBA32(0x3e48) = 0x00000080;
	RCBA32(0x3e4c) = 0x00800000;
	RCBA32(0x3f00) = 0x0000000b;
}

static void ich7_enable_lpc(void)
{
	/* Disable Serial IRQ */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0x00);
	/* Decode range */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0010);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN,
		CNF1_LPC_EN | CNF2_LPC_EN | KBC_LPC_EN | FDD_LPC_EN
			| LPT_LPC_EN | COMA_LPC_EN | COMB_LPC_EN);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x007c0291);
}

void mainboard_romstage_entry(unsigned long bist)
{
	//                          ch0      ch1
	const u8 spd_addrmap[4] = { 0x50, 0, 0x52, 0 };
	u8 boot_path = 0;
	u8 s3_resume;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	/* Set southbridge and Super I/O GPIOs. */
	ich7_enable_lpc();
	mb_gpio_init();
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Disable SIO reboot */
	ite_reg_write(GPIO_DEV, 0xEF, 0x7E);

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
