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
 */

#ifndef _BAYTRAIL_GPIO_H_
#define _BAYTRAIL_GPIO_H_

#include <stdint.h>
#include <compiler.h>
#include <arch/io.h>
#include <soc/iomap.h>

/* #define GPIO_DEBUG */

#define CROS_GPIO_DEVICE_NAME	"BayTrail"

/* Pad base, ex. PAD_CONF0[n]= PAD_BASE+16*n */
#define GPSCORE_PAD_BASE	(IO_BASE_ADDRESS + IO_BASE_OFFSET_GPSCORE)
#define GPNCORE_PAD_BASE	(IO_BASE_ADDRESS + IO_BASE_OFFSET_GPNCORE)
#define GPSSUS_PAD_BASE		(IO_BASE_ADDRESS + IO_BASE_OFFSET_GPSSUS)

/* DIRQ registers start at pad base + 0x980 */
#define PAD_BASE_DIRQ_OFFSET		0x980

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
#define GPIO_USE_MMIO		0
#define GPIO_USE_LEGACY		1

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

/* config0[26] - gd_tne */
#define PAD_TNE_IRQ		(1 << 26)

/* config0[25] - gd_tpe */
#define PAD_TPE_IRQ		(1 << 25)

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

/* Input / Output state should usually be mutually exclusive */
#define PAD_VAL_INPUT		(PAD_VAL_INPUT_ENABLE | PAD_VAL_OUTPUT_DISABLE)
#define PAD_VAL_OUTPUT		(PAD_VAL_OUTPUT_ENABLE | PAD_VAL_INPUT_DISABLE)

/* pad_val[0] - Value */
#define PAD_VAL_HIGH		(1 << 0)
#define PAD_VAL_LOW		(0 << 0)

/* pad_val reg power-on default varies by pad, and apparently can cause issues
 * if not set correctly, even if the pin isn't configured as GPIO. */
#define PAD_VAL_DEFAULT		PAD_VAL_INPUT

/* Configure GPIOs as MMIO by default */
#define GPIO_INPUT_PU_10K \
	{ .pad_conf0 = PAD_PU_10K | PAD_PULL_UP | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_MMIO, \
	  .is_gpio   = 1 }

#define GPIO_INPUT_PU_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_UP | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_MMIO, \
	  .is_gpio   = 1 }

#define GPIO_INPUT_PD_10K \
	{ .pad_conf0 = PAD_PU_10K | PAD_PULL_DOWN | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_MMIO, \
	  .is_gpio   = 1 }

#define GPIO_INPUT_PD_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_DOWN | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_MMIO, \
	  .is_gpio   = 1 }

#define GPIO_INPUT_NOPU \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_MMIO, \
	  .is_gpio   = 1 }

#define GPIO_INPUT_LEGACY_NOPU \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT, \
	  .is_gpio   = 1 }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TPE_IRQ | PAD_LEVEL_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ_LEVELHIGH_NO_PULL \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TPE_IRQ | PAD_LEVEL_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ_LEVELLOW_PU_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_UP | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TNE_IRQ | PAD_LEVEL_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ_EDGELOW_PU_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_UP | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TNE_IRQ | PAD_EDGE_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ_EDGEHIGH_PD_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_DOWN | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TPE_IRQ | PAD_EDGE_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ_EDGELOW_PD_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_DOWN | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TNE_IRQ | PAD_EDGE_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

/* Direct / dedicated IRQ input - pass signal directly to apic */
#define GPIO_DIRQ_EDGEBOTH_PU_20K \
	{ .pad_conf0 = PAD_PU_20K | PAD_PULL_UP | PAD_CONFIG0_DEFAULT \
		     | PAD_FUNC0 | PAD_IRQ_EN | PAD_TPE_IRQ| PAD_TNE_IRQ | PAD_EDGE_IRQ, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, }

#define GPIO_OUT_LOW \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_OUTPUT | PAD_VAL_LOW, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_OUTPUT, \
	  .gp_lvl    = GPIO_LEVEL_LOW, \
	  .is_gpio   = 1 }

#define GPIO_OUT_HIGH \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_OUTPUT | PAD_VAL_HIGH, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_OUTPUT, \
	  .gp_lvl    = GPIO_LEVEL_HIGH, \
	  .is_gpio   = 1 }

/* Define no-pull / PU / PD configs for each functional config option */
#define GPIO_FUNC(_func, _pudir, _str) \
	{ .use_sel   = GPIO_USE_MMIO, \
	  .pad_conf0 = PAD_FUNC##_func | PAD_##_pudir | PAD_PU_##_str | \
		       PAD_CONFIG0_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_DEFAULT }

/* Default functional configs -- no PU */
#define GPIO_FUNC0		GPIO_FUNC(0, PULL_DISABLE, 20K)
#define GPIO_FUNC1		GPIO_FUNC(1, PULL_DISABLE, 20K)
#define GPIO_FUNC2		GPIO_FUNC(2, PULL_DISABLE, 20K)
#define GPIO_FUNC3		GPIO_FUNC(3, PULL_DISABLE, 20K)
#define GPIO_FUNC4		GPIO_FUNC(4, PULL_DISABLE, 20K)
#define GPIO_FUNC5		GPIO_FUNC(5, PULL_DISABLE, 20K)
#define GPIO_FUNC6		GPIO_FUNC(6, PULL_DISABLE, 20K)

/* ACPI GPIO routing. Assume everything is externally pulled and negative edge
 * triggered. SCI implies WAKE, but WAKE doesn't imply SCI. */
