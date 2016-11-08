/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _COMMON_MRC_CACHE_H_
#define _COMMON_MRC_CACHE_H_

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_MRC_CACHE	"RW_MRC_CACHE"
#define VARIABLE_MRC_CACHE	"RW_VAR_MRC_CACHE"
#define RECOVERY_MRC_CACHE	"RECOVERY_MRC_CACHE"
#define UNIFIED_MRC_CACHE	"UNIFIED_MRC_CACHE"

/* Wrapper object to save MRC information. */
struct mrc_saved_data {
	uint32_t signature;
	uint32_t size;
	uint32_t checksum;
	uint32_t version;
	uint8_t  data[0];
} __attribute__((packed));

/* Locate the most recently saved MRC data. */
int mrc_cache_get_current(const struct mrc_saved_data **cache);
int mrc_cache_get_current_with_version(const struct mrc_saved_data **cache,
					uint32_t version);
int mrc_cache_get_vardata(const struct mrc_saved_data **cache,
			    uint32_t version);
int mrc_cache_get_current_from_region(const struct mrc_saved_data **cache,
				      uint32_t version,
				      const char *region_name);

/* Stash the resulting MRC data to be saved in non-volatile storage later. */
int mrc_cache_stash_data(const void *data, size_t size);
int mrc_cache_stash_data_with_version(const void *data, size_t size,
					uint32_t version);
int mrc_cache_stash_vardata(const void *data, size_t size, uint32_t version);

#endif /* _COMMON_MRC_CACHE_H_ */
