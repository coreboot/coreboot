/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 LiPPERT Embedded Computers GmbH
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

/* Based on mainboard.c from AMD's DB800 mainboard. */

#include <stdlib.h>
#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>

/* Bit1 switches Com1 to RS485, bit2 same for Com2, bit5 turns off the Live LED. */
#if CONFIG_ONBOARD_UARTS_RS485
	#define SIO_GP1X_CONFIG 0x26
#else
	#define SIO_GP1X_CONFIG 0x20
#endif

static const u16 ec_init_table[] = {	/* hi=data, lo=index */
	0x1900,		/* Enable monitoring */
	0x0351,		/* TMPIN1,2 diode mode, TMPIN3 off */
	0x805C,		/* Unlock zero adjust */
	0x7056, 0x3C57,	/* Zero adjust TMPIN1,2 */
	0x005C,		/* Lock zero adjust */
	0xD014		/* Also set FAN_CTL polarity to Active High */
};

static void init(struct device *dev)
{
	unsigned int gpio_base, i;
	printk(BIOS_DEBUG, "LiPPERT RoadRunner-LX ENTER %s\n", __func__);

	/* Init CS5536 GPIOs. */
	gpio_base = pci_read_config32(dev_find_device(PCI_VENDOR_ID_AMD,
		    PCI_DEVICE_ID_AMD_CS5536_ISA, 0), PCI_BASE_ADDRESS_1) - 1;

	outl(0x00000040, gpio_base + 0x00); // GPIO6  value      1 - LAN_PD#
	outl(0x00000040, gpio_base + 0x04); // GPIO6  output     1 - LAN_PD#
	outl(0x00000400, gpio_base + 0x34); // GPIO10 in aux1    1 - THRM_ALRM#
	outl(0x00000400, gpio_base + 0x20); // GPIO10 input      1 - THRM_ALRM#
	outl(0x08000000, gpio_base + 0x98); // GPIO27 pull up    0 - PM-LED

	/* Init Environment Controller. */
	for (i = 0; i < ARRAY_SIZE(ec_init_table); i++) {
		u16 val = ec_init_table[i];
		outb((u8)val, 0x0295);
		outb(val >> 8, 0x0296);
	}

	/* bit5 = Live LED, bit2 = RS485_EN2, bit1 = RS485_EN1 */
	outb(SIO_GP1X_CONFIG, 0x1220); /* Simple-I/O GP17-10 */
	printk(BIOS_DEBUG, "LiPPERT RoadRunner-LX EXIT %s\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
