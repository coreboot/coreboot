/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <cbmem.h>
#include <fsp/api.h>

const struct cbmem_entry *fsp_load_logo(UINT32 *logo_ptr, UINT32 *logo_size)
{
	const struct cbmem_entry *logo_entry = NULL;
	void *logo_buffer;

	logo_entry = cbmem_entry_add(CBMEM_ID_FSP_LOGO, 1 * MiB);
	if (logo_entry) {
		logo_buffer = cbmem_entry_start(logo_entry);
		if (logo_buffer) {
			*logo_size = cbfs_load("logo.bmp", (void *)logo_buffer,
						1 * MiB);
			if (*logo_size)
				*logo_ptr = (UINT32)logo_buffer;
		}
	}
	return (logo_entry);
}
