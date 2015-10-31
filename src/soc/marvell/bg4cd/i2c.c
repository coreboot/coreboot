/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <device/i2c.h>
#include <soc/i2c.h>

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int seg_count)
{
	return 0;
}

void i2c_init(unsigned int bus, unsigned int hz)
{
}
