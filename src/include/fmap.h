/*
 * This file is part of the coreboot project.
 *
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

#ifndef _FMAP_H_
#define _FMAP_H_

#include <commonlib/bsd/fmap_serialized.h>
#include <commonlib/region.h>

/* Locate the named area in the fmap and fill in a region device representing
 * that area. The region is a sub-region of the readonly boot media. Return
 * 0 on success, < 0 on error. */
int fmap_locate_area_as_rdev(const char *name, struct region_device *area);

/* Just like fmap_locate_area_as_rdev(), however the region device is
 * created from the RW boot device. */
int fmap_locate_area_as_rdev_rw(const char *name, struct region_device *area);

/* Locate the named area in the fmap and fill in a region with the
 * offset and size of that area within the boot media. Return 0 on success,
 * < 0 on error. */
int fmap_locate_area(const char *name, struct region *r);

/* Find fmap area name by offset and size.
 * Return 0 on success, < 0 on error. */
int fmap_find_region_name(const struct region * const ar,
	char name[FMAP_STRLEN]);

/* Read fmap area into provided buffer.
 * Return size read on success, < 0 on error. */
ssize_t fmap_read_area(const char *name, void *buffer, size_t size);

/* Write provided buffer into fmap area.
 * Return size written on success, < 0 on error. */
ssize_t fmap_overwrite_area(const char *name, const void *buffer, size_t size);

/* Get offset of FMAP in flash. */
uint64_t get_fmap_flash_offset(void);

#endif
