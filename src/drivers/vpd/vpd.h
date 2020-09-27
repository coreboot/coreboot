/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __VPD_H__
#define __VPD_H__

#include <types.h>

#define GOOGLE_VPD_2_0_OFFSET 0x600

enum vpd_region {
	VPD_RO,
	VPD_RW,
	VPD_RO_THEN_RW,
	VPD_RW_THEN_RO
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

/*
 * Find value of boolean type vpd key.
 *
 * During the process, necessary checking is done, such as making
 * sure the value length is 1, and value is either '1' or '0'.
 */
bool vpd_get_bool(const char *key, enum vpd_region region,
	uint8_t *val);

/*
 * Find value of integer type by vpd key.
 *
 * Expects to find a decimal string, trailing chars are ignored.
 * Returns true if the key is found and the value is not too long and
 * starts with a decimal digit.
 */
bool vpd_get_int(const char *key, enum vpd_region region, int *val);

#endif  /* __VPD_H__ */
