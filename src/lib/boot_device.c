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

#include <boot_device.h>
#include <compiler.h>

void __weak boot_device_init(void)
{
	/* Provide weak do-nothing init. */
}

static int boot_device_subregion(const struct region *sub,
				struct region_device *subrd,
				const struct region_device *parent)
{
	if (parent == NULL)
		return -1;

	return rdev_chain(subrd, parent, region_offset(sub), region_sz(sub));
}

int boot_device_ro_subregion(const struct region *sub,
				struct region_device *subrd)
{
	/* Ensure boot device has been initialized at least once. */
	boot_device_init();

	return boot_device_subregion(sub, subrd, boot_device_ro());
}

int boot_device_rw_subregion(const struct region *sub,
				struct region_device *subrd)
{
	/* Ensure boot device has been initialized at least once. */
	boot_device_init();

	return boot_device_subregion(sub, subrd, boot_device_rw());
}
