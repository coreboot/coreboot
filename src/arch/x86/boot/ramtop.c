/*
 * This file is part of the coreboot project.
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
 */

#include <console/console.h>
#include <cbmem.h>

unsigned long __attribute__((weak)) get_top_of_ram(void)
{
	printk(BIOS_WARNING, "WARNING: you need to define get_top_of_ram() for your chipset\n");
	return 0;
}

#if !CONFIG_DYNAMIC_CBMEM && !defined(__PRE_RAM__)
void set_top_of_ram(uint64_t ramtop)
{
	set_cbmem_table(ramtop - HIGH_MEMORY_SIZE, HIGH_MEMORY_SIZE);
}
#endif
