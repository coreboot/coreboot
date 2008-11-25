/*
* This file is part of the coreboot project.
*
* Copyright (C) 2007 Advanced Micro Devices, Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <pirq_routing.h>
#include <irq_tables.h>
#include "i82801gx.h"

/**
 * Write_pirq_routing_table
 * This is just a pass through for now, but it might have to change. 
 *  All hooks for southbridge irq setup go here. 
 * See cs5536 for an example of how hooks can be put in. 
 */
unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr);

}
