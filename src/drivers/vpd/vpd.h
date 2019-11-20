/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __VPD_H__
#define __VPD_H__

#include <types.h>

#define GOOGLE_VPD_2_0_OFFSET 0x600

enum vpd_region {
	VPD_ANY = 0,
	VPD_RO = 1,
	VPD_RW = 2
};

/* VPD 2.0 data blob structure */
struct vpd_blob {
	bool initialized;
	uint8_t  *ro_base;
	uint32_t ro_size;
	uint8_t  *rw_base;
	uint32_t rw_size;
};
extern struct vpd_blob g_vpd_blob;

/*
 * This function loads g_vpd_blob global variable.
 * The variable is initialized if it was not.
 */
const struct vpd_blob *vpd_load_blob(void);

/*
 * This function gets the base address and size of
 * buffers for RO_VPD/RW_VPD binary blobs, and sets
 * the struct.
 */
void vpd_get_buffers(struct vpd_blob *blob);

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

/*
 * Find value of boolean type vpd key.
 *
 * During the process, necessary checking is done, such as making
 * sure the value length is 1, and value is either '1' or '0'.
 */
bool vpd_get_bool(const char *key, enum vpd_region region,
	uint8_t *val);

#endif  /* __VPD_H__ */
