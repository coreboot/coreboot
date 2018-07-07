/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Philipp Hug <philipp@hug.cx>
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

#include <soc/sdram.h>

void sdram_init(void)
{
	// TODO: implement
}

size_t sdram_size_mb(void)
{
	static size_t size_mb = 0;

	if (!size_mb) {
		// TODO: implement
		size_mb = 8 * 1024;
	}

	return size_mb;
}
