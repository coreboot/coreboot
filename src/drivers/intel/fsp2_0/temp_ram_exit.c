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
 */

#include <arch/romstage.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memrange.h>
#include <cbfs.h>
#include <types.h>

void fsp_temp_ram_exit(void)
{
	struct fsp_header hdr;
	uint32_t status;
	temp_ram_exit_fn  temp_ram_exit;
	struct cbfsf file_desc;
	struct region_device file_data;
	const char *name = CONFIG_FSP_M_CBFS;

	if (cbfs_boot_locate(&file_desc, name, NULL)) {
		printk(BIOS_CRIT, "Could not locate %s in CBFS\n", name);
		die("FSPM not available for CAR Exit!\n");
	}

	cbfs_file_data(&file_data, &file_desc);

	if (fsp_validate_component(&hdr, &file_data) != CB_SUCCESS)
		die("Invalid FSPM header!\n");

	temp_ram_exit = (void *)(hdr.image_base + hdr.temp_ram_exit_entry);
	printk(BIOS_DEBUG, "Calling TempRamExit: %p\n", temp_ram_exit);
	status = temp_ram_exit(NULL);

	if (status != FSP_SUCCESS) {
		printk(BIOS_CRIT, "TempRamExit returned 0x%08x\n", status);
		die("TempRamExit returned an error!\n");
	}
}

void late_car_teardown(void)
{
	fsp_temp_ram_exit();
}
