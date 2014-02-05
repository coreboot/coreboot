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
#define GPSCORE_PAD_BASE		(IO_BASE_ADDRESS + 0x0000)
#define GPNCORE_PAD_BASE		(IO_BASE_ADDRESS + 0x1000)
#define GPSSUS_PAD_BASE			(IO_BASE_ADDRESS + 0x2000)

/* Pad register offset */
#define PAD_CONF0_REG			0x0
#define PAD_CONF1_REG			0x4
#define PAD_VAL_REG			0x8

/* Legacy IO register base */
#define GPSCORE_LEGACY_BASE		(GPIO_BASE_ADDRESS + 0x00)
#define GPSSUS_LEGACY_BASE		(GPIO_BASE_ADDRESS + 0x80)
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

/* config0[29] - Disable second mask */
#define PAD_MASK2_DISABLE	(1 << 29)

/* config0[27] - Direct Irq En */
#define PAD_IRQ_EN		(1 << 27)

/* config0[24] - Gd Level */
#define PAD_LEVEL_IRQ		(1 << 24)
#define PAD_EDGE_IRQ		(0 << 24)

/* config0[17] - Slow clkgate / glitch filter */
#define PAD_SLOWGF_ENABLE	(1 << 17)

/* config0[16] - Fast clkgate / glitch filter */
#define PAD_FASTGF_ENABLE	(1 << 16)

/* config0[15] - Hysteresis enable (inverted) */
#define PAD_HYST_DISABLE	(1 << 15)
#define PAD_HYST_ENABLE		(0 << 15)

/* config0[14:13] - Hysteresis control */
#define PAD_HYST_CTRL_DEFAULT	(2 << 13)

/* config0[11] - Bypass Flop */
#define PAD_FLOP_BYPASS		(1 << 11)
#define PAD_FLOP_ENABLE		(0 << 11)

/* config0[10:9] - Pull str */
#define PAD_PU_2K		(0 << 9)
#define PAD_PU_10K		(1 << 9)
#define PAD_PU_20K		(2 << 9)
#define PAD_PU_40K		(3 << 9)

/* config0[8:7] - Pull assign */
#define PAD_PULL_DISABLE	(0 << 7)
#define PAD_PULL_UP		(1 << 7)
#define PAD_PULL_DOWN		(2 << 7)

/* config0[2:0] - Func. pin mux */
#define PAD_FUNC0		0x0
#define PAD_FUNC1		0x1
#define PAD_FUNC2		0x2
#define PAD_FUNC3		0x3
#define PAD_FUNC4		0x4
#define PAD_FUNC5		0x5
#define PAD_FUNC6		0x6

/* pad config0 power-on values - We will not often want to change these */
#define PAD_CONFIG0_DEFAULT	(PAD_MASK2_DISABLE     | PAD_SLOWGF_ENABLE | \
				 PAD_FASTGF_ENABLE     | PAD_HYST_DISABLE | \
				 PAD_HYST_CTRL_DEFAULT | PAD_FLOP_BYPASS)

/* pad config1 reg power-on values - Shouldn't need to change this */
#define PAD_CONFIG1_DEFAULT	0x8000

/* pad_val[2] - Iinenb - active low */
#define PAD_VAL_INPUT_DISABLE	(1 << 2)
#define PAD_VAL_INPUT_ENABLE	(0 << 2)

/* pad_val[1] - Ioutenb - active low */
#define PAD_VAL_OUTPUT_DISABLE	(1 << 1)
#define PAD_VAL_OUTPUT_ENABLE	(0 << 1)

/* pad_val[0] - Value */
#define PAD_VAL_HIGH		(1 << 0)
#define PAD_VAL_LOW		(0 << 0)

/* pad_val reg power-on default varies by pad, and apparently can cause issues
 * if not set correctly, even if the pin isn't configured as GPIO. */
#define PAD_VAL_DEFAULT		(PAD_VAL_INPUT_ENABLE | PAD_VAL_OUTPUT_DISABLE)

/* Configure GPIOs as legacy by default. GPNCORE doesn't support
 * legacy config -- so also configure the pad regs as GPIO. We rely upon
 * the fact that all GPNCORE pads are function 0 GPIO. */
#define GPIO_INPUT_PU_10K \
	{ .pad_conf0 = PAD_PU_10K | PAD_PULL_UP | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT_ENABLE, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT }

#define GPIO_INPUT_PD_10K \
	{ .pad_conf0 = PAD_PU_10K | PAD_PULL_DOWN | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT_ENABLE, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT }

#define GPIO_INPUT_NOPU \
	{ .pad_conf0 = PAD_PU_10K | PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT_ENABLE, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT }

#define GPIO_OUT_LOW \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_OUTPUT_ENABLE | PAD_VAL_LOW, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_OUTPUT, \
	  .gp_lvl    = GPIO_LEVEL_LOW }

#define GPIO_OUT_HIGH \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_OUTPUT_ENABLE | PAD_VAL_HIGH, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_OUTPUT, \
	  .gp_lvl    = GPIO_LEVEL_HIGH }

/* Define no-pull / PU / PD configs for each functional config option */
#define GPIO_FUNC(_func, _pudir, _str) \
	{ .use_sel   = GPIO_USE_PAD, \
	  .pad_conf0 = PAD_FUNC##_func | PAD_##_pudir | PAD_PU_##_str | \
		       PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_DEFAULT }

/* Default functional configs -- no PU */
#define GPIO_FUNC0		GPIO_FUNC(0, PULL_DISABLE, 10K)
#define GPIO_FUNC1		GPIO_FUNC(1, PULL_DISABLE, 10K)
#define GPIO_FUNC2		GPIO_FUNC(2, PULL_DISABLE, 10K)
#define GPIO_FUNC3		GPIO_FUNC(3, PULL_DISABLE, 10K)
#define GPIO_FUNC4		GPIO_FUNC(4, PULL_DISABLE, 10K)
#define GPIO_FUNC5		GPIO_FUNC(5, PULL_DISABLE, 10K)
#define GPIO_FUNC6		GPIO_FUNC(6, PULL_DISABLE, 10K)

/* End marker */
#define GPIO_LIST_END		0xffffffff

#define GPIO_END \
	{  .pad_conf0 = GPIO_LIST_END }

/* Common default GPIO settings */
#define GPIO_INPUT 	GPIO_INPUT_NOPU
#define GPIO_INPUT_PU	GPIO_INPUT_PU_10K
#define GPIO_INPUT_PD 	GPIO_INPUT_PD_10K
#define GPIO_NC		GPIO_INPUT_PU_10K
#define GPIO_DEFAULT 	GPIO_FUNC0

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
#define UART_RXD_PAD			82
#define UART_TXD_PAD			83
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

/* These functions require that the input pad be configured as an input GPIO */
static inline int score_get_gpio(int pad)
{
	uint32_t val_addr = score_pconf0(pad) + PAD_VAL_REG;

	return read32(val_addr) & PAD_VAL_HIGH;
}

static inline int ssus_get_gpio(int pad)
{
	uint32_t val_addr = ssus_pconf0(pad) + PAD_VAL_REG;

	return read32(val_addr) & PAD_VAL_HIGH;
}

#endif /* _BAYTRAIL_GPIO_H_ */
