/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#include <console/console.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
// #include "i82371eb.h"


/* The plain PCI device uses the standard PCI operations. */

/* Note that this structure is not necessary (yet), 
 * but is here as an example of how you can set up your own ops 
 */

/* You can override or extend each of these operations as needed for the device. */
static struct device_operations i82371eb_pci_ops_dev = {
	.constructor		  = default_device_constructor,
	.phase3_scan		  = 0,
	.phase4_read_resources	  = pci_dev_read_resources,
	.phase4_set_resources	  = pci_dev_set_resources,
	.phase4_enable_disable 	  = 0,
	.phase5_enable_resources  = pci_dev_enable_resources,
	.phase6_init		  = pci_dev_init,
	.ops_pci		  = &pci_dev_ops_pci,
};


struct constructor i82371eb_constructors[] = {
	{.id={.type=DEVICE_ID_PCI, .u={.pci={.vendor=0x8086,  .device=0x7110}}}, 
		&i82371eb_pci_ops_dev},
	{.id={.type=DEVICE_ID_PCI, .u={.pci={.vendor=0x8086,  .device=0x7111}}}, 
		&i82371eb_pci_ops_dev},
	{.id={.type=DEVICE_ID_PCI, .u={.pci={.vendor=0x8086,  .device=0x7112}}}, 
		&i82371eb_pci_ops_dev},
	{.id={.type=DEVICE_ID_PCI, .u={.pci={.vendor=0x8086,  .device=0x7113}}}, 
		&i82371eb_pci_ops_dev},
	{.ops = 0},
};
