/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
 * Copyright (C) 2018-2019 Eltan B.V.
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

#include <soc/ramstage.h>
#include <console/console.h>
#include <include/cbfs.h>
#include "mainboard.h"

static char logo_data[1 * MiB];
static size_t logo_data_sz = 0;

void *load_logo(size_t *logo_size)
{
	const char *filename = "logo.bmp";

	if (logo_data_sz != 0) {
		if (logo_size)
			*logo_size = logo_data_sz;
		return (void *)logo_data;
	}

	logo_data_sz = cbfs_boot_load_file(filename, logo_data,
				sizeof(logo_data), CBFS_TYPE_RAW);
	if (logo_data_sz == 0)
		return NULL;

	if (logo_size)
		*logo_size = logo_data_sz;

	printk(BIOS_DEBUG, "Found a Logo of %zu bytes after decompression\n",
		logo_data_sz);

	return (void *)logo_data;
}
