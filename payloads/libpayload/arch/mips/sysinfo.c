/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <libpayload-config.h>
#include <libpayload.h>
#include <coreboot_tables.h>
#include <multiboot_tables.h>

/*
 * This is a global structure that is used through the library - we set it
 * up initially with some dummy values - hopefully they will be overridden.
 */
struct sysinfo_t lib_sysinfo = {
	.cpu_khz = 200,
};

int lib_get_sysinfo(void)
{
	int ret;

	/* Get the CPU speed (for delays). */
	lib_sysinfo.cpu_khz = get_cpu_speed();

	/* Get information from the coreboot tables,
	 * if they exist */
	ret = get_coreboot_info(&lib_sysinfo);

	/* If we can't get a good memory range, use the default. */
	if (!lib_sysinfo.n_memranges) {
		lib_sysinfo.n_memranges = 1;
		lib_sysinfo.memrange[0].base = 0;
		lib_sysinfo.memrange[0].size = 1024 * 1024;
		lib_sysinfo.memrange[0].type = CB_MEM_RAM;
	}

	return ret;
}
