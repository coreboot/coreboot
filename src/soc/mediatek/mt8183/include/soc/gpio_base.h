/*
 * This file is part of the coreboot project.
 *
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
#ifndef SOC_MEDIATEK_MT8183_GPIO_BASE_H
#define SOC_MEDIATEK_MT8183_GPIO_BASE_H

#include <stdint.h>

typedef union {
	u32 raw;
	struct {
		u32 id     : 8;
		u32 flag   : 3;
		u32 bit    : 5;
		u32 base   : 8;
		u32 offset : 8;
	};
} gpio_t;

#endif
