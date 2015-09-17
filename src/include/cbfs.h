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

#ifndef _CBFS_H_
#define _CBFS_H_

#include <commonlib/cbfs_serialized.h>
#include <commonlib/region.h>
#include <program_loading.h>

/*
 * CBFS operations consist of the following concepts:
 * - region_device for the boot media
 * - cbfsd which is a descriptor for representing a cbfs instance
 */

/* Object representing cbfs files. */
struct cbfsf;

/***********************************************
 * Perform CBFS operations on the boot device. *
 ***********************************************/

/* Return mapping of option rom found in boot device. NULL on error. */
void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device);
/* Load stage by name into memory. Returns entry address on success. NULL on
 * failure. */
void *cbfs_boot_load_stage_by_name(const char *name);
/* Locate file by name and optional type. Return 0 on success. < 0 on error. */
int cbfs_boot_locate(struct cbfsf *fh, const char *name, uint32_t *type);
/* Map file into memory leaking the mapping. Only should be used when
 * leaking mappings are a no-op. Returns NULL on error, else returns
 * the mapping and sets the size of the file. */
void *cbfs_boot_map_with_leak(const char *name, uint32_t type, size_t *size);

/* Load stage into memory filling in prog. Return 0 on success. < 0 on error. */
int cbfs_prog_stage_load(struct prog *prog);

/* Locate file by name and optional type. Returns 0 on succcess else < 0 on
 * error.*/
int cbfs_locate(struct cbfsf *fh, const struct region_device *cbfs,
		const char *name, uint32_t *type);

/*****************************************************************
 * Support structures and functions. Direct field access should  *
 * only be done by implementers of cbfs regions -- Not the above *
 * API.                                                          *
 *****************************************************************/

struct cbfsf {
	struct region_device metadata;
	struct region_device data;
};

static inline void cbfs_file_data(struct region_device *data,
					const struct cbfsf *file)
{
	rdev_chain(data, &file->data, 0, region_device_sz(&file->data));
}

/* The cbfs_props struct describes the properties associated with a CBFS. */
struct cbfs_props {
	/* CBFS starts at the following offset within the boot region. */
	size_t offset;
	/* CBFS size. */
	size_t size;
};

/* Return < 0 on error otherwise props are filled out accordingly. */
int cbfs_boot_region_properties(struct cbfs_props *props);

#endif
