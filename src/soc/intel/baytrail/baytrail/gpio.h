/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BAYTRAIL_GPIO_H_
#define _BAYTRAIL_GPIO_H_

#include <stdint.h>
#include <arch/io.h>
#include <baytrail/iomap.h>

/* #define GPIO_DEBUG */

/* Pad base, ex. PAD_CONF0[n]= PAD_BASE+16*n */
#define GPSCORE_PAD_BASE		IO_BASE_ADDRESS + 0x0000
#define GPNCORE_PAD_BASE		IO_BASE_ADDRESS + 0x1000
#define GPSSUS_PAD_BASE			IO_BASE_ADDRESS + 0x2000

/* Pad register offset */
#define PAD_CONF0_REG			0x0
#define PAD_CONF1_REG			0x4
#define PAD_VAL_REG			0x8

/* Legacy IO register base */
#define GPSCORE_LEGACY_BASE		GPIO_BASE_ADDRESS + 0x00
#define GPSSUS_LEGACY_BASE		GPIO_BASE_ADDRESS + 0x80
/* Some banks have no legacy GPIO interface */
#define GP_LEGACY_BASE_NONE		0xFFFF

#define LEGACY_USE_SEL_REG		0x00
#define LEGACY_IO_SEL_REG		0x04
#define LEGACY_GP_LVL_REG		0x08
#define LEGACY_TPE_REG			0x0C
#define LEGACY_TNE_REG			0x10
#define LEGACY_TS_REG			0x14
#define LEGACY_WAKE_EN_REG		0x18

/* Number of GPIOs in each bank */
#define GPNCORE_COUNT		27
#define GPSCORE_COUNT		102
#define GPSSUS_COUNT		44

/* GPIO legacy IO register settings */
#define GPIO_USE_PAD 		0
#define GPIO_USE_LEGACY 	1

#define GPIO_DIR_OUTPUT		0
#define GPIO_DIR_INPUT		1

#define GPIO_LEVEL_LOW		0
#define GPIO_LEVEL_HIGH		1

#define GPIO_PEDGE_DISABLE	0
#define GPIO_PEDGE_ENABLE	1

#define GPIO_NEDGE_DISABLE	0
#define GPIO_NEDGE_ENABLE	1

/* PAD_CONF0 settings */
#define PAD_IRQ_EN		(1 << 27)
#define PAD_LEVEL_IRQ		(1 << 24)

#define PAD_PU_2K		(0 << 9)
#define PAD_PU_10K		(1 << 9)
#define PAD_PU_20K		(2 << 9)
#define PAD_PU_40K		(3 << 9)

#define PAD_PU_DISABLE		(0 << 7)
#define PAD_PU_UP		(1 << 7)
#define PAD_PU_DOWN		(2 << 7)

#define PAD_FUNC0		0x0
#define PAD_FUNC1		0x1
#define PAD_FUNC2		0x2
#define PAD_FUNC3		0x3
#define PAD_FUNC4		0x4
#define PAD_FUNC5		0x5
#define PAD_FUNC6		0x6

/* PAD_VAL settings */
#define PAD_INPUT_ENABLE	(1 << 2)
#define PAD_OUTPUT_ENABLE	(1 << 1)

/* End marker */
#define GPIO_LIST_END		0xffffffff

#define GPIO_INPUT_PU_10K \
	{ .pad_conf0 = PAD_PU_10K | PAD_PU_UP, \
	  .pad_val   = PAD_INPUT_ENABLE, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT }

#define GPIO_OUT_LOW \
	{ .pad_conf0 = PAD_PU_DISABLE, \
	  .pad_val   = PAD_OUTPUT_ENABLE, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_OUTPUT, \
	  .gp_lvl    = GPIO_LEVEL_LOW }

#define GPIO_OUT_HIGH \
	{ .pad_conf0 = PAD_PU_DISABLE, \
	  .pad_val   = PAD_OUTPUT_ENABLE, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_OUTPUT, \
	  .gp_lvl    = GPIO_LEVEL_HIGH }

#define GPIO_FUNC0 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC0 }

#define GPIO_FUNC1 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC1 }

#define GPIO_FUNC2 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC2 }

#define GPIO_FUNC3 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC3 }

#define GPIO_FUNC4 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC4 }

#define GPIO_FUNC5 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC5 }

#define GPIO_FUNC6 \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC6 }

#define GPIO_END \
	{  .pad_conf0 = GPIO_LIST_END }

/* Common default GPIO settings */
#define GPIO_INPUT GPIO_INPUT_PU_10K
#define GPIO_UNUSED GPIO_INPUT_PU_10K
#define GPIO_DEFAULT GPIO_FUNC0

struct soc_gpio_map {
	u32 pad_conf0;
	u32 pad_conf1;
	u32 pad_val;
	u8  use_sel : 1;
	u8  io_sel  : 1;
	u8  gp_lvl  : 1;
	u8  tpe     : 1;
	u8  tne     : 1;
	u8  wake_en : 1;
} __attribute__ ((packed));

struct soc_gpio_config {
	const struct soc_gpio_map *ncore;
	const struct soc_gpio_map *score;
	const struct soc_gpio_map *ssus;
};

/* Description of GPIO 'bank' ex. {ncore, score. ssus} */
struct gpio_bank {
	const int gpio_count;
	const u8* gpio_to_pad;
	const int legacy_base;
	const unsigned long pad_base;
	const u8 has_wake_en :1;
};

void setup_soc_gpios(struct soc_gpio_config *config);
/* This function is weak and can be overridden by a mainboard function. */
struct soc_gpio_config* mainboard_get_gpios(void);

/* Functions / defines for changing GPIOs in romstage */
/* SCORE Pad definitions. */
#define PCU_SMB_CLK_PAD			88
#define PCU_SMB_DATA_PAD		90

static inline unsigned int score_pconf0(int pad_num)
{
	return GPSCORE_PAD_BASE + pad_num * 16;
}

static inline unsigned int ssus_pconf0(int pad_num)
{
	return GPSSUS_PAD_BASE + pad_num * 16;
}

static inline void score_select_func(int pad, int func)
{
	uint32_t reg;
	uint32_t pconf0_addr = score_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

static inline void ssus_select_func(int pad, int func)
{
	uint32_t reg;
	uint32_t pconf0_addr = ssus_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

#endif /* _BAYTRAIL_GPIO_H_ */
