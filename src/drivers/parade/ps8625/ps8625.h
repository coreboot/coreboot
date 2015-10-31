/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef __PS8625_H__
#define __PS8625_H__

struct parade_write {
	uint8_t offset;
	uint8_t reg;
	uint8_t val;
};

void parade_ps8625_bridge_setup(unsigned bus, unsigned chip_base,
				const struct parade_write *,
				int parade_write_count);

#endif
