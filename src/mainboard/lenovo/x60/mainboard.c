/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <arch/coreboot_tables.h>
#include "chip.h"
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/acpi/ec.h>
#include <northbridge/intel/i945/i945.h>

static void backlight_enable(void)
{
	pmh7_register_set_bit(0x50, 5);
}

static void trackpoint_enable(void)
{
	ec_write(0x0b, 0x03);
}

static void wlan_enable(void)
{
	ec_write(0x3a, 0x20);
}

static void mainboard_enable(device_t dev)
{
	device_t dev0;

	backlight_enable();
	trackpoint_enable();
	/* FIXME: this should be ACPI's task
	 * but for now, enable it here */
	wlan_enable();

	/* enable ACPI events */
	ec_write(0x00, 0xa6);
	ec_write(0x01, 0x05);

	ec_write(0x02, 0xa0);
	ec_write(0x03, 0x05);

	/* set mask of enabled beeps */
	ec_write(0x04, 0xfe);
	ec_write(0x05, 0x96);

	/* Unknown, but required for hotkeys
	   Maybe a mask for enabled keys? */

	ec_write(0x12, 0xff);
	ec_write(0x13, 0xff);
	ec_write(0x14, 0xf4);
	ec_write(0x15, 0x3c);

	/* enable Audio */
	ec_clr_bit(0x3a, 0);

	/* If we're resuming from suspend, blink suspend LED */
	dev0 = dev_find_slot(0, PCI_DEVFN(0,0));
	if (dev0 && pci_read_config32(dev0, SKPAD) == 0xcafed00d)
		ec_write(0x0c, 0xc7);
}

struct chip_operations mainboard_ops = {
	CHIP_NAME(CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER)
	.enable_dev = mainboard_enable,
};

