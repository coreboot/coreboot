/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 LiPPERT Embedded Computers GmbH
 *
 * Based on mainboard.c from AMD's DB800 mainboard.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdlib.h>
#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "chip.h"

static const u16 ec_init_table[] = {	// hi=data, lo=index
	0x1900,		// enable monitoring
	0x0351,		// TMPIN1,2 diode mode, TMPIN3 off
	0x805C,		// unlock zero adjust
	0x7056, 0x3C57,	// zero adjust TMPIN1,2
	0x005C,		// lock zero adjust
	0xD014		// also set FAN_CTL polarity to Active High
};

static void init(struct device *dev)
{
	struct mainboard_lippert_roadrunner_lx_config *mb = dev->chip_info;
	unsigned int gpio_base, i;
	printk_debug("LiPPERT RoadRunner-LX ENTER %s\n", __FUNCTION__);

	/* Init CS5536 GPIOs */
	gpio_base = pci_read_config32(dev_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_CS5536_ISA, 0),
				      PCI_BASE_ADDRESS_1) - 1;
	outl(0x00000040, gpio_base + 0x00);	// GPIO6  value      1 - LAN_PD#
	outl(0x00000040, gpio_base + 0x04);	// GPIO6  output     1 - LAN_PD#
	outl(0x00000400, gpio_base + 0x34);	// GPIO10 in aux1    1 - THRM_ALRM#
	outl(0x00000400, gpio_base + 0x20);	// GPIO10 input      1 - THRM_ALRM#
	outl(0x08000000, gpio_base + 0x98);	// GPIO27 pull up    0 - PM-LED

	/* Init Environment Controller */
	for (i=0; i<ARRAY_SIZE(ec_init_table); i++) {
		u16 val = ec_init_table[i];
		outb((u8)val, 0x0295); outb(val>>8, 0x0296);
	}

	outb(mb->sio_gp1x_config, 0x1220); // Simple-I/O GP17-10
	printk_debug("LiPPERT RoadRunner-LX EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_lippert_roadrunner_lx_ops = {
	CHIP_NAME("LiPPERT RoadRunner-LX Mainboard")
	.enable_dev = enable_dev,
};
