/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/x86/cpu.h>


static void cpu_bus_init(struct device *dev)
{
#warning "cpu_bus_init() empty, what should it do?"
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __FUNCTION__);
	printk(BIOS_SPEW, ">> Exiting northbridge.c: %s\n", __FUNCTION__);
}

static void cpu_bus_noop(struct device *dev)
{
}

/** Operations for when the northbridge is running an APIC cluster. */
struct device_operations cn700_north_apic = {
	.id = {.type = DEVICE_ID_APIC_CLUSTER,
		{.apic_cluster = {.vendor = PCI_VENDOR_ID_VIA,
				       .device = PCI_DEVICE_ID_VIA_CN700_AGP}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= 0,
	.phase4_read_resources		= cpu_bus_noop,
	.phase4_set_resources		= cpu_bus_noop,
	.phase5_enable_resources	= cpu_bus_noop,
	.phase6_init			= cpu_bus_init,
};
