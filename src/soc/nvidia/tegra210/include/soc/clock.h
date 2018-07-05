/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef __SOC_NVIDIA_TEGRA210_CLOCK_H__
#define __SOC_NVIDIA_TEGRA210_CLOCK_H__

#include <arch/hlt.h>
#include <arch/io.h>
#include <console/console.h>
#include <soc/clk_rst.h>
#include <stdint.h>
#include <stdlib.h>

enum {
	CLK_L_CPU = 0x1 << 0,
	CLK_L_COP = 0x1 << 1,
	CLK_L_TRIG_SYS = 0x1 << 2,
	CLK_L_RTC = 0x1 << 4,
	CLK_L_TMR = 0x1 << 5,
	CLK_L_UARTA = 0x1 << 6,
	CLK_L_UARTB = 0x1 << 7,
	CLK_L_GPIO = 0x1 << 8,
	CLK_L_SDMMC2 = 0x1 << 9,
	CLK_L_SPDIF = 0x1 << 10,
	CLK_L_I2S2 = 0x1 << 11,
	CLK_L_I2C1 = 0x1 << 12,
	CLK_L_NDFLASH = 0x1 << 13,
	CLK_L_SDMMC1 = 0x1 << 14,
	CLK_L_SDMMC4 = 0x1 << 15,
	CLK_L_PWM = 0x1 << 17,
	CLK_L_I2S3 = 0x1 << 18,
	CLK_L_EPP = 0x1 << 19,
	CLK_L_VI = 0x1 << 20,
	CLK_L_2D = 0x1 << 21,
	CLK_L_USBD = 0x1 << 22,
	CLK_L_ISP = 0x1 << 23,
	CLK_L_3D = 0x1 << 24,
	CLK_L_DISP2 = 0x1 << 26,
	CLK_L_DISP1 = 0x1 << 27,
	CLK_L_HOST1X = 0x1 << 28,
	CLK_L_VCP = 0x1 << 29,
	CLK_L_I2S1 = 0x1 << 30,
	CLK_L_CACHE2 = 0x1 << 31,

	CLK_H_MEM = 0x1 << 0,
	CLK_H_AHBDMA = 0x1 << 1,
	CLK_H_APBDMA = 0x1 << 2,
	CLK_H_KBC = 0x1 << 4,
	CLK_H_STAT_MON = 0x1 << 5,
	CLK_H_PMC = 0x1 << 6,
	CLK_H_FUSE = 0x1 << 7,
	CLK_H_KFUSE = 0x1 << 8,
	CLK_H_SBC1 = 0x1 << 9,
	CLK_H_SNOR = 0x1 << 10,
	CLK_H_JTAG2TBC = 0x1 << 11,
	CLK_H_SBC2 = 0x1 << 12,
	CLK_H_SBC3 = 0x1 << 14,
	CLK_H_I2C5 = 0x1 << 15,
	CLK_H_DSI = 0x1 << 16,
	CLK_H_HSI = 0x1 << 18,
	CLK_H_HDMI = 0x1 << 19,
	CLK_H_CSI = 0x1 << 20,
	CLK_H_I2C2 = 0x1 << 22,
	CLK_H_UARTC = 0x1 << 23,
	CLK_H_MIPI_CAL = 0x1 << 24,
	CLK_H_EMC = 0x1 << 25,
	CLK_H_USB2 = 0x1 << 26,
	CLK_H_USB3 = 0x1 << 27,
	CLK_H_MPE = 0x1 << 28,
	CLK_H_VDE = 0x1 << 29,
	CLK_H_BSEA = 0x1 << 30,
	CLK_H_BSEV = 0x1 << 31,

