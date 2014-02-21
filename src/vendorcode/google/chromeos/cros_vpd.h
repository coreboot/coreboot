/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __CROS_VPD_H__
#define __CROS_VPD_H__

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

#endif  /* __CROS_VPD_H__ */
