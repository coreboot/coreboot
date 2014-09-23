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

static void *load_ipq_blob(const char *file_name)
{
	struct cbfs_file *blob_file;
	struct mbn_header *blob_mbn;
	void *blob_dest;

	blob_file = cbfs_get_file(CBFS_DEFAULT_MEDIA, file_name);
	if (!blob_file)
		return NULL;

	blob_mbn = (struct mbn_header *)((uintptr_t)blob_file +
					 ntohl(blob_file->offset));

	/* some sanity checks on the headers */
	if ((blob_mbn->mbn_version != 3) ||
	    (blob_mbn->mbn_total_size > ntohl(blob_file->len)))
		return NULL;

	blob_dest = (void *) blob_mbn->mbn_destination;
	if (blob_mbn->mbn_destination) {
		/* Copy the blob to the appropriate memory location. */
		memcpy(blob_dest, blob_mbn + 1, blob_mbn->mbn_total_size);
		cache_sync_instructions();
		return blob_dest;
	}

	/*
	 * The blob did not have to be relocated, return its address in CBFS
	 * cache.
	 */
	return blob_mbn + 1;
}

#ifdef __PRE_RAM__

int initialize_dram(void)
{
	void *cdt;
	int (*ddr_init_function)(void *cdt_header);

	cdt = load_ipq_blob("cdt.mbn");
	ddr_init_function = load_ipq_blob("ddr.mbn");

	if (!cdt || !ddr_init_function) {
		printk(BIOS_ERR, "cdt: %p, ddr_init_function: %p\n",
		       cdt, ddr_init_function);
		die("could not find DDR initialization blobs\n");
	}

	if (ddr_init_function(cdt) < 0)
		die("Fail to Initialize DDR\n");

	printk(BIOS_INFO, "DDR initialized\n");

	return 0;
}

#else  /* __PRE_RAM__ */

void start_tzbsp(void)
{
	void *tzbsp = load_ipq_blob("tz.mbn");

	if (!tzbsp)
		die("could not find or map TZBSP\n");

	tz_init_wrapper(0, 0, tzbsp);
}

#endif  /* !__PRE_RAM__ */
