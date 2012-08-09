/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <console/console.h>
#include "fmap.h"

static int fmap_try_find(void *fmap)
{
	if (!memcmp(fmap, FMAP_SIGNATURE,
	    sizeof(FMAP_SIGNATURE)-1))
		return 1;
	return 0;
}

/* Find FMAP data structure in ROM.
 * See http://code.google.com/p/flashmap/ for more information on FMAP.
 */
const struct fmap *fmap_find(void)
{
	const struct fmap *fmap = NULL;

	/* FIXME: Get rid of the hard codes. The "easy" way would be to
	 * do a binary search, but since ROM accesses are slow, we don't
	 * want to spend a lot of time looking for the FMAP. An elegant
	 * solution would be to store a pointer to the FMAP in the CBFS
	 * master header; that would require some more changes to cbfstool
	 * and possibly cros_bundle_firmware.
	 * FIXME: Use CONFIG_ROMSIZE instead of CONFIG_MRC_CACHE_BASE
	 * (and get rid of CONFIG_MRC_CACHE_BASE), once we are building
	 * coreboot images with ME firmware etc built in instead of just
	 * the CBFS part.
	 */
	if (fmap_try_find((void *)CONFIG_MRC_CACHE_BASE +
						CONFIG_FLASHMAP_OFFSET))
		fmap = (const struct fmap  *)(CONFIG_MRC_CACHE_BASE +
						CONFIG_FLASHMAP_OFFSET);
	if (fmap) {
		printk(BIOS_DEBUG, "FMAP: Found \"%s\" version %d.%d at %p.\n",
			fmap->name, fmap->ver_major, fmap->ver_minor, fmap);
		printk(BIOS_DEBUG, "FMAP: base = %llx size = %x #areas = %d\n",
			(unsigned long long)fmap->base, fmap->size,
			fmap->nareas);
	} else
		printk(BIOS_DEBUG, "No FMAP found.\n");

	return fmap;
}

const struct fmap_area *find_fmap_area(const struct fmap *fmap,
							const char name[])
{
	const struct fmap_area *area = NULL;

	if (fmap) {
		int i;
		for (i = 0; i < fmap->nareas; i++) {
			if (!strcmp((const char *)fmap->areas[i].name, name)) {
				area = &fmap->areas[i];
				break;
			}
		}
	}

	if (area) {
		printk(BIOS_DEBUG, "FMAP: area %s found\n", name);
		printk(BIOS_DEBUG, "FMAP:   offset: %x\n", area->offset);
		printk(BIOS_DEBUG, "FMAP:   size:   %d bytes\n", area->size);
	} else {
		printk(BIOS_DEBUG, "FMAP: area %s not found\n", name);
	}

	return area;
}

int find_fmap_entry(const char name[], void **pointer)
{
#ifndef __PRE_RAM__
	static
#endif
	const struct fmap *fmap = NULL;
	const struct fmap_area *area;
	void *base = NULL;

	if (!fmap)
		fmap = fmap_find();

	area = find_fmap_area(fmap, name);

	if (!area)
		return -1;

	/* Right now cros_bundle_firmware does not write a valid
	 * base address into the FMAP. Hence, if base is 0, assume
	 * 4GB-8MB as base address.
	 */
	if (fmap->base) {
		base = (void *)(unsigned long)fmap->base;
	} else {
		printk(BIOS_WARNING, "FMAP: No valid base address, using"
				" 0x%08x\n", CONFIG_MRC_CACHE_BASE);
		base = (void *)CONFIG_MRC_CACHE_BASE;
	}

	*pointer = base + area->offset;

	return area->size;
}
