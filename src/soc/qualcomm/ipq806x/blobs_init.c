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

#include <cbfs.h>
#include <string.h>
#include <arch/cache.h>

#include <soc/soc_services.h>

#include <console/console.h>

#include "mbn_header.h"

static struct mbn_header *map_ipq_blob(const char *file_name)
{
	struct cbfs_file *blob_file;
	struct mbn_header *blob_mbn;

	blob_file = cbfs_get_file(CBFS_DEFAULT_MEDIA, file_name);
	if (!blob_file)
		return NULL;

	blob_mbn = (struct mbn_header *)((uintptr_t)blob_file +
					 ntohl(blob_file->offset));

	/* some sanity checks on the headers */
	if ((blob_mbn->mbn_version != 3) ||
	    (blob_mbn->mbn_total_size > ntohl(blob_file->len)))
		return NULL;

	return blob_mbn;
}

int initialize_dram(void)
{
	struct mbn_header *cdt_mbn;
	struct mbn_header *ddr_mbn;
	int (*ddr_init_function)(void *cdt_header);

	cdt_mbn = map_ipq_blob("cdt.mbn");
	ddr_mbn = map_ipq_blob("ddr.mbn");

	if (!cdt_mbn || !ddr_mbn) {
		printk(BIOS_ERR, "cdt.mbn: %p, ddr.mbn: %p\n",
		       cdt_mbn, ddr_mbn);
		die("could not find DDR initialization blobs\n");
	}

	/* Actual area where DDR init is going to be running */
	ddr_init_function = (int (*)(void *))ddr_mbn->mbn_destination;

	/* Copy core into the appropriate memory location. */
	memcpy(ddr_init_function, ddr_mbn + 1, ddr_mbn->mbn_total_size);
	cache_sync_instructions();

	if (ddr_init_function(cdt_mbn + 1) < 0) /* Skip mbn header. */
		die("Fail to Initialize DDR\n");

	printk(BIOS_INFO, "DDR initialized\n");

	return 0;
}
