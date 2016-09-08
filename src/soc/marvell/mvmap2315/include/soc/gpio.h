/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_GPIO_H__
#define __SOC_MARVELL_MVMAP2315_GPIO_H__

#include <stdint.h>

#include <types.h>

#define GPIO(b, i)		((gpio_t){.bank = GPIO_##b, .idx = i})
#define GET_GPIO_PAD(gpio)	((gpio.bank * 32) + gpio.idx + 160)

struct mvmap2315_gpio_regs {
	u32 plr;
	u32 pdr;
	u32 psr;
	u32 pcr;
	u32 hripr;
	u32 lfipr;
	u32 isr;
	u32 sdr;
	u32 cdr;
	u32 shripr;
	u32 chripr;
	u32 slfipr;
	u32 clfipr;
	u32 olr;
	u32 dwer;
	u32 imr;
	u32 rev0;
	u32 rev1;
	u32 simr;
	u32 cimr;
	u32 iter0;
	u32 iter1;
	u32 iter2;
	u32 iter3;
	u32 iter4;
	u32 iter5;
	u32 iter6;
	u32 iter7;
	u32 iter8;
	u32 iter9;
	u32 iter10;
	u32 iter11;
	u32 iter12;
	u32 iter13;
	u32 iter14;
	u32 iter15;
	u32 iter16;
	u32 iter17;
	u32 iter18;
	u32 iter19;
	u32 iter20;
	u32 iter21;
	u32 iter22;
	u32 iter23;
};

check_member(mvmap2315_gpio_regs, iter23, 0xac);

typedef union {
	u32 raw;
	struct {
		u16 port;
		union {
			struct {
				u16 num : 5;
				u16 reserved1 : 11;
			};
			struct {
				u16 idx : 3;
				u16 bank : 2;
				u16 reserved2 : 11;
			};
		};
	};
} gpio_t;

enum {
	GPIO_F = 0,
	GPIO_G = 1,
	GPIO_H = 2,
};

#endif /* __SOC_MARVELL_MVMAP2315_GPIO_H__ */
