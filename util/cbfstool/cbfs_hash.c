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

#include <commonlib/cbfs.h>
#include <commonlib/region.h>

int cbfs_calculate_hash(void *cbfs, size_t cbfs_sz,
			enum vb2_hash_algorithm hash_algo,
			void *digest, size_t digest_sz);

int cbfs_calculate_hash(void *cbfs, size_t cbfs_sz,
			enum vb2_hash_algorithm hash_algo,
			void *digest, size_t digest_sz)
{
	struct mem_region_device mdev;

	mem_region_device_init(&mdev, cbfs, cbfs_sz);

	return cbfs_vb2_hash_contents(&mdev.rdev,
				hash_algo, digest, digest_sz);
}
