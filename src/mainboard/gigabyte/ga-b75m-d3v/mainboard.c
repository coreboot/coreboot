/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011-2012 Google Inc.
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <device/pci.h>
#include <cbfs.h>

static void mainboard_init(struct device *dev)
{
	RCBA32(0x38c8) = 0x00002005;
	RCBA32(0x38c4) = 0x00802005;
	RCBA32(0x2240) = 0x00330e71;
	RCBA32(0x2244) = 0x003f0eb1;
	RCBA32(0x2248) = 0x002102cd;
	RCBA32(0x224c) = 0x00f60000;
	RCBA32(0x2250) = 0x00020000;
	RCBA32(0x2254) = 0x00e3004c;
	RCBA32(0x2258) = 0x00e20bef;
	RCBA32(0x2260) = 0x003304ed;
	RCBA32(0x2278) = 0x001107c1;
	RCBA32(0x227c) = 0x001d07e9;
	RCBA32(0x2280) = 0x00e20000;
	RCBA32(0x2284) = 0x00ee0000;
	RCBA32(0x2288) = 0x005b05d3;
	RCBA32(0x2318) = 0x04b8ff2e;
	RCBA32(0x231c) = 0x03930f2e;
	RCBA32(0x3808) = 0x005044a3;
	RCBA32(0x3810) = 0x52410000;
	RCBA32(0x3814) = 0x0000008a;
	RCBA32(0x3818) = 0x00000006;
	RCBA32(0x381c) = 0x0000072e;
	RCBA32(0x3820) = 0x0000000a;
	RCBA32(0x3824) = 0x00000123;
	RCBA32(0x3828) = 0x00000009;
	RCBA32(0x382c) = 0x00000001;
	RCBA32(0x3834) = 0x0000061a;
	RCBA32(0x3838) = 0x00000003;
	RCBA32(0x383c) = 0x00000a76;
	RCBA32(0x3840) = 0x00000004;
	RCBA32(0x3844) = 0x0000e5e4;
	RCBA32(0x3848) = 0x0000000e;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_NONE, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_CRT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