#define GPIO_ACPI_SCI \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT, \
	  .tne       = 1, \
	  .sci       = 1, \
	  .wake_en   = 1, }
#define GPIO_ACPI_WAKE \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT, \
	  .tne       = 1, \
	  .wake_en   = 1, }
#define GPIO_ACPI_SMI \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_CONFIG0_DEFAULT | PAD_FUNC0, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT, \
	  .pad_val   = PAD_VAL_INPUT, \
	  .use_sel   = GPIO_USE_LEGACY, \
	  .io_sel    = GPIO_DIR_INPUT, \
	  .tne       = 1, \
	  .smi       = 1}

/* End marker */
#define GPIO_LIST_END		0xffffffff

#define GPIO_END \
	{  .pad_conf0 = GPIO_LIST_END }

/* Common default GPIO settings */
#define GPIO_INPUT		GPIO_INPUT_NOPU
#define GPIO_INPUT_LEGACY	GPIO_INPUT_LEGACY_NOPU
#define GPIO_INPUT_PU		GPIO_INPUT_PU_20K
#define GPIO_INPUT_PD		GPIO_INPUT_PD_20K
#define GPIO_NC			GPIO_OUT_HIGH
#define GPIO_DEFAULT		GPIO_FUNC0

/* 16 DirectIRQs per supported bank */
#define GPIO_MAX_DIRQS	16

/* Most pins are GPIO function 0. Some banks have a range of pins with GPIO
 * function 1. Indicate first / last GPIOs with function 1. */
#define GPIO_NONE			255
/* All NCORE GPIOs are function 0 */
#define GPNCORE_GPIO_F1_RANGE_START	GPIO_NONE
#define GPNCORE_GPIO_F1_RANGE_END	GPIO_NONE
/* SCORE GPIO [92:93] are function 1 */
#define GPSCORE_GPIO_F1_RANGE_START	92
#define GPSCORE_GPIO_F1_RANGE_END	93
/* SSUS GPIO [11:21] are function 1 */
#define GPSSUS_GPIO_F1_RANGE_START	11
#define GPSSUS_GPIO_F1_RANGE_END	21

struct soc_gpio_map {
	u32 pad_conf0;
	u32 pad_conf1;
	u32 pad_val;
	u32 use_sel : 1;
	u32 io_sel  : 1;
	u32 gp_lvl  : 1;
	u32 tpe     : 1;
	u32 tne     : 1;
	u32 wake_en : 1;
	u32 smi     : 1;
	u32 is_gpio : 1;
	u32 sci     : 1;
} __packed;

struct soc_gpio_config {
	const struct soc_gpio_map *ncore;
	const struct soc_gpio_map *score;
	const struct soc_gpio_map *ssus;
	const u8 (*core_dirq)[GPIO_MAX_DIRQS];
	const u8 (*sus_dirq)[GPIO_MAX_DIRQS];
};

/* Description of GPIO 'bank' ex. {ncore, score. ssus} */
struct gpio_bank {
	const int gpio_count;
	const u8 *gpio_to_pad;
	const int legacy_base;
	const unsigned long pad_base;
	const u8 has_wake_en :1;
	const u8 gpio_f1_range_start;
	const u8 gpio_f1_range_end;
};

void setup_soc_gpios(struct soc_gpio_config *config, u8 enable_xdp_tap);
/* This function is weak and can be overridden by a mainboard function. */
struct soc_gpio_config* mainboard_get_gpios(void);

/* Functions / defines for changing GPIOs in romstage */
/* SCORE Pad definitions. */
#define UART_RXD_PAD			82
#define UART_TXD_PAD			83
#define PCU_SMB_CLK_PAD			88
#define PCU_SMB_DATA_PAD		90
#define SOC_DDI1_VDDEN_PAD		16

static inline u32 *ncore_pconf0(int pad_num)
{
	return (u32 *)(GPNCORE_PAD_BASE + pad_num * 16);
}

static inline void ncore_select_func(int pad, int func)
{
	uint32_t reg;
	u32 *pconf0_addr = ncore_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

static inline u32 *score_pconf0(int pad_num)
{
	return (u32 *)(GPSCORE_PAD_BASE + pad_num * 16);
}

static inline u32 *ssus_pconf0(int pad_num)
{
	return (u32 *)(GPSSUS_PAD_BASE + pad_num * 16);
}

static inline void score_select_func(int pad, int func)
{
	uint32_t reg;
	uint32_t *pconf0_addr = score_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

static inline void ssus_select_func(int pad, int func)
{
	uint32_t reg;
	uint32_t *pconf0_addr = ssus_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

/* These functions require that the input pad be configured as an input GPIO */
static inline int score_get_gpio(int pad)
{
	uint32_t *val_addr = score_pconf0(pad) + (PAD_VAL_REG/sizeof(uint32_t));

	return read32(val_addr) & PAD_VAL_HIGH;
}

static inline int ssus_get_gpio(int pad)
{
	uint32_t *val_addr = ssus_pconf0(pad) + (PAD_VAL_REG/sizeof(uint32_t));

	return read32(val_addr) & PAD_VAL_HIGH;
}

static inline void ssus_disable_internal_pull(int pad)
{
	const uint32_t pull_mask = ~(0xf << 7);
	write32(ssus_pconf0(pad), read32(ssus_pconf0(pad)) & pull_mask);
}

static inline void ssus_enable_internal_pull(int pad, int mask)
{
	const int pull_mask = ~(0xf << 7);
	write32(ssus_pconf0(pad),
		(read32(ssus_pconf0(pad)) & pull_mask) | mask);
}

#endif /* _BAYTRAIL_GPIO_H_ */
