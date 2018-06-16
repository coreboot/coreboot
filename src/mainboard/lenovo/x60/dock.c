/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include <arch/io.h>
#include "dock.h"
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <superio/nsc/pc87392/pc87392.h>

static void dlpc_write_register(int reg, int value)
{
	outb(reg, 0x164e);
	outb(value, 0x164f);
}

static u8 dlpc_read_register(int reg)
{
	outb(reg, 0x164e);
	return inb(0x164f);
}

static void dock_write_register(int reg, int value)
{
	outb(reg, 0x2e);
	outb(value, 0x2f);
}

static u8 dock_read_register(int reg)
{
	outb(reg, 0x2e);
	return inb(0x2f);
}

static void dlpc_gpio_set_mode(int port, int mode)
{
	dlpc_write_register(0xf0, port);
	dlpc_write_register(0xf1, mode);
}

static void dock_gpio_set_mode(int port, int mode, int irq)
{
	dock_write_register(0xf0, port);
	dock_write_register(0xf1, mode);
	dock_write_register(0xf2, irq);
}

static void dlpc_gpio_init(void)
{
	/* Select GPIO module */
	dlpc_write_register(0x07, 0x07);
	/* GPIO Base Address 0x1680 */
	dlpc_write_register(0x60, 0x16);
	dlpc_write_register(0x61, 0x80);

	/* Activate GPIO */
	dlpc_write_register(0x30, 0x01);

	dlpc_gpio_set_mode(0x00, 3);
	dlpc_gpio_set_mode(0x01, 3);
	dlpc_gpio_set_mode(0x02, 0);
	dlpc_gpio_set_mode(0x03, 3);
	dlpc_gpio_set_mode(0x04, 4);
	dlpc_gpio_set_mode(0x20, 4);
	dlpc_gpio_set_mode(0x21, 4);
	dlpc_gpio_set_mode(0x23, 4);
}

int dlpc_init(void)
{
	int timeout = 1000;

	/* Enable 14.318MHz CLK on CLKIN */
	dlpc_write_register(0x29, 0xa0);
	while(!(dlpc_read_register(0x29) & 0x10) && timeout--)
		udelay(1000);

	if (!timeout)
		return 1;

	/* Select DLPC module */
	dlpc_write_register(0x07, 0x19);
	/* DLPC Base Address 0x164c */
	dlpc_write_register(0x60, 0x16);
	dlpc_write_register(0x61, 0x4c);
	/* Activate DLPC */
	dlpc_write_register(0x30, 0x01);

	dlpc_gpio_init();

	return 0;
}

int dock_connect(void)
{
	int timeout = 1000;

	outb(0x07, 0x164c);

	timeout = 1000;

	while(!(inb(0x164c) & 8) && timeout--)
		udelay(1000);

	if (!timeout) {
		/* docking failed, disable DLPC switch */
		outb(0x00, 0x164c);
		dlpc_write_register(0x30, 0x00);
		return 1;
	}

	/* Assert D_PLTRST# */
	outb(0xfe, 0x1680);
	udelay(100000);
	/* Deassert D_PLTRST# */
	outb(0xff, 0x1680);

	udelay(100000);

	/* startup 14.318MHz Clock */
	dock_write_register(0x29, 0x06);
	/* wait until clock is settled */
	timeout = 1000;
	while(!(dock_read_register(0x29) & 0x08) && timeout--)
		udelay(1000);

	if (!timeout)
		return 1;

	/* Pin  6: CLKRUN
	 * Pin 72:  #DR1
	 * Pin 19: #SMI
	 * Pin 73: #MTR
	 */
	dock_write_register(0x24, 0x37);

	/* PNF active HIGH */
	dock_write_register(0x25, 0xa0);

	/* disable FDC */
	dock_write_register(0x26, 0x01);

	/* Enable GPIO IRQ to #SMI */
	dock_write_register(0x28, 0x02);

	/* select GPIO */
	dock_write_register(0x07, 0x07);

	/* set base address */
	dock_write_register(0x60, 0x16);
	dock_write_register(0x61, 0x20);

	/* init GPIO pins */
	dock_gpio_set_mode(0x00, PC87392_GPIO_PIN_DEBOUNCE |
				 PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x01, PC87392_GPIO_PIN_DEBOUNCE |
				 PC87392_GPIO_PIN_PULLUP,
				 PC87392_GPIO_PIN_TRIGGERS_SMI);

	dock_gpio_set_mode(0x02, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x03, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x04, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x05, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x06, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x07, PC87392_GPIO_PIN_PULLUP, 0x02);

	dock_gpio_set_mode(0x10, PC87392_GPIO_PIN_DEBOUNCE |
				 PC87392_GPIO_PIN_PULLUP,
				 PC87392_GPIO_PIN_TRIGGERS_SMI);

	dock_gpio_set_mode(0x11, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x12, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x13, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x14, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x15, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x16, PC87392_GPIO_PIN_PULLUP |
				 PC87392_GPIO_PIN_OE , 0x00);

	dock_gpio_set_mode(0x17, PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x20, PC87392_GPIO_PIN_TYPE_PUSH_PULL |
				 PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x21, PC87392_GPIO_PIN_TYPE_PUSH_PULL |
				 PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x22, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x23, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x24, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x25, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x26, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x27, PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x30, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x31, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x32, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x33, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x34, PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x35, PC87392_GPIO_PIN_PULLUP |
				 PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x36, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x37, PC87392_GPIO_PIN_PULLUP, 0x00);

	/* enable GPIO */
	dock_write_register(0x30, 0x01);

	outb(0x00, 0x1628);
	outb(0x00, 0x1623);
	outb(0x82, 0x1622);
	outb(0xff, 0x1624);

	/* Enable USB and Ultrabay power */
	outb(0x03, 0x1628);

	dock_write_register(0x07, 0x03);
	dock_write_register(0x30, 0x01);
	return 0;
}

void dock_disconnect(void)
{
	printk(BIOS_DEBUG, "%s enter\n", __func__);
	/* disconnect LPC bus */
	outb(0x00, 0x164c);
	udelay(10000);

	/* Assert PLTRST and DLPCPD */
	outb(0xfc, 0x1680);
	udelay(10000);

	/* disable Ultrabay and USB Power */
	outb(0x00, 0x1628);
	udelay(10000);

	printk(BIOS_DEBUG, "%s finish\n", __func__);
}

int dock_present(void)
{
	return !((inw(DEFAULT_GPIOBASE + 0x0c) >> 13) & 1);
}

int dock_ultrabay_device_present(void)
{
	return inb(0x1621) & 0x02 ? 0 : 1;
}
