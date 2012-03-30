/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <cbfs.h>
#include <console/console.h>
#include "gnvs.h"

chromeos_acpi_t *vboot_data;
static u32 me_hash_saved[8];

void chromeos_init_vboot(chromeos_acpi_t *chromeos)
{
	vboot_data = chromeos;

	/* Copy saved ME hash into NVS */
	memcpy(vboot_data->mehh, me_hash_saved, sizeof(vboot_data->mehh));
}

void chromeos_set_vboot_data_ptr(void *blob)
{
	/* This code has to be rewritten to pass the vboot table
	 * pointer through the coreboot table instead of the 
	 * FDT, since FDT support was rejected upstream. For now
	 * just make the code available for reference.
	 */
#if 0 // CONFIG_ADD_FDT
	int node_offset, addr_cell_len;
	const u32 *cell;
	uintptr_t table_addr = (uintptr_t)vboot_data;
	u32 table_addr32;
	u64 table_addr64;
	void *table_ptr;

	cell = fdt_getprop(blob, 0, "#address-cells", NULL);
	if (cell && *cell == 2) {
		addr_cell_len = 8;
		table_addr64 = cpu_to_fdt64(table_addr);
		table_ptr = &table_addr64;
	} else {
		addr_cell_len = 4;
		table_addr32 = cpu_to_fdt32(table_addr);
		table_ptr = &table_addr32;
	}

	node_offset = fdt_path_offset(blob, "/chromeos-config");
	if (node_offset < 0) {
		printk(BIOS_ERR,
			"Couldn't find /chromeos-config in the fdt.\n");
		return;
	}

	if (fdt_setprop(blob, node_offset, "gnvs-vboot-table",
			table_ptr, addr_cell_len) < 0) {
		printk(BIOS_ERR, "Couldn't set gnvs-vboot-table.\n");
	}
#else
	printk(BIOS_ERR, "Can't set gnvs-vboot-table.\n");
#endif
}

void chromeos_set_me_hash(u32 *hash, int len)
{
	if ((len*sizeof(u32)) > sizeof(vboot_data->mehh))
		return;

	/* Copy to NVS or save until it is ready */
	if (vboot_data)
		memcpy(vboot_data->mehh, hash, len*sizeof(u32));
	else
		memcpy(me_hash_saved, hash, len*sizeof(u32));
}
