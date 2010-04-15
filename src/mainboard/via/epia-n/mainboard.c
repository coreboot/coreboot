/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Aaron Lwe <aaron.lwe@gmail.com> for VIA)
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <boot/tables.h>
#include <southbridge/via/vt8237r/vt8237r.h>
#include "chip.h"

int add_mainboard_resources(struct lb_memory *mem)
{
#if CONFIG_IOAPIC == 1
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		VT8237R_APIC_BASE, 0x1000);
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		0xFEE00000ULL, 0x1000);
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		0xFFFF0000ULL, 0x10000);
#endif
	return 0;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("VIA EPIA-N Mainboard")
};
