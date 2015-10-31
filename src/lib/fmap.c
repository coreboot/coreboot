/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012-2015 Google Inc.
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

#include <boot_device.h>
#include <console/console.h>
#include <fmap.h>
#include <commonlib/fmap_serialized.h>
#include <stddef.h>
#include <string.h>

/*
 * See http://code.google.com/p/flashmap/ for more information on FMAP.
 */

int find_fmap_directory(struct region_device *fmrd)
{
	const struct region_device *boot;
	struct fmap *fmap;
	size_t fmap_size;
	size_t offset = CONFIG_FLASHMAP_OFFSET;

	boot_device_init();
	boot = boot_device_ro();

	if (boot == NULL)
		return -1;

	fmap_size = sizeof(struct fmap);

	fmap = rdev_mmap(boot, offset, fmap_size);

	if (fmap == NULL)
		return -1;

	if (memcmp(fmap->signature, FMAP_SIGNATURE, sizeof(fmap->signature))) {
		printk(BIOS_DEBUG, "No FMAP found at %zx offset.\n", offset);
		rdev_munmap(boot, fmap);
		return -1;
	}

	printk(BIOS_DEBUG, "FMAP: Found \"%s\" version %d.%d at %zx.\n",
	       fmap->name, fmap->ver_major, fmap->ver_minor, offset);
	printk(BIOS_DEBUG, "FMAP: base = %llx size = %x #areas = %d\n",
	       (long long)fmap->base, fmap->size, fmap->nareas);

	fmap_size += fmap->nareas * sizeof(struct fmap_area);

	rdev_munmap(boot, fmap);

	return rdev_chain(fmrd, boot, offset, fmap_size);
}

int fmap_locate_area_as_rdev(const char *name, struct region_device *area)
{
	struct region ar;

	if (fmap_locate_area(name, &ar))
		return -1;

	return boot_device_ro_subregion(&ar, area);
}

int fmap_locate_area(const char *name, struct region *ar)
{
	struct region_device fmrd;
	size_t offset;

	if (find_fmap_directory(&fmrd))
		return -1;

	/* Start reading the areas just after fmap header. */
	offset = sizeof(struct fmap);

	while (1) {
		struct fmap_area *area;

		area = rdev_mmap(&fmrd, offset, sizeof(*area));

		if (area == NULL)
			return -1;

		if (strcmp((const char *)area->name, name)) {
			rdev_munmap(&fmrd, area);
			offset += sizeof(struct fmap_area);
			continue;
		}

		printk(BIOS_DEBUG, "FMAP: area %s found\n", name);
		printk(BIOS_DEBUG, "FMAP:   offset: %x\n", area->offset);
		printk(BIOS_DEBUG, "FMAP:   size:   %d bytes\n", area->size);

		ar->offset = area->offset;
		ar->size = area->size;

		rdev_munmap(&fmrd, area);

		return 0;
	}

	printk(BIOS_DEBUG, "FMAP: area %s not found\n", name);

	return -1;
}

int fmap_find_region_name(const struct region * const ar,
	char name[FMAP_STRLEN])
{
	struct region_device fmrd;
	size_t offset;

	if (find_fmap_directory(&fmrd))
		return -1;

	/* Start reading the areas just after fmap header. */
	offset = sizeof(struct fmap);

	while (1) {
		struct fmap_area *area;

		area = rdev_mmap(&fmrd, offset, sizeof(*area));

		if (area == NULL)
			return -1;

		if ((ar->offset != area->offset) ||
		    (ar->size != area->size)) {
			rdev_munmap(&fmrd, area);
			offset += sizeof(struct fmap_area);
			continue;
		}

		printk(BIOS_DEBUG, "FMAP: area (%zx, %zx) found, named %s\n",
			ar->offset, ar->size, area->name);

		memcpy(name, area->name, FMAP_STRLEN);

		rdev_munmap(&fmrd, area);

		return 0;
	}

	printk(BIOS_DEBUG, "FMAP: area (%zx, %zx) not found\n",
		ar->offset, ar->size);

	return -1;
}
