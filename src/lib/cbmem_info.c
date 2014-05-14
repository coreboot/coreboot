/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <console/console.h>
#include <cbmem.h>
#include <stdlib.h>

static const struct cbmem_id_to_name cbmem_ids[] = { CBMEM_ID_TO_NAME_TABLE };

void cbmem_print_entry(int n, u32 id, u64 base, u64 size)
{
	int i;
	const char *name;

	name = NULL;
	for (i = 0; i < ARRAY_SIZE(cbmem_ids); i++) {
		if (cbmem_ids[i].id == id) {
			name = cbmem_ids[i].name;
			break;
		}
	}

	if (name == NULL)
		printk(BIOS_DEBUG, "%08x ", id);
	else
		printk(BIOS_DEBUG, "%s", name);
	printk(BIOS_DEBUG, "%2d. ", n);
	printk(BIOS_DEBUG, "%08llx ", base);
	printk(BIOS_DEBUG, "%08llx\n", size);
}
