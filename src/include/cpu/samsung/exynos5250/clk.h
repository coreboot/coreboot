/*
 * (C) Copyright 2012 Samsung Electronics
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

#ifndef __ASM_ARM_ARCH_EXYNOS5_CLK_H__
#define __ASM_ARM_ARCH_EXYNOS5_CLK_H__

#include <asm/arch-exynos/clk.h>
#include <asm/arch/pinmux.h>

/*
 * Set mshci controller instances clock drivder
 *
 * @param enum periph_id instance of the mshci controller
 *
 * Return	0 if ok else -1
 */
int clock_set_mshci(enum periph_id peripheral);

/*
 * Sets the epll clockrate
 *
 * @param rate	Required clock rate to the presacaler in Hz
 *
 * Return	0 if ok else -1
 */
int clock_epll_set_rate(unsigned long rate);

/*
 * selects the clk source for I2S MCLK
 */
void clock_select_i2s_clk_source(void);

/*
 * Set prescaler division based on input and output frequency
 * for i2s audio clock
 *
 * @param src_frq	Source frequency in Hz
 * @param dst_frq	Required MCLK frequency in Hz
 *
 * Return	0 if ok else -1
 */
int clock_set_i2s_clk_prescaler(unsigned int src_frq, unsigned int dst_frq);

#endif  /* __ASM_ARM_ARCH_EXYNOS5_CLK_H__ */
