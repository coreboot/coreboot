/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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
#include <console.h>
#include <string.h>
#include <mtrr.h>
#include <macros.h>
#include <spd_ddr2.h>
#include <cpu.h>
#include <msr.h>
#include <device/pci.h>
#include <pci_ops.h>
#include <mc146818rtc.h>
#include <lib.h>
#include <spd.h>
#include <io.h>
#include <types.h>
#include "raminit.h"
#include "i945.h"
#include "pcie_config.h"

int is_coldboot(void)
{
	int boot_mode = 0;
	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected.\n");
		boot_mode = 1;
	}
	return boot_mode;
}

