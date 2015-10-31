/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google, Inc.
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

#ifndef _BCD_H_
#define _BCD_H_

#include <stdint.h>

static inline uint8_t bcd2bin(uint8_t val)
{
	return ((val >> 4) & 0xf) * 10 + (val & 0xf);
}

static inline uint8_t bin2bcd(uint8_t val)
{
	return ((val / 10) << 4) | (val % 10);
}

#endif /* _BCD_H_ */
