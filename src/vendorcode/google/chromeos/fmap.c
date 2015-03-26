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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <console/console.h>
#include <cbfs.h>
#include "fmap.h"

static int is_fmap_signature_valid(const struct fmap *fmap)
{
	const char reversed_sig[] = FMAP_REVERSED_SIGNATURE;
	const char *p2 = reversed_sig + sizeof(FMAP_REVERSED_SIGNATURE) - 2;
	const char *p1 = (char *)fmap;

	while (p2 >= reversed_sig)
		if (*p1++ != *p2--) {
			printk(BIOS_ERR, "No FMAP found at %p.\n", fmap);
			return 1;
		}

	printk(BIOS_DEBUG, "FMAP: Found \"%s\" version %d.%d at %p.\n",
	       fmap->name, fmap->ver_major, fmap->ver_minor, fmap);
	printk(BIOS_DEBUG, "FMAP: base = %llx size = %x #areas = %d\n",
	       (unsigned long long)fmap->base, fmap->size, fmap->nareas);

	return 0;
}

/* Find FMAP data structure in ROM.
 * See http://code.google.com/p/flashmap/ for more information on FMAP.
 */
const struct fmap *fmap_find(void)
{
	/* FIXME: Get rid of the hard codes. The "easy" way would be to
	 * do a binary search, but since ROM accesses are slow, we don't
	 * want to spend a lot of time looking for the FMAP. An elegant
	 * solution would be to store a pointer to the FMAP in the CBFS
	 * master header; that would require some more changes to cbfstool
	 * and possibly cros_bundle_firmware.
	 */
	const struct fmap *fmap;
	struct cbfs_media media;
	size_t size;

	if (init_default_cbfs_media(&media)) {
		printk(BIOS_ERR, "failed to init default cbfs media\n");
		return NULL;
	}

	media.open(&media);
	fmap = media.map(&media, CONFIG_FLASHMAP_OFFSET, sizeof(*fmap));

	if (fmap == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		printk(BIOS_ERR, "failed to map FMAP header\n");
		media.close(&media);
		return NULL;
	}

	if (is_fmap_signature_valid(fmap)) {
		media.unmap(&media, fmap);
		media.close(&media);
		return NULL;
	}

	size = sizeof(*fmap) + sizeof(struct fmap_area) * fmap->nareas;
	media.unmap(&media, fmap);
	fmap = media.map(&media, CONFIG_FLASHMAP_OFFSET, size);

	if (fmap == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		printk(BIOS_ERR, "failed to map FMAP (size=%zu)\n", size);
		media.unmap(&media, fmap);
		media.close(&media);
		return NULL;
	}

	media.close(&media);
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
	MAYBE_STATIC const struct fmap *fmap = NULL;
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
		printk(BIOS_DEBUG, "FMAP: %s base at %p\n", name, base);
	} else {
#if CONFIG_ARCH_X86
		base = (void *)(0 - CONFIG_ROM_SIZE);
		printk(BIOS_WARNING, "FMAP: No valid base address, using"
				" 0x%p\n", base);
#endif
	}

	*pointer = (void*) ((uintptr_t)base + area->offset);
	printk(BIOS_DEBUG, "FMAP: %s at %p (offset %x)\n",
	       name, *pointer, area->offset);
	return area->size;
}
