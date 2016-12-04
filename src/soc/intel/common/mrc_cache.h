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

enum {
	MRC_TRAINING_DATA,
	MRC_VARIABLE_DATA,
};

/*
 * It's up to the caller to decide when to retrieve and stash data. There is
 * differentiation on recovery mode CONFIG_HAS_RECOVERY_MRC_CACHE, but that's
 * only for locating where to retrieve and save the data. If a platform doesn't
 * want to update the data then it shouldn't stash the data for saving.
 * Similarly, if the platform doesn't need the data for booting because of a
 * policy don't request the data.
 */

/* Get and stash data for saving provided the type passed in. The functions
 * return < 0 on error, 0 on success. */
int mrc_cache_get_current(int type, uint32_t version,
				struct region_device *rdev);
int mrc_cache_stash_data(int type, uint32_t version, const void *data,
			size_t size);

#endif /* _COMMON_MRC_CACHE_H_ */
