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
 */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <commonlib/cbfs.h>
#include <program_loading.h>

/***********************************************
 * Perform CBFS operations on the boot device. *
 ***********************************************/

/* Return mapping of option ROM found in boot device. NULL on error. */
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
/* Locate file in a specific region of fmap. Return 0 on success. < 0 on error*/
int cbfs_locate_file_in_region(struct cbfsf *fh, const char *region_name,
		const char *name, uint32_t *type);
/* Load an arbitrary type file from CBFS into a buffer. Returns amount of
 * loaded bytes on success or 0 on error. File will get decompressed as
 * necessary.  Same decompression requirements as
 * cbfs_load_and_decompress(). */
size_t cbfs_boot_load_file(const char *name, void *buf, size_t buf_size,
	uint32_t type);
/* Load |in_size| bytes from |rdev| at |offset| to the |buffer_size| bytes
 * large |buffer|, decompressing it according to |compression| in the process.
 * Returns the decompressed file size, or 0 on error.
 * LZMA files will be mapped for decompression. LZ4 files will be decompressed
 * in-place with the buffer size requirements outlined in compression.h. */
size_t cbfs_load_and_decompress(const struct region_device *rdev, size_t offset,
	size_t in_size, void *buffer, size_t buffer_size, uint32_t compression);

/* Return the size and fill base of the memory pstage will occupy after
 * loaded.
 */
size_t cbfs_prog_stage_section(struct prog *pstage, uintptr_t *base);

/* Load stage into memory filling in prog. Return 0 on success. < 0 on error. */
int cbfs_prog_stage_load(struct prog *prog);

/*****************************************************************
 * Support structures and functions. Direct field access should  *
 * only be done by implementers of cbfs regions -- Not the above *
 * API.                                                          *
 *****************************************************************/

/* The cbfs_props struct describes the properties associated with a CBFS. */
struct cbfs_props {
	/* CBFS starts at the following offset within the boot region. */
	size_t offset;
	/* CBFS size. */
	size_t size;
};

/* Return < 0 on error otherwise props are filled out accordingly. */
int cbfs_boot_region_properties(struct cbfs_props *props);

/* Allow external logic to take action prior to locating a program
 * (stage or payload). */
void cbfs_prepare_program_locate(void);

/* Object used to identify location of current cbfs to use for cbfs_boot_*
 * operations. It's used by cbfs_boot_region_properties() and
 * cbfs_prepare_program_locate(). */
struct cbfs_locator {
	const char *name;
	void (*prepare)(void);
	/* Returns 0 on successful fill of cbfs properties. */
	int (*locate)(struct cbfs_props *props);
};

#endif
