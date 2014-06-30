/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <string.h>
#include <console/console.h>
#include <arch/io.h>
#include <cbfs.h>
#include <soc/addressmap.h>
#include "ccplex.h"
#include "mc.h"

#define MTS_FILE_NAME "mts"

int ccplex_load_mts(void)
{
	struct cbfs_file file;
	ssize_t offset;
	size_t nread;
	/*
	 * MTS location is hard coded to this magic address. The hardware will
	 * take the MTS from this location and place it in the final resting
	 * place in the carveout region.
	 */
	void * const mts = (void *)(uintptr_t)MTS_LOAD_ADDRESS;
	struct cbfs_media *media = CBFS_DEFAULT_MEDIA;

	offset = cbfs_locate_file(media, &file, MTS_FILE_NAME);
	if (offset < 0) {
		printk(BIOS_DEBUG, "MTS file not found: %s\n", MTS_FILE_NAME);
		return -1;
	}

	/* Read MTS file into the carveout region. */
	nread = cbfs_read(media, mts, offset, file.len);

	if (nread != file.len) {
		printk(BIOS_DEBUG, "MTS bytes read (%zu) != file length(%u)!\n",
			nread, file.len);
		return -1;
	}

	printk(BIOS_DEBUG, "MTS: %zu bytes loaded @ %p\n", nread, mts);

	return 0;
}
