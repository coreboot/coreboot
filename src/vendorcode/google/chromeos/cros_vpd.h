/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __CROS_VPD_H__
#define __CROS_VPD_H__

#define CROS_VPD_REGION_NAME "region"
#define CROS_VPD_WIFI_SAR_NAME "wifi_sar"

/*
 * Reads VPD string value by key.
 *
 * Reads in at most one less than size characters from VPD and stores them
 * into buffer. A terminating null byte ('\0') is stored after the last
 * character in the buffer.
 *
 * Returns NULL if key is not found, otherwise buffer.
 */
char *cros_vpd_gets(const char *key, char *buffer, int size);

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

const void *cros_vpd_find(const char *key, int *size);

#endif  /* __CROS_VPD_H__ */
