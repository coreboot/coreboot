/*
 * This file is part of the coreboot project.
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
#include <console/console.h>
#include <boot/tables.h>
#include "chip.h"

/* in arch/i386/boot/tables.c */
extern uint64_t high_tables_base, high_tables_size;

int add_mainboard_resources(struct lb_memory *mem)
{
#if HAVE_HIGH_TABLES == 1
  lb_add_memory_range(mem, LB_MEM_TABLE,
    high_tables_base, high_tables_size);
#endif
  return 0;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("Supermicro H8DME Mainboard")
};
