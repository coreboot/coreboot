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

#ifndef _COMMONLIB_CBFS_H_
#define _COMMONLIB_CBFS_H_

#include <commonlib/cbfs_serialized.h>
#include <commonlib/region.h>
/* TODO: remove me! This is for vboot_handoff.c's benefit. */
#define NEED_VB20_INTERNALS
#include <vb2_api.h>

/* Object representing cbfs files. */
struct cbfsf {
	struct region_device metadata;
	struct region_device data;
};

/* Locate file by name and optional type. Returns 0 on succcess else < 0 on
 * error.*/
int cbfs_locate(struct cbfsf *fh, const struct region_device *cbfs,
		const char *name, uint32_t *type);

static inline void cbfs_file_data(struct region_device *data,
					const struct cbfsf *file)
{
	rdev_chain(data, &file->data, 0, region_device_sz(&file->data));
}

static inline void cbfs_file_metadata(struct region_device *metadata,
					const struct cbfsf *file)
{
	rdev_chain(metadata, &file->metadata, 0,
			region_device_sz(&file->metadata));
}

/*
 * Provide a handle to each cbfs file within a cbfs. The prev pointer represents
 * the previous file (NULL on first invocation). The next object gets filled
 * out with the next file. This returns < 0 on error, 0 on finding the next
 * file, and > 0 at end of cbfs.
 */
int cbfs_for_each_file(const struct region_device *cbfs,
			const struct cbfsf *prev, struct cbfsf *fh);

/*
 * Return the offset for each CBFS attribute in a CBFS file metadata region.
 * The metadata must already be fully mapped by the caller. Will return the
 * offset (relative to the start of the metadata) or 0 when there are no
 * further attributes. Should be called with 0 to begin, then always with
 * the previously returned value until it returns 0.
 */
size_t cbfs_for_each_attr(void *metadata, size_t metadata_size,
			  size_t last_offset);

/*
 * Find out the decompression algorithm and decompressed size of a non-stage
 * CBFS file (by parsing its metadata attributes), and return them with
 * out-parameters. Returns 0 on success and < 0 on error.
 */
int cbfsf_decompression_info(struct cbfsf *fh, uint32_t *algo, size_t *size);

/*
 * Perform the vb2 hash over the CBFS region skipping empty file contents.
 * Caller is responsible for providing the hash algorithm as well as storage
 * for the final digest. Return 0 on success or non-zero on error.
 */
int cbfs_vb2_hash_contents(const struct region_device *cbfs,
				enum vb2_hash_algorithm hash_alg, void *digest,
				size_t digest_sz);

#endif
