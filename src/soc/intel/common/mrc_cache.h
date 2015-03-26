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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _COMMON_MRC_CACHE_H_
#define _COMMON_MRC_CACHE_H_

#include <stddef.h>
#include <stdint.h>

/* Wrapper object to save MRC information. */
struct mrc_saved_data {
	uint32_t signature;
	uint32_t size;
	uint32_t checksum;
	uint32_t reserved;
	uint8_t  data[0];
} __attribute__((packed));

/* Locate the most recently saved MRC data. */
int mrc_cache_get_current(const struct mrc_saved_data **cache);

/* Stash the resulting MRC data to be saved in non-volatile storage later. */
int mrc_cache_stash_data(void *data, size_t size);

#endif /* _COMMON_MRC_CACHE_H_ */