	CLK_U_UARTD = 0x1 << 1,
	CLK_U_UARTE = 0x1 << 2,
	CLK_U_I2C3 = 0x1 << 3,
	CLK_U_SBC4 = 0x1 << 4,
	CLK_U_SDMMC3 = 0x1 << 5,
	CLK_U_PCIE = 0x1 << 6,
	CLK_U_OWR = 0x1 << 7,
	CLK_U_AFI = 0x1 << 8,
	CLK_U_CSITE = 0x1 << 9,
	CLK_U_PCIEXCLK = 0x1 << 10,
	CLK_U_AVPUCQ = 0x1 << 11,
	CLK_U_TRACECLKIN = 0x1 << 13,
	CLK_U_SOC_THERM = 0x1 << 14,
	CLK_U_DTV = 0x1 << 15,
	CLK_U_NAND_SPEED = 0x1 << 16,
	CLK_U_I2C_SLOW = 0x1 << 17,
	CLK_U_DSIB = 0x1 << 18,
	CLK_U_TSEC = 0x1 << 19,
	CLK_U_IRAMA = 0x1 << 20,
	CLK_U_IRAMB = 0x1 << 21,
	CLK_U_IRAMC = 0x1 << 22,

	// Clock reset.
	CLK_U_EMUCIF = 0x1 << 23,
	// Clock enable.
	CLK_U_IRAMD = 0x1 << 23,

	CLK_U_CRAM2 = 0x2 << 24,
	CLK_U_XUSB_HOST = 0x1 << 25,
	CLK_U_MSENC = 0x1 << 27,
	CLK_U_SUS_OUT = 0x1 << 28,
	CLK_U_DEV2_OUT = 0x1 << 29,
	CLK_U_DEV1_OUT = 0x1 << 30,
	CLK_U_XUSB_DEV = 0x1 << 31,

	CLK_V_CPUG = 0x1 << 0,
	CLK_V_CPULP = 0x1 << 1,
	CLK_V_3D2 = 0x1 << 2,
	CLK_V_MSELECT = 0x1 << 3,
	CLK_V_I2S4 = 0x1 << 5,
	CLK_V_I2S5 = 0x1 << 6,
	CLK_V_I2C4 = 0x1 << 7,
	CLK_V_SBC5 = 0x1 << 8,
	CLK_V_SBC6 = 0x1 << 9,
	CLK_V_AHUB = 0x1 << 10,
	CLK_V_APB2APE = 0x1 << 11,
	CLK_V_HDA2CODEC_2X = 0x1 << 15,
	CLK_V_ATOMICS = 0x1 << 16,
	CLK_V_ACTMON = 0x1 << 23,
	CLK_V_EXTPERIPH1 = 0x1 << 24,
	CLK_V_SATA = 0x1 << 28,
	CLK_V_HDA = 0x1 << 29,

	CLK_W_HDA2HDMICODEC = 0x1 << 0,
	CLK_W_SATACOLD = 0x1 << 1,
	CLK_W_CEC = 0x1 << 8,
	CLK_W_XUSB_PADCTL = 0x1 << 14,
	CLK_W_ENTROPY = 0x1 << 21,
	CLK_W_DVFS = 0x1 << 27,
	CLK_W_XUSB_SS = 0x1 << 28,

	CLK_X_GPU = 0x1 << 24,
	CLK_X_SOR1 = 0x1 << 23,
	CLK_X_SOR0 = 0x1 << 22,
	CLK_X_DPAUX = 0x1 << 21,
	CLK_X_VIC = 0x1 << 18,
	CLK_X_UART_FST_MIPI_CAL = 0x1 << 17,
	CLK_X_MIPIBIF = 0x1 << 13,
	CLK_X_I2C6 = 0x1 << 6,
	CLK_X_ETR = 0x1 << 3,
	CLK_X_SPARE = 0x1 << 0,

	CLK_Y_APE = 0x1 << 6,
	CLK_Y_DPAUX1 = 0x1 << 15,
	CLK_Y_QSPI = 0x1 << 19,
	CLK_Y_SOR_SAFE = 0x1 << 30,
};

