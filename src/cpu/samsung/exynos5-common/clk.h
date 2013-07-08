/*
 * (C) Copyright 2012 The Chromium Authors
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#ifndef __EXYNOS_COMMON_CLK_H_
#define __EXYNOS_COMMON_CLK_H_

#include <types.h>
#include <stdint.h>

enum periph_id;

#define APLL	0
#define MPLL	1
#define EPLL	2
#define HPLL	3
#define VPLL	4
#define BPLL	5

enum pll_src_bit {
	SRC_MPLL = 6,
	SRC_EPLL,
	SRC_VPLL,
};

/* *
 * This structure is to store the src bit, div bit and prediv bit
 * positions of the peripheral clocks of the src and div registers
 */
struct clk_bit_info {
	s8 src_bit;    /* offset in register to clock source field */
	s8 n_src_bits; /* number of bits in 'src_bit' field */
	s8 div_bit;
	s8 prediv_bit;
};

unsigned long get_pll_clk(int pllreg);
unsigned long get_arm_clk(void);
unsigned long get_pwm_clk(void);
unsigned long get_uart_clk(int dev_index);
void set_mmc_clk(int dev_index, unsigned int div);

/**
 * get the clk frequency of the required peripheral
 *
 * @param peripheral	Peripheral id
 *
 * @return frequency of the peripheral clk
 */
unsigned long clock_get_periph_rate(enum periph_id peripheral);

#endif
