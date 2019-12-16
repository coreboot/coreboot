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
 */

#include <cbfs.h>
#include <cbmem.h>
#include <soc/ramstage.h>

const struct cbmem_entry *fsp_load_logo(UINT32 *logo_ptr, UINT32 *logo_size)
{
	const struct cbmem_entry *logo_entry = NULL;
	void *logo_buffer;

	logo_entry = cbmem_entry_add(CBMEM_ID_FSP_LOGO, 1 * MiB);
	if (logo_entry) {
		logo_buffer = cbmem_entry_start(logo_entry);
		if (logo_buffer) {
			*logo_size = cbfs_boot_load_file("logo.bmp", (void *)logo_buffer,
							1 * MiB, CBFS_TYPE_RAW);
			if (logo_size)
				*logo_ptr = (UINT32)logo_buffer;
		}
	}
	return (logo_entry);
}
