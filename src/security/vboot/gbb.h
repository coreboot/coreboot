/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#ifndef __SECURITY_VBOOT_GBB_H__
#define __SECURITY_VBOOT_GBB_H__

#include <stdint.h>

/* In order to use GBB_FLAG_* macros from vboot, include gbb_header.h. */

/*
 * Read flags field from GBB header.
 * Return value:
 * Success: 32-bit unsigned integer representing flags field from GBB header.
 * Error  : 0
 */
uint32_t gbb_get_flags(void);

/*
 * Check if given flag is set in the flags field in GBB header.
 * Return value:
 * true: Flag is set.
 * false: Flag is not set or failure to read GBB flags.
 */
bool gbb_is_flag_set(uint32_t flag);

#endif /* __SECURITY_VBOOT_GBB_H__ */