enum {
	PLLP = 0,
	PLLC2 = 1,
	PLLC = 2,
	PLLC_OUT1 = 3,
	PLLM = 4,
	CLK_M = 5,
	CLK_S = 6,
	PLLE = 7,
	PLLA = 8,
	PLLD = 9,
	PLLD2 = 10,
	PLLC4_OUT0 = 11,
	PLLC4_OUT1 = 12,
	PLLC4_OUT2 = 13,
	PLLC4_OUT3 = 14,
	PLLC4_OUT0_L = 15,
	PLLC4_OUT1_L = 16,
	PLLC4_OUT2_L = 17,
	PLLD_OUT0 = 18,
	PLLP_OUT3 = 19,
	PLLC2_OUT0 = 20,
	UNUSED0 = 100,
	UNUSED1 = 101,
	UNUSED2 = 102,
	UNUSED3 = 103,
	UNUSED4 = 104,
	UNUSED5 = 105,
	UNUSED6 = 106,
	UNUSED7 = 107,
};

#define CLK_SRC_DEV_ID(dev, src)	CLK_SRC_##dev##_##src
#define CLK_SRC_FREQ_ID(dev, src)	CLK_SRC_FREQ_##dev##_##src

#define CLK_SRC_DEVICE(dev, a, b, c, d, e, f, g, h)	\
	CLK_SRC_DEV_ID(dev, a) = 0,		 \
	CLK_SRC_DEV_ID(dev, b) = 1,		 \
	CLK_SRC_DEV_ID(dev, c) = 2,		 \
	CLK_SRC_DEV_ID(dev, d) = 3,		 \
	CLK_SRC_DEV_ID(dev, e) = 4,		 \
	CLK_SRC_DEV_ID(dev, f) = 5,		 \
	CLK_SRC_DEV_ID(dev, g) = 6,	         \
	CLK_SRC_DEV_ID(dev, h) = 7,	         \
	CLK_SRC_FREQ_ID(dev, a) = a,		 \
	CLK_SRC_FREQ_ID(dev, b) = b,		 \
	CLK_SRC_FREQ_ID(dev, c) = c,		 \
	CLK_SRC_FREQ_ID(dev, d) = d,		 \
	CLK_SRC_FREQ_ID(dev, e) = e,		 \
	CLK_SRC_FREQ_ID(dev, f) = f,		 \
	CLK_SRC_FREQ_ID(dev, g) = g,		 \
	CLK_SRC_FREQ_ID(dev, h) = h

