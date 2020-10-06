/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COMMON_MRC_CACHE_H_
#define _COMMON_MRC_CACHE_H_

#include <stddef.h>
#include <stdint.h>
#include <commonlib/region.h>

enum {
	MRC_TRAINING_DATA,
	MRC_VARIABLE_DATA,
};

/*
 * It's up to the caller to decide when to retrieve and stash data. There is
 * differentiation on recovery mode CONFIG(HAS_RECOVERY_MRC_CACHE), but that's
 * only for locating where to retrieve and save the data. If a platform doesn't
 * want to update the data then it shouldn't stash the data for saving.
 * Similarly, if the platform doesn't need the data for booting because of a
 * policy don't request the data.
 */

/* Get and stash data for saving provided the type passed in. */

/**
 * mrc_cache_load_current
 *
 * Fill in the buffer with the latest slot data.  This will be a
 * common entry point for ARM platforms.  Returns < 0 on error, size
 * of the returned data on success.
 */
ssize_t mrc_cache_load_current(int type, uint32_t version, void *buffer,
			       size_t buffer_size);
/**
 * mrc_cache_mmap_leak
 *
 * Return a pointer to a buffer with the latest slot data.  An mmap
 * will be executed (without a matching unmap).  This will be a common
 * entry point for platforms where mmap is considered a noop, like x86
 */
void *mrc_cache_current_mmap_leak(int type, uint32_t version,
				  size_t *data_size);
/**
 * Returns < 0 on error, 0 on success.
 */
int mrc_cache_stash_data(int type, uint32_t version, const void *data,
			 size_t size);

#endif /* _COMMON_MRC_CACHE_H_ */
