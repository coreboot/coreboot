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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <boot/tables.h>
#include <delay.h>
#include <arch/io.h>
#include "dock.h"
#include "superio/nsc/pc87384/pc87384.h"
#include "ec/acpi/ec.h"
#include "southbridge/intel/i82801gx/i82801gx.h"

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

static int dock_superio_init(void)
{
	int timeout = 1000;
	/* startup 14.318MHz Clock */
	dock_write_register(0x29, 0xa0);
	/* wait until clock is settled */
	while(!(dock_read_register(0x29) & 0x10) && timeout--)
		udelay(1000);

	if (!timeout)
		return 1;

	/* set GPIO pins to Serial/Parallel Port
	 * functions
	 */
	dock_write_register(0x22, 0xeb);

	dock_write_register(0x07, PC87384_GPIO);
	dock_write_register(0x60, 0x16);
	dock_write_register(0x61, 0x20);

	dock_gpio_set_mode(0x00, PC87384_GPIO_PIN_DEBOUNCE |
			   PC87384_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x01, PC87384_GPIO_PIN_TYPE_PUSH_PULL |
			   PC87384_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x02, PC87384_GPIO_PIN_TYPE_PUSH_PULL |
			   PC87384_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x03, PC87384_GPIO_PIN_DEBOUNCE |
			   PC87384_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x04, PC87384_GPIO_PIN_DEBOUNCE |
			   PC87384_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x05, PC87384_GPIO_PIN_DEBOUNCE |
			   PC87384_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x06, PC87384_GPIO_PIN_DEBOUNCE |
			   PC87384_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x07, PC87384_GPIO_PIN_DEBOUNCE |
			   PC87384_GPIO_PIN_PULLUP, 0x00);

	outb(0xfd, 0x1620);

	/* no GPIO events enabled for PORT0 */
	outb(0x00, 0x1622);
	/* clear GPIO events on PORT0 */
	outb(0xff, 0x1623);
	outb(0xff, 0x1624);
	/* no GPIO events enabled for PORT1 */
	outb(0x00, 0x1626);

	/* clear GPIO events on PORT1*/
	outb(0xff, 0x1627);
	outb(0x1F, 0x1628);
	/* enable GPIO */
	dock_write_register(0x30, 0x01);
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
	udelay(1000);
	/* Deassert D_PLTRST# */
	outb(0xff, 0x1680);
	udelay(10000);

	return dock_superio_init();
}

void dock_disconnect(void)
{
	/* disconnect LPC bus */
	outb(0x00, 0x164c);
	/* Assert PLTRST and DLPCPD */
	outb(0xfc, 0x1680);
}

int dock_present(void)
{
	outb(0x61, 0x15ec);
	return inb(0x15ee) & 1;
}