enum {
	CLK_SRC_DEVICE(disp1, PLLP, PLLD, PLLD_OUT0, UNUSED3, UNUSED4, PLLD2,
		       CLK_M, UNUSED7),
	CLK_SRC_DEVICE(host1x, PLLC4_OUT1, PLLC2, PLLC, PLLC4_OUT2, PLLP, CLK_M,
		       PLLA, PLLC4_OUT0),
	CLK_SRC_DEVICE(I2C1, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(I2C2, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(I2C3, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(I2C5, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(I2C6, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(I2S1, PLLA, UNUSED1, CLK_S, UNUSED3, PLLP, UNUSED5,
		       CLK_M, UNUSED7),
	CLK_SRC_DEVICE(mselect, PLLP, PLLC2, PLLC, PLLC4_OUT2, PLLC4_OUT1,
		       CLK_S, CLK_M, PLLC4_OUT0),
	CLK_SRC_DEVICE(SPI1, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(SPI4, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4, PLLC4_OUT1,
		       CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(SDMMC1, PLLP, PLLA, PLLC, PLLC4_OUT2, PLLM, PLLE, CLK_M,
		       PLLC4_OUT0),
	CLK_SRC_DEVICE(SDMMC4, PLLP, PLLC4_OUT2_L, PLLC4_OUT0_L, PLLC4_OUT2,
		       PLLC4_OUT1, PLLC4_OUT1_L, CLK_M, PLLC4_OUT0),
	CLK_SRC_DEVICE(UARTA, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4,
		       PLLC4_OUT1, CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(UARTB, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4,
		       PLLC4_OUT1, CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(UARTC, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4,
		       PLLC4_OUT1, CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(UARTD, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4,
		       PLLC4_OUT1, CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(UARTE, PLLP, PLLC2, PLLC, PLLC4_OUT0, UNUSED4,
		       PLLC4_OUT1, CLK_M, PLLC4_OUT2),
	CLK_SRC_DEVICE(i2s1, PLLA, UNUSED1, CLK_S, UNUSED3, PLLP, UNUSED5,
		       CLK_M, UNUSED7),
	CLK_SRC_DEVICE(extperiph1, PLLA, CLK_S, PLLP, CLK_M, PLLE, UNUSED5,
		       UNUSED6, UNUSED7),
	CLK_SRC_DEVICE(QSPI, PLLP, PLLC_OUT1, PLLC, UNUSED3, PLLC4_OUT2,
		       PLLC4_OUT1, CLK_M, PLLC4_OUT0),
	CLK_SRC_DEVICE(uart_fst_mipi_cal, PLLP_OUT3, PLLP, PLLC, UNUSED3, PLLC2_OUT0,
			UNUSED5, CLK_M, UNUSED7),
};

/* PLL stabilization delay in usec */
#define CLOCK_PLL_STABLE_DELAY_US 300

#define IO_STABILIZATION_DELAY (2)
#define LOGIC_STABILIZATION_DELAY (2)

/* Calculate clock fractional divider value from ref and target frequencies.
 * This is for a U7.1 format. This is not well written up in the book and
 * there have been some questions about this macro, so here we go.
 * U7.1 format is defined as (ddddddd+1) + (h*.5)
 * The lowest order bit is actually a fractional bit.
 * Hence, the divider can be thought of as 9 bits.
 * So:
 * divider = ((ref/freq) << 1 - 1) (upper 7 bits) |
 *	(ref/freq & 1) (low order half-bit)
 * however we can't do fractional arithmetic ... these are integers!
 * So we normalize by shifting the result left 1 bit, and extracting
 * ddddddd and h directly to the returned u8.
 * divider = 2*(ref/freq);
 * We want to
 * preserve 7 bits of divisor and one bit of fraction, in 8 bits, as well as
 * subtract one from ddddddd. Since we computed ref*2, the dddddd is now nicely
 * situated in the upper 7 bits, and the h is sitting there in the low order
 * bit. To subtract 1 from ddddddd, just subtract 2 from the 8-bit number
 * and voila, upper 7 bits are (ref/freq-1), and lowest bit is h. Since you
 * will assign this to a u8, it gets nicely truncated for you.
 */
#define CLK_DIVIDER(REF, FREQ)	(div_round_up(((REF) * 2), (FREQ)) - 2)

/* Calculate clock frequency value from reference and clock divider value
 * The discussion in the book is pretty lacking.
 * The idea is that we need to divide a ref clock by a divisor
 * in U7.1 format, where 7 upper bits are the integer
 * and lowest order bit is a fraction.
 * from the book, U7.1 is (ddddddd+1) + (h*.5)
 * To normalize to an actual number, we might do this:
 * ((d>>7+1)&0x7f) + (d&1 >> 1)
 * but as you might guess, the low order bit would be lost.
 * Since we can't express the fractional bit, we need to multiply it all by 2.
 * ((d + 2)&0xfe) + (d & 1)
 * Since we're just adding +2, the lowest order bit is preserved. Hence
 * (d+2) is the same as ((d + 2)&0xfe) + (d & 1)
 *
 * Since you multiply denominator * 2 (by NOT shifting it),
 * you multiply numerator * 2 to cancel it out.
 */
#define CLK_FREQUENCY(REF, REG)	(((REF) * 2) / ((REG) + 2))

static inline void _clock_set_div(u32 *reg, const char *name, u32 div,
				  u32 div_mask, u32 src)
{
	// The I2C and UART divisors are 16 bit while all the others are 8 bit.
	// The I2C clocks are handled by the specialized macro below, but the
	// UART clocks aren't. Don't use this function on UART clocks.
	if (div & ~div_mask) {
		printk(BIOS_ERR, "%s clock divisor overflow!", name);
		hlt();
	}
	clrsetbits_le32(reg, CLK_SOURCE_MASK | CLK_DIVISOR_MASK,
			src << CLK_SOURCE_SHIFT | div);
}

#define get_i2c_clk_div(src,freq) (div_round_up(src, (freq) * (0x19 + 1) * 8) - 1)
#define get_clk_div(src,freq)     CLK_DIVIDER(src,freq)
#define CLK_DIV_MASK              0xff
#define CLK_DIV_MASK_I2C          0xffff

#define clock_configure_source(device, src, freq)			\
	_clock_set_div(CLK_RST_REG(clk_src_##device), #device,		\
		       get_clk_div(TEGRA_##src##_KHZ, freq), CLK_DIV_MASK, \
		       CLK_SRC_DEV_ID(device, src))

/* soc-specific */
#define TEGRA_CLK_M_KHZ	 (clock_get_osc_khz()/2)
#define TEGRA_PLLX_KHZ   CONFIG_PLLX_KHZ
#define TEGRA_PLLP_KHZ   (408000)
#define TEGRA_PLLP_OUT3_KHZ	(68000)
#define TEGRA_PLLC_KHZ   (600000)
#define TEGRA_PLLD_KHZ   (925000)
#define TEGRA_PLLD_OUT0_KHZ   (TEGRA_PLLD_KHZ/2)
#define TEGRA_PLLU_KHZ   (960000)

#define clock_enable(l, h, u, v, w, x, y)				\
	do {								\
		u32 bits[DEV_CONFIG_BLOCKS] = {l, h, u, v, w, x, y};	\
		clock_enable_regs(bits);				\
	} while (0)

#define clock_disable(l, h, u, v, w, x, y)				\
	do {								\
		u32 bits[DEV_CONFIG_BLOCKS] = {l, h, u, v, w, x, y};	\
		clock_disable_regs(bits);				\
	} while (0)

#define clock_set_reset(l, h, u, v, w, x, y)				\
	do {								\
		u32 bits[DEV_CONFIG_BLOCKS] = {l, h, u, v, w, x, y};	\
		clock_set_reset_regs(bits);				\
	} while (0)

#define clock_clr_reset(l, h, u, v, w, x, y)				\
	do {								\
		u32 bits[DEV_CONFIG_BLOCKS] = {l, h, u, v, w, x, y};	\
		clock_clr_reset_regs(bits);				\
	} while (0)

#define clock_enable_l(l)               clock_enable(l, 0, 0, 0, 0, 0, 0)
#define clock_enable_h(h)               clock_enable(0, h, 0, 0, 0, 0, 0)
#define clock_enable_u(u)               clock_enable(0, 0, u, 0, 0, 0, 0)
#define clock_enable_v(v)               clock_enable(0, 0, 0, v, 0, 0, 0)
#define clock_enable_w(w)               clock_enable(0, 0, 0, 0, w, 0, 0)
#define clock_enable_x(x)               clock_enable(0, 0, 0, 0, 0, x, 0)
#define clock_enable_y(y)               clock_enable(0, 0, 0, 0, 0, 0, y)

#define clock_disable_l(l)              clock_disable(l, 0, 0, 0, 0, 0, 0)
#define clock_disable_h(h)              clock_disable(0, h, 0, 0, 0, 0, 0)
#define clock_disable_u(u)              clock_disable(0, 0, u, 0, 0, 0, 0)
#define clock_disable_v(v)              clock_disable(0, 0, 0, v, 0, 0, 0)
#define clock_disable_w(w)              clock_disable(0, 0, 0, 0, w, 0, 0)
#define clock_disable_x(x)              clock_disable(0, 0, 0, 0, 0, x, 0)
#define clock_disable_y(y)              clock_disable(0, 0, 0, 0, 0, 0, y)

#define clock_set_reset_l(l)            clock_set_reset(l, 0, 0, 0, 0, 0, 0)
#define clock_set_reset_h(h)            clock_set_reset(0, h, 0, 0, 0, 0, 0)
#define clock_set_reset_u(u)            clock_set_reset(0, 0, u, 0, 0, 0, 0)
#define clock_set_reset_v(v)            clock_set_reset(0, 0, 0, v, 0, 0, 0)
#define clock_set_reset_w(w)            clock_set_reset(0, 0, 0, 0, w, 0, 0)
#define clock_set_reset_x(x)            clock_set_reset(0, 0, 0, 0, 0, x, 0)
#define clock_set_reset_y(x)            clock_set_reset(0, 0, 0, 0, 0, y, 0)

#define clock_clr_reset_l(l)            clock_clr_reset(l, 0, 0, 0, 0, 0, 0)
#define clock_clr_reset_h(h)            clock_clr_reset(0, h, 0, 0, 0, 0, 0)
#define clock_clr_reset_u(u)            clock_clr_reset(0, 0, u, 0, 0, 0, 0)
#define clock_clr_reset_v(v)            clock_clr_reset(0, 0, 0, v, 0, 0, 0)
#define clock_clr_reset_w(w)            clock_clr_reset(0, 0, 0, 0, w, 0, 0)
#define clock_clr_reset_x(x)            clock_clr_reset(0, 0, 0, 0, 0, x, 0)
#define clock_clr_reset_y(y)            clock_clr_reset(0, 0, 0, 0, 0, 0, y)

#define clock_enable_clear_reset_l(l)			\
	clock_enable_clear_reset(l, 0, 0, 0, 0, 0, 0)
#define clock_enable_clear_reset_h(h)			\
	clock_enable_clear_reset(0, h, 0, 0, 0, 0, 0)
#define clock_enable_clear_reset_u(u)			\
	clock_enable_clear_reset(0, 0, u, 0, 0, 0, 0)
#define clock_enable_clear_reset_v(v)			\
	clock_enable_clear_reset(0, 0, 0, v, 0, 0, 0)
#define clock_enable_clear_reset_w(w)			\
	clock_enable_clear_reset(0, 0, 0, 0, w, 0, 0)
#define clock_enable_clear_reset_x(x)			\
	clock_enable_clear_reset(0, 0, 0, 0, 0, x, 0)
#define clock_enable_clear_reset_y(y)			\
	clock_enable_clear_reset(0, 0, 0, 0, 0, 0, y)

int clock_get_osc_khz(void);
int clock_get_pll_input_khz(void);
/*
 * Configure PLLD to requested frequency. Returned value is closest match
 * within the PLLD's constraints or 0 if an error.
 */
u32 clock_configure_plld(u32 frequency);
void clock_early_uart(void);
void clock_external_output(int clk_id);
void clock_sdram(u32 m, u32 n, u32 p, u32 setup, u32 kvco, u32 kcp,
		 u32 stable_time, u32 emc_source, u32 same_freq);
void clock_cpu0_config(void);
void clock_halt_avp(void);
void clock_enable_regs(u32 bits[DEV_CONFIG_BLOCKS]);
void clock_disable_regs(u32 bits[DEV_CONFIG_BLOCKS]);
void clock_set_reset_regs(u32 bits[DEV_CONFIG_BLOCKS]);
void clock_clr_reset_regs(u32 bits[DEV_CONFIG_BLOCKS]);
void clock_enable_clear_reset(u32 l, u32 h, u32 u, u32 v, u32 w, u32 x, u32 y);
void clock_grp_enable_clear_reset(u32 val, u32 *clk_enb_set_reg,
				  u32 *rst_dev_clr_reg);
void clock_reset_l(u32 l);
void clock_reset_h(u32 h);
void clock_reset_u(u32 u);
void clock_reset_v(u32 v);
void clock_reset_w(u32 w);
void clock_reset_x(u32 x);
void clock_reset_y(u32 y);
void clock_init(void);
void clock_init_arm_generic_timer(void);
void sor_clock_stop(void);
void sor_clock_start(void);
void clock_enable_audio(void);

#endif /* __SOC_NVIDIA_TEGRA210_CLOCK_H__ */
