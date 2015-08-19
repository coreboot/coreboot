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
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/x86/bist.h>
#include <superio/ite/it8718f/it8718f.h>
#include <superio/ite/common/ite.h>
#include <lib.h>
#include <cpu/intel/romstage.h>
#include <arch/stages.h>
#include <cbmem.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8718F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8718F_GPIO)
#define EC_DEV PNP_DEV(0x2e, IT8718F_EC)
#define SUPERIO_DEV PNP_DEV(0x2e, 0)

/* Early mainboard specific GPIO setup.
 * We should use standard gpio.h eventually
 */

static void mb_gpio_init(void)
{
	device_t dev;

	/* Southbridge GPIOs. */
	dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(dev, GPIO_BASE, (DEFAULT_GPIOBASE | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);

	outl(0x1f15f7c0, DEFAULT_GPIOBASE + 0x00); /* GPIO_USE_SEL */
	outl(0xe2e9ffc3, DEFAULT_GPIOBASE + 0x04); /* GP_IO_SEL */
	outl(0xe0d7fcc3, DEFAULT_GPIOBASE + 0x0c); /* GP_LVL */
	outl(0x000039ff, DEFAULT_GPIOBASE + 0x2c); /* GPI_INV */
	outl(0x000000e7, DEFAULT_GPIOBASE + 0x30);
	outl(0x000000f0, DEFAULT_GPIOBASE + 0x34);
	outl(0x00000083, DEFAULT_GPIOBASE + 0x38);

	/* Set default power management registers */
	pci_write_config32(dev, PMBASE, DEFAULT_PMBASE | 1);
	outw(0x0011, DEFAULT_PMBASE + 0x00);
	outw(0x0120, DEFAULT_PMBASE + 0x02);
	outl(0x00001c01, DEFAULT_PMBASE + 0x04);
	outl(0x00bb29d2, DEFAULT_PMBASE + 0x08);
	outl(0x000000a0, DEFAULT_PMBASE + 0x10);
	outl(0xc5000000, DEFAULT_PMBASE + 0x28);
	outl(0x00000040, DEFAULT_PMBASE + 0x2c);
	outw(0x13e0, DEFAULT_PMBASE + 0x44);
	outw(0x003f, DEFAULT_PMBASE + 0x60);
	outw(0x0800, DEFAULT_PMBASE + 0x68);
	outw(0x0008, DEFAULT_PMBASE + 0x6a);
	outw(0x003f, DEFAULT_PMBASE + 0x72);

	/* Set default GPIOs on superio */
	ite_reg_write(GPIO_DEV, 0x25, 0x00);
	ite_reg_write(GPIO_DEV, 0x26, 0xc7);
	ite_reg_write(GPIO_DEV, 0x27, 0x80);
	ite_reg_write(GPIO_DEV, 0x28, 0x41);
	ite_reg_write(GPIO_DEV, 0x29, 0x0a);
	ite_reg_write(GPIO_DEV, 0x2c, 0x01);
	ite_reg_write(GPIO_DEV, 0x62, 0x08);
	ite_reg_write(GPIO_DEV, 0x62, 0x08);
	ite_reg_write(GPIO_DEV, 0x72, 0x00);
	ite_reg_write(GPIO_DEV, 0x73, 0x00);
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

	/* IRQ routing */
	RCBA32(0x3100) = 0x00002210;
	RCBA32(0x3104) = 0x00002100;
	RCBA32(0x3108) = 0x10004321;
	RCBA32(0x310c) = 0x00214321;
	RCBA32(0x3110) = 0x00000001;
	RCBA32(0x3140) = 0x00410032;
	RCBA32(0x3144) = 0x32100237;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;
	RCBA8(0x31ff);
}

static void ich7_enable_lpc(void)
{
	/* Disable Serial IRQ */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x64, 0x00);
	/* Decode range */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0010);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN,
		CNF1_LPC_EN | CNF2_LPC_EN | KBC_LPC_EN | COMA_LPC_EN | COMB_LPC_EN);

	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x88, 0x0291);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x8a, 0x007c);
}

void main(unsigned long bist)
{
	//                          ch0      ch1
	const u8 spd_addrmap[4] = { 0x50, 0, 0x52, 0 };

	/* Disable watchdog timer and route port 80 to LPC */
	RCBA32(0x3410) = (RCBA32(0x3410) | 0x20);// & ~0x4;

	/* Set southbridge and Super I/O GPIOs. */
	mb_gpio_init();

	ich7_enable_lpc();
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Disable SIO reboot */
	ite_reg_write(GPIO_DEV, 0xEF, 0x7E);

	console_init();

	report_bist_failure(bist);
	enable_smbus();

	x4x_early_init();

	printk(BIOS_DEBUG, "Initializing memory\n");
	sdram_initialize(0, spd_addrmap);
	quick_ram_check();
	printk(BIOS_DEBUG, "Memory initialized\n");
}
