/*
 * table management code for Linux BIOS
 * This is the architecture-independent driver; it has a hook to architecture-dependent code. 
 *
 * Copright (C) 2002 Eric Biederman, Linux NetworX
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

#include <console/console.h>
#include <cpu.h>
#include <tables.h>
#include <boot/linuxbios_tables.h>

struct lb_memory *
write_tables(void)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;

	rom_table_start = 0xf0000;      
	rom_table_end =   0xf0000;           
	/* Start low addr at 16 bytes instead of 0 because of a buglet
	* in the generic linux unzip code, as it tests for the a20 line.
	*/
	low_table_start = 0;
	low_table_end = 16;

	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(
		low_table_start, low_table_end,
		rom_table_start, rom_table_end);

	return get_lb_mem();
}
