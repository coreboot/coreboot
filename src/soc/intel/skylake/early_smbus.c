/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <console/console.h>
#include <device/smbus_def.h>
#include <device/early_smbus.h>
#include <soc/smbus.h>
#include <soc/iomap.h>

u8 smbus_read_byte(u32 smbus_dev, u8 addr, u8 offset)
{
	return do_smbus_read_byte(SMBUS_BASE_ADDRESS, addr, offset);
}

u8 smbus_write_byte(u32 smbus_dev, u8 addr, u8 offset, u8 value)
{
	return do_smbus_write_byte(SMBUS_BASE_ADDRESS, addr, offset, value);
}
