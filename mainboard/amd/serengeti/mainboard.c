/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <mainboard.h>
#include <config.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>

static void show(struct device *dev)
{
	showallroutes(BIOS_DEBUG, PCI_BDF(0,0x18, 0));

}

struct device_operations serengeti = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = 1}}},
	.constructor		 = default_device_constructor,
	.phase6_init = show,
};
