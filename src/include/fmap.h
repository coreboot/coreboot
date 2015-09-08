/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef _FMAP_H_
#define _FMAP_H_

#include <commonlib/region.h>
#include <commonlib/fmap_serialized.h>

/* Locate the fmap directory. Return 0 on success, < 0 on error. */
int find_fmap_directory(struct region_device *fmrd);

/* Locate the named area in the fmap and fill in a region device representing
 * that area. The region is a sub-region of the readonly boot media. Return
 * 0 on success, < 0 on error. */
int fmap_locate_area_as_rdev(const char *name, struct region_device *area);

/* Locate the named area in the fmap and fill in a region with the
 * offset and size of that area within the boot media. Return 0 on success,
 * < 0 on error. */
int fmap_locate_area(const char *name, struct region *r);

/* Find fmap area name by offset and size.
 * Return 0 on success, < 0 on error. */
int fmap_find_region_name(const struct region * const ar,
	char name[FMAP_STRLEN]);
#endif
