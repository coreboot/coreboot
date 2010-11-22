/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <boot/tables.h>
#include <southbridge/via/k8t890/k8t890.h>
#include "chip.h"

#if CONFIG_GFXUMA
uint64_t uma_memory_base, uma_memory_size;
#endif

int add_mainboard_resources(struct lb_memory *mem)
{
#if (CONFIG_GFXUMA == 1)
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		uma_memory_base, uma_memory_size);
#endif
	return 0;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS M2V-MX SE Mainboard")
};
