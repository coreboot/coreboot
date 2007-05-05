/*
 * table management code for Linux BIOS
 * This is the architecture-independent driver; it has a hook to architecture-dependent code. 
 *
 * Copyright (C) 2002 Eric Biederman, Linux NetworX
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */

#include <console.h>
//#include <cpu.h>
#include <tables.h>
#include <tables.h>

struct lb_memory *
write_tables(void)
{
	return arch_write_tables();
}
