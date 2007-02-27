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
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "i440bx.h"

/* this is the starting point */

static void i440bxemulation_enable_dev(struct device *dev)
{
	printk(BIOS_INFO, "%s: \n", __FUNCTION__);
        /* Set the operations if it is a special bus type */
/*
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
                dev->ops = &pci_domain_ops;
		pci_set_method(dev);
        }
 */
}

struct chip_operations northbridge_intel_i440bxemulation_ops = {
	.name="Intel 440BX Northbridge Emulation",
	.enable_dev = i440bxemulation_enable_dev, 
};
