/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <boot_device.h>

/* Maps directly to qemu memory mapped space of 0x10000 up to rom size. */
static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_INIT((void *)0x10000, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}
