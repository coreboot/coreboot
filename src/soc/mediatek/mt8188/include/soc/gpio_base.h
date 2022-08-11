/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.2
 */

#ifndef SOC_MEDIATEK_MT8188_GPIO_BASE_H
#define SOC_MEDIATEK_MT8188_GPIO_BASE_H

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
