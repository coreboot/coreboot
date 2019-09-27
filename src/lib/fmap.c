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

#include <arch/early_variables.h>
#include <boot_device.h>
#include <console/console.h>
#include <fmap.h>
#include <commonlib/fmap_serialized.h>
#include <stddef.h>
#include <string.h>
#include <cbmem.h>

#include "fmap_config.h"

/*
 * See http://code.google.com/p/flashmap/ for more information on FMAP.
 */

static int fmap_print_once CAR_GLOBAL;
static struct mem_region_device fmap_cache CAR_GLOBAL;

uint64_t get_fmap_flash_offset(void)
{
	return FMAP_OFFSET;
}

static int find_fmap_directory(struct region_device *fmrd)
{
	const struct region_device *boot;
	struct fmap *fmap;
	size_t fmap_size;
	size_t offset = FMAP_OFFSET;

	if (cbmem_possibly_online() && !ENV_SMM) {
		/* Try FMAP cache first */
		const struct mem_region_device *cache;

		cache = car_get_var_ptr(&fmap_cache);
		if (region_device_sz(&cache->rdev))
			return rdev_chain(fmrd, &cache->rdev, 0,
					  region_device_sz(&cache->rdev));
	}

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

	if (!car_get_var(fmap_print_once)) {
		printk(BIOS_DEBUG, "FMAP: Found \"%s\" version %d.%d at %zx.\n",
		       fmap->name, fmap->ver_major, fmap->ver_minor, offset);
		printk(BIOS_DEBUG, "FMAP: base = %llx size = %x #areas = %d\n",
		       (long long)fmap->base, fmap->size, fmap->nareas);
		car_set_var(fmap_print_once, 1);
	}

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

int fmap_locate_area_as_rdev_rw(const char *name, struct region_device *area)
{
	struct region ar;

	if (fmap_locate_area(name, &ar))
		return -1;

	return boot_device_rw_subregion(&ar, area);
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

		printk(BIOS_DEBUG, "FMAP: area %s found @ %x (%d bytes)\n",
		       name, area->offset, area->size);

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

ssize_t fmap_read_area(const char *name, void *buffer, size_t size)
{
	struct region_device rdev;
	if (fmap_locate_area_as_rdev(name, &rdev))
		return -1;
	return rdev_readat(&rdev, buffer, 0,
			MIN(size, region_device_sz(&rdev)));
}

ssize_t fmap_overwrite_area(const char *name, const void *buffer, size_t size)
{
	struct region_device rdev;

	if (fmap_locate_area_as_rdev_rw(name, &rdev))
		return -1;
	if (size > region_device_sz(&rdev))
		return -1;
	if (rdev_eraseat(&rdev, 0, region_device_sz(&rdev)) < 0)
		return -1;
	return rdev_writeat(&rdev, buffer, 0, size);
}

static void fmap_register_cache(int unused)
{
	const struct cbmem_entry *e;
	struct mem_region_device *mdev;

	mdev = car_get_var_ptr(&fmap_cache);

	/* Find the FMAP cache installed by previous stage */
	e = cbmem_entry_find(CBMEM_ID_FMAP);
	/* Don't set fmap_cache so that find_fmap_directory will use regular path */
	if (!e)
		return;

	mem_region_device_ro_init(mdev, cbmem_entry_start(e), cbmem_entry_size(e));
}

/*
 * The main reason to copy the FMAP into CBMEM is to make it available to the
 * OS on every architecture. As side effect use the CBMEM copy as cache.
 */
static void fmap_setup_cache(int unused)
{
	struct region_device fmrd;

	if (find_fmap_directory(&fmrd))
		return;

	/* Reloads the FMAP even on ACPI S3 resume */
	const size_t s = region_device_sz(&fmrd);
	struct fmap *fmap = cbmem_add(CBMEM_ID_FMAP, s);
	if (!fmap) {
		printk(BIOS_ERR, "ERROR: Failed to allocate CBMEM\n");
		return;
	}

	const ssize_t ret = rdev_readat(&fmrd, fmap, 0, s);
	if (ret != s) {
		printk(BIOS_ERR, "ERROR: Failed to read FMAP into CBMEM\n");
		cbmem_entry_remove(cbmem_entry_find(CBMEM_ID_FMAP));
		return;
	}

	/* Finally advertise the cache for the current stage */
	fmap_register_cache(unused);
}

ROMSTAGE_CBMEM_INIT_HOOK(fmap_setup_cache)
RAMSTAGE_CBMEM_INIT_HOOK(fmap_register_cache)
POSTCAR_CBMEM_INIT_HOOK(fmap_register_cache)
