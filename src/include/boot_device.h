/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef _BOOT_DEVICE_H_
#define _BOOT_DEVICE_H_

#include <commonlib/region.h>

/* Return the region_device for the read-only boot device. */
const struct region_device *boot_device_ro(void);

/*
 * Create a sub-region of the read-only boot device.
 * Returns 0 on success, < 0 on error.
 */
int boot_device_ro_subregion(const struct region *sub,
				struct region_device *subrd);

/*
 * Initialize the boot device. This may be called multiple times within
 * a stage so boot device implementations should account for this behavior.
 **/
void boot_device_init(void);

#endif /* _BOOT_DEVICE_H_ */
