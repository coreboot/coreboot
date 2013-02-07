/*
 * Copyright (C) 2013 The ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>

#if CONFIG_WRITE_HIGH_TABLES
#include <cbmem.h>
#endif

void hardwaremain(int boot_complete);
void main(void)
{
	console_init();
	printk(BIOS_INFO, "hello from ramstage\n");

#if CONFIG_WRITE_HIGH_TABLES
	/* Leave some space for ACPI tables */
	high_tables_base = CONFIG_RAMBASE;
	high_tables_size = CONFIG_RAMBASE + 0x100000;
#endif

	hardwaremain(0);
}
