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

#if !CONFIG_DYNAMIC_CBMEM
/* This is for compatibility with old boards only. Any new chipset and board
 * must implement get_top_of_ram() for both romstage and ramstage to support
 * features like CAR_MIGRATION and CBMEM_CONSOLE.
 */
void set_top_of_ram(uint64_t ramtop)
{
	high_tables_base = ramtop - HIGH_MEMORY_SIZE;
	high_tables_size = HIGH_MEMORY_SIZE;
	printk(BIOS_DEBUG, "high_tables_base: %08llx, size %lld\n",
		high_tables_base, high_tables_size);
}
#endif
