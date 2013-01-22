/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <bootblock_common.h>
#include <arch/bootblock_exit.h>
#include <arch/cbfs.h>
#include <arch/hlt.h>

static int boot_cpu(void)
{
	/*
	 * FIXME: This is a stub for now. All non-boot CPUs should be
	 * waiting for an interrupt. We could move the chunk of assembly
	 * which puts them to sleep in here...
	 */
	return 1;
}

void main(unsigned long bist)
{
	const char *target1 = "fallback/romstage";
	unsigned long romstage_entry;

	if (boot_cpu()) {
		bootblock_cpu_init();
		bootblock_mainboard_init();
	}

	printk(BIOS_INFO, "bootblock main(): loading romstage\n");
	romstage_entry = (unsigned long)cbfs_load_stage(
			CBFS_DEFAULT_MEDIA, target1);

	printk(BIOS_INFO, "bootblock main(): jumping to romstage\n");
	if (romstage_entry) bootblock_exit(romstage_entry);
	hlt();
}
