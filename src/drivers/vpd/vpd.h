/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __VPD_H__
#define __VPD_H__

enum vpd_region {
	VPD_ANY = 0,
	VPD_RO = 1,
	VPD_RW = 2
};
/*
 * Reads VPD string value by key.
 *
 * Reads in at most one less than size characters from VPD and stores them
 * into buffer. A terminating null byte ('\0') is stored after the last
 * character in the buffer.
 *
 * Returns NULL if key is not found, otherwise buffer.
 */
char *vpd_gets(const char *key, char *buffer, int size, enum vpd_region region);

/*
 * Find VPD value by key.
 *
 * Searches for a VPD entry in the VPD cache. If found, places the size of the
 * entry into '*size' and returns the pointer to the entry data.
 *
 * This function presumes that VPD is cached in DRAM (which is the case in the
 * current implementation) and as such returns the pointer into the cache. The
 * user is not supposed to modify the data, and does not have to free the
 * memory.
 *
 * Returns NULL if key is not found.
 */

const void *vpd_find(const char *key, int *size, enum vpd_region region);

#endif  /* __VPD_H__ */
