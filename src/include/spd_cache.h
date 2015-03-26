/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _SPD_CACHE_H_
#define _SPD_CACHE_H_

#include <stdint.h>

#if IS_ENABLED(CONFIG_SPD_CACHE)
int read_spd_from_cbfs(u8 *buf, int idx);
#else
static inline int read_spd_from_cbfs(u8 *buf, int idx) { return -1; }
#endif

#endif
