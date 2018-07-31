/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#ifndef _SOC_GPIO_H_
#define _SOC_GPIO_H_

#include <stdint.h>
#include <compiler.h>
#include <arch/io.h>
#include <soc/gpio_defs.h>
#include <soc/iomap.h>

#define CROS_GPIO_DEVICE_NAME "Braswell"

#define COMMUNITY_SIZE	0x20000

#define COMMUNITY_GPSOUTHWEST_BASE	\
(IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPSOUTHWEST)

#define COMMUNITY_GPNORTH_BASE		\
(IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPNORTH)

#define COMMUNITY_GPEAST_BASE		\
(IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPEAST)

#define COMMUNITY_GPSOUTHEAST_BASE	\
(IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPSOUTHEAST)

#define GPIO_COMMUNITY_COUNT		4
#define GPIO_FAMILIES_MAX_PER_COMM	7
#define GP_SOUTHWEST	0
#define GP_NORTH	1
#define GP_EAST		2
#define GP_SOUTHEAST	3

#define COMMUNITY_BASE(community)		\
(IO_BASE_ADDRESS + community * 0x8000)

#define GP_READ_ACCESS_POLICY_BASE(community)   \
(COMMUNITY_BASE(community) + 0x000)

#define GP_WRITE_ACCESS_POLICY_BASE(community)	\
(COMMUNITY_BASE(community) + 0x100)

#define GP_WAKE_STATUS_REG_BASE(community)	\
(COMMUNITY_BASE(community) + 0x200)

#define GP_WAKE_MASK_REG_BASE(community)	\
(COMMUNITY_BASE(community) + 0x280)

#define GP_INT_STATUS_REG_BASE(community)	\
(COMMUNITY_BASE(community) + 0x300)

#define GP_INT_MASK_REG_BASE(community)		\
(COMMUNITY_BASE(community) + 0x380)

#define GP_FAMILY_RCOMP_CTRL(community, family)	\
(COMMUNITY_BASE(community) + 0x1080 + 0x80 * family)

#define GP_FAMILY_RCOMP_OFFSET(community, family)	\
(COMMUNITY_BASE(community) + 0x1084 + 0x80 * family)

#define GP_FAMILY_RCOMP_OVERRIDE(community, family)	\
(COMMUNITY_BASE(community) + 0x1088 + 0x80 * family)

#define GP_FAMILY_RCOMP_VALUE(community, family)	\
(COMMUNITY_BASE(community) + 0x108C + 0x80 * family)

#define GP_FAMILY_CONF_COMP(community, family)		\
(COMMUNITY_BASE(community) + 0x1090 + 0x80 * family)

#define GP_FAMILY_CONF_REG(community, family)		\
(COMMUNITY_BASE(community) + 0x1094 + 0x80 * family)


/* Value written into pad control reg 0 */
#define PAD_CONTROL_REG0_TRISTATE      (PAD_CONFIG0_DEFAULT|PAD_GPIOFG_HI_Z)

/* Calculate the MMIO Address for specific GPIO pin
 * control register pointed by index.
 */
#define FAMILY_NUMBER(gpio_pad)		(gpio_pad / MAX_FAMILY_PAD_GPIO_NO)
#define INTERNAL_PAD_NUM(gpio_pad)	(gpio_pad % MAX_FAMILY_PAD_GPIO_NO)
#define GPIO_OFFSET(gpio_pad)		(FAMILY_PAD_REGS_OFF \
			+ (FAMILY_PAD_REGS_SIZE * FAMILY_NUMBER(gpio_pad) \
			+ (GPIO_REGS_SIZE * INTERNAL_PAD_NUM(gpio_pad))))

/* Gpio to Pad mapping */
#define SDMMC1_CMD_MMIO_OFFSET		GPIO_OFFSET(23)
#define SDMMC1_D0_MMIO_OFFSET		GPIO_OFFSET(17)
#define SDMMC1_D1_MMIO_OFFSET		GPIO_OFFSET(24)
#define SDMMC1_D2_MMIO_OFFSET		GPIO_OFFSET(20)
#define SDMMC1_D3_MMIO_OFFSET		GPIO_OFFSET(26)
#define MMC1_D4_SD_WE_MMIO_OFFSET	GPIO_OFFSET(67)
#define MMC1_D5_MMIO_OFFSET		GPIO_OFFSET(65)
#define MMC1_D6_MMIO_OFFSET		GPIO_OFFSET(63)
#define MMC1_D7_MMIO_OFFSET		GPIO_OFFSET(68)
#define MMC1_RCLK_OFFSET		GPIO_OFFSET(69)
#define HV_DDI2_DDC_SDA_MMIO_OFFSET	GPIO_OFFSET(62)
#define HV_DDI2_DDC_SCL_MMIO_OFFSET	GPIO_OFFSET(67)
#define CFIO_139_MMIO_OFFSET		GPIO_OFFSET(64)
#define CFIO_140_MMIO_OFFSET		GPIO_OFFSET(67)

/* GPIO Security registers offset */
#define GPIO_READ_ACCESS_POLICY_REG	0x0000
#define GPIO_WRITE_ACCESS_POLICY_REG	0x0100
#define GPIO_WAKE_STATUS_REG		0x0200
#define GPIO_WAKE_MASK_REG0		0x0280
#define GPIO_WAKE_MASK_REG1		0x0284
#define GPIO_INTERRUPT_STATUS		0x0300
#define GPIO_INTERRUPT_MASK		0x0380
#define GPE0A_STS_REG			0x20
#define GPE0A_EN_REG			0x28
#define ALT_GPIO_SMI_REG		0x38
#define GPIO_ROUT_REG			0x58

/* Pad register offset */
#define PAD_CONF0_REG			0x0
#define PAD_CONF1_REG			0x4
#define PAD_VAL_REG			0x8

/* Some banks have no legacy GPIO interface */
#define GP_LEGACY_BASE_NONE		0xFFFF

/* Number of GPIOs in each bank */
#define GPNCORE_COUNT			27
#define GPSCORE_COUNT			102
#define GPSSUS_COUNT			44

#define GP_SOUTHWEST_COUNT		56
#define GP_NORTH_COUNT			59
#define GP_EAST_COUNT			24
#define GP_SOUTHEAST_COUNT		55

/* General */
#define GPIO_REGS_SIZE			8
#define NA				0
#define LOW				0
#define HIGH				1
#define MASK_WAKE			0
#define UNMASK_WAKE			1
#define GPE_CAPABLE			1
#define GPE_CAPABLE_NONE		0

#define MAX_FAMILY_PAD_GPIO_NO		15
#define FAMILY_PAD_REGS_OFF		0x4400
#define FAMILY_PAD_REGS_SIZE		0x400

/* config0[31:28] - Interrupt Selection Interrupt Select */
#define PAD_INT_SEL(int_s)	(int_s << 28)

/* config0[27:26] - Glitch Filter Config */
#define PAD_GFCFG(glitch_cfg)		(glitch_cfg << 26)
#define PAD_GFCFG_DISABLE		(0 << 26)
#define PAD_ENABLE_EDGE_DETECTION	(1 << 26)  /* EDGE DETECTION ONLY */
#define PAD_ENABLE_RX_DETECTION		(2 << 26)  /* RX DETECTION ONLY */
#define PAD_ENABLE_EDGE_RX_DETECTION	(3 << 26)  /* RX & EDGE DETECTION */

/* config0[25:24] - RX/TX Enable Config */
#define PAD_FUNC_CTRL(tx_rx_enable)		(tx_rx_enable << 24)
#define PAD_FUNC_CTRL_RX_TX_ENABLE		(0 << 24)
#define PAD_FUNC_CTRL_TX_ENABLE_RX_DISABLE	(1 << 24)
#define PAD_FUNC_CTRL_TX_ENABLE_RX_ENABLE	(2 << 24)
#define PAD_TX_RX_ENABLE			(3 << 24)

/* config0[23:20] - Termination */
#define PAD_PULL(TERM)		(TERM << 20)
#define PAD_PULL_DISABLE	(0 << 20)
#define PAD_PULL_DOWN_20K	(1 << 20)
#define PAD_PULL_DOWN_5K	(2 << 20)
#define PAD_PULL_DOWN_1K	(4 << 20)
#define PAD_PULL_UP_20K		(9 << 20)
#define PAD_PULL_UP_5K		(10 << 20)
#define PAD_PULL_UP_1K		(12 << 20)

/* config0[19:16] - PAD Mode */
#define PAD_MODE_SELECTION(MODE_SEL)	(MODE_SEL<<16)

#define SET_PAD_MODE_SELECTION(pad_config, mode)	\
	((pad_config & 0xfff0ffff) | PAD_MODE_SELECTION(mode))

/* config0[15] -   GPIO Enable */
#define PAD_GPIO_DISABLE	(0 << 15)
#define PAD_GPIO_ENABLE		(1 << 15)

/* config0[14:11] - Reserver2 */

/* config0[10:8] - GPIO Config */
#define PAD_GPIO_CFG(gpio_cfg)	(gpio_cfg << 8)
#define PAD_GPIOFG_GPIO		(0 << 8)
#define PAD_GPIOFG_GPO		(1 << 8)
#define PAD_GPIOFG_GPI		(2 << 8)
#define PAD_GPIOFG_HI_Z		(3 << 8)

/* config0[7] - Gpio Light Mode Bar */
/* config0[6:2] - Reserved1 */
/* config0[1] - GPIO TX State */
#define PAD_DEFAULT_TX(STATE)	(STATE<<1)
/* config0[0] - GPIO RX State */
#define PAD_RX_BIT	1

/* Pad Control Register 1 configuration */
#define PAD_DISABLE_INT		(0 << 0)
#define PAD_TRIG_EDGE_LOW	(1 << 0)
#define PAD_TRIG_EDGE_HIGH	(2 << 0)
#define PAD_TRIG_EDGE_BOTH	(3 << 0)
#define PAD_TRIG_EDGE_LEVEL	(4 << 0)

/* Pad config0 power-on values */
#define PAD_CONFIG0_DEFAULT	0x00010300
#define PAD_CONFIG0_DEFAULT0	0x00910300
#define PAD_CONFIG0_DEFAULT1	0x00110300
#define PAD_CONFIG0_GPI_DEFAULT	0x00010200

/* Pad config1 reg power-on values */
#define PAD_CONFIG1_DEFAULT0	0x05C00000
#define PAD_CONFIG1_CSEN	0x0DC00000
#define PAD_CONFIG1_DEFAULT1	0x05C00020

#define GPIO_INPUT_NO_PULL \
	{ .pad_conf0 = PAD_PULL_DISABLE | PAD_GPIO_ENABLE \
		     | PAD_CONFIG0_GPI_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define GPIO_INPUT_PU_20K \
	{ .pad_conf0 = PAD_PULL_UP_20K | PAD_GPIO_ENABLE \
		     | PAD_CONFIG0_GPI_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define GPIO_INPUT_PD_5K \
	{ .pad_conf0 = PAD_PULL_DOWN_5K | PAD_GPIO_ENABLE \
		     | PAD_CONFIG0_GPI_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define GPIO_INPUT_PD_20K \
	{ .pad_conf0 = PAD_PULL_DOWN_20K | PAD_GPIO_ENABLE \
		     | PAD_CONFIG0_GPI_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define GPIO_INPUT_PU_5K \
	{ .pad_conf0 = PAD_PULL_UP_5K | PAD_GPIO_ENABLE \
		     | PAD_CONFIG0_GPI_DEFAULT, \
	  .pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define GPI(int_type, int_sel, term, int_msk, glitch_cfg, wake_msk, gpe_val) { \
	.pad_conf0 = PAD_INT_SEL(int_sel) | PAD_GFCFG(glitch_cfg) \
		   | PAD_PULL(term) | PAD_GPIO_ENABLE | PAD_GPIOFG_GPI, \
	.pad_conf1 = int_type << 0 | PAD_CONFIG1_DEFAULT0, \
	.wake_mask = wake_msk, \
	.int_mask  = int_msk, \
	.gpe       = gpe_val }

#define GPO_FUNC(term, tx_state) {\
	.pad_conf0 = PAD_GPIO_ENABLE | PAD_GPIOFG_GPO | PAD_PULL(term) \
		   | tx_state << 1, \
	.pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define NATIVE_FUNC(mode, term, inv_rx_tx) {\
	.pad_conf0 = PAD_GPIO_DISABLE | PAD_GPIOFG_HI_Z \
		   | PAD_MODE_SELECTION(mode) | PAD_PULL(term),\
	.pad_conf1 = PAD_CONFIG1_DEFAULT0 | inv_rx_tx << 4 }

#define NATIVE_FUNC_TX_RX(tx_rx_enable, mode, term, inv_rx_tx) {\
	.pad_conf0 = PAD_FUNC_CTRL(tx_rx_enable) | PAD_GPIO_DISABLE \
		   | PAD_GPIOFG_GPIO | PAD_MODE_SELECTION(mode) \
		   | PAD_PULL(term),\
	.pad_conf1 = PAD_CONFIG1_DEFAULT0 | inv_rx_tx << 4 }

#define NATIVE_FUNC_CSEN(mode, term, inv_rx_tx) {\
	.pad_conf0 = PAD_GPIO_DISABLE | PAD_GPIOFG_HI_Z \
		   | PAD_MODE_SELECTION(mode) | PAD_PULL(term),\
	.pad_conf1 = PAD_CONFIG1_CSEN | inv_rx_tx << 4 }

#define NATIVE_INT(mode, int_sel) {\
	.pad_conf0 = PAD_INT_SEL(int_sel) | PAD_GPIO_DISABLE \
		   | PAD_GPIOFG_HI_Z | PAD_MODE_SELECTION(mode),\
	.pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define NATIVE_INT_PU20K(mode, int_sel) {\
	.pad_conf0 = PAD_PULL_UP_20K | PAD_INT_SEL(int_sel) | PAD_GPIO_DISABLE \
		   | PAD_GPIOFG_HI_Z | PAD_MODE_SELECTION(mode),\
	.pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define SPEAKER \
{	.pad_conf0 = PAD_CONFIG0_DEFAULT0, \
	.pad_conf1 = PAD_CONFIG1_DEFAULT0 }

#define SPARE_PIN\
	{	.pad_conf0 = 0x00110300,\
		.pad_conf1 = PAD_CONFIG1_DEFAULT0 }

/* SCI , SMI, Wake  */
#define GPIO_SCI(int_sel) \
	{   .pad_conf0  = PAD_PULL_DISABLE | PAD_ENABLE_EDGE_RX_DETECTION\
			| PAD_GPIO_ENABLE  | PAD_GPIOFG_GPI \
			| PAD_INT_SEL(int_sel), \
	.pad_conf1  = PAD_TRIG_EDGE_LOW | PAD_CONFIG1_DEFAULT0, \
	.gpe        = SCI, \
	.int_mask   = 1 }

#define GPIO_WAKE(int_sel) \
	{   .pad_conf0  = PAD_PULL_DISABLE | PAD_ENABLE_EDGE_RX_DETECTION\
			| PAD_GPIO_ENABLE  | PAD_GPIOFG_GPI \
			| PAD_INT_SEL(int_sel), \
	.pad_conf1  = PAD_TRIG_EDGE_LOW | PAD_CONFIG1_DEFAULT0, \
	.int_mask   = 1,\
	.wake_mask  = 1 }

#define GPIO_SMI(int_sel) \
	{   .pad_conf0  = PAD_PULL_DISABLE | PAD_ENABLE_EDGE_RX_DETECTION\
			| PAD_GPIO_ENABLE  | PAD_GPIOFG_GPI \
			| PAD_INT_SEL(int_sel), \
	.pad_conf1  = PAD_TRIG_EDGE_LOW | PAD_CONFIG1_DEFAULT0, \
	.int_mask   = 1,\
	.gpe        = SMI }

#define GPIO_SKIP { .skip_config = 1 }

/* Common GPIO settings */
#define NATIVE_DEFAULT(mode)	NATIVE_FUNC(mode, 0, 0) /* no pull */
#define NATIVE_PU20K(mode)	NATIVE_FUNC(mode, 9, 0) /* PH 20k */
#define NATIVE_PU5K(mode)	NATIVE_FUNC(mode, 10, 0) /* PH 5k */
#define NATIVE_PU5K_INVTX(mode)	NATIVE_FUNC(mode, 10, inv_tx_enable) /* PH 5k */
#define NATIVE_PU1K(mode)	NATIVE_FUNC(mode, 12, 0) /* PH 1k */
#define NATIVE_PU1K_CSEN_INVTX(mode) \
		NATIVE_FUNC_CSEN(mode, 12, inv_tx_enable) /* PH 1k */
#define NATIVE_PU1K_INVTX(mode)	NATIVE_FUNC(mode, 12, inv_tx_enable) /* PH 1k */
#define NATIVE_PD20K(mode)	NATIVE_FUNC(mode, 1, 0) /* PD 20k */
#define NATIVE_PD5K(mode)	NATIVE_FUNC(mode, 2, 0) /* PD 5k */
#define NATIVE_PD1K(mode)	NATIVE_FUNC(mode, 4, 0) /* PD 1k */
#define	NATIVE_PD1K_CSEN_INVTX(mode) NATIVE_FUNC_CSEN(mode, 4, inv_tx_enable)
								/* no pull */
#define NATIVE_TX_RX_EN		NATIVE_FUNC_TX_RX(3, 1, 0, inv_tx_enable)
#define NATIVE_TX_RX_M1		NATIVE_FUNC_TX_RX(0, 1, 0, 0) /* no pull */
#define NATIVE_TX_RX_M3		NATIVE_FUNC_TX_RX(0, 3, 0, 0) /* no pull */
#define NATIVE_PU1K_M1		NATIVE_PU1K(1) /* PU1k M1 */

/* Default native functions */
#define Native_M0		NATIVE_DEFAULT(0)
#define Native_M1		NATIVE_DEFAULT(1)
#define Native_M2		NATIVE_DEFAULT(2)
#define Native_M3		NATIVE_DEFAULT(3)
#define Native_M4		NATIVE_DEFAULT(4)
#define Native_M5		NATIVE_DEFAULT(5)
#define Native_M6		NATIVE_DEFAULT(6)
#define Native_M7		NATIVE_DEFAULT(7)
#define Native_M8		NATIVE_DEFAULT(8)

#define GPIO_OUT_LOW		GPO_FUNC(0, 0) /* gpo low */
#define GPIO_OUT_HIGH		GPO_FUNC(0, 1) /* gpo high */
#define GPIO_NC			GPIO_INPUT_PU_20K /* not connect */

/* End marker */
#define GPIO_LIST_END		0xffffffff

#define GPIO_END \
	{  .pad_conf0 = GPIO_LIST_END }

/* 16 DirectIRQs per supported bank */
#define GPIO_MAX_DIRQS			16

#define GPIO_NONE		255

/* Functions / defines for changing GPIOs in romstage */
/* SCORE Pad definitions. */
#define UART_RXD_PAD		82
#define UART_TXD_PAD		83
#define PCU_SMB_CLK_PAD		88
#define PCU_SMB_DATA_PAD	90
#define SOC_DDI1_VDDEN_PAD	16
#define UART1_RXD_PAD		9
#define UART1_TXD_PAD		13
#define DDI2_DDC_SCL		48
#define DDI2_DDC_SDA		53

struct soc_gpio_map {
	u32 pad_conf0;
	u32 pad_conf1;
	u32 pad_val;
	u32 gpe;
	u32 int_mask:1;
	u32 wake_mask:1;
	u32 is_gpio:1;
	u32 skip_config:1;
} __packed;

struct soc_gpio_config {
	const struct soc_gpio_map *north;
	const struct soc_gpio_map *southeast;
	const struct soc_gpio_map *southwest;
	const struct soc_gpio_map *east;
};

/* Description of a GPIO 'community' */
struct gpio_bank {
	const int gpio_count;
	const u8 *gpio_to_pad;
	const int legacy_base;
	const unsigned long pad_base;
	const u8 has_gpe_en:1;
	const u8 has_wake_en:1;
};

typedef enum {
	NATIVE = 0xff,
	GPIO = 0,	  /* Native, no need to set PAD_VALUE */
	GPO = 1,	   /* GPI, input only in PAD_VALUE */
	GPI = 2,	   /* GPO, output only in PAD_VALUE */
	HI_Z = 3,
	NA_GPO = 0,
} gpio_en_t;

typedef enum {
	LO = 0,
	HI = 1,
} gpo_d4_t;

typedef enum {
	F0 = 0,
	F1 = 1,
	F2 = 2,
	F3 = 3
} gpio_func_num_t;

typedef enum {
	_CAP = 1,
	_NOT_CAP = 0
} int_capable_t;

typedef enum {
	P_NONE  = 0,		/* Pull None */
	P_20K_L = 1,		/* Pull Down  20K */
	P_5K_L  = 2,		/* Pull Down  5K */
	P_1K_L  = 4,		/* Pull Down  1K */
	P_20K_H = 9,		/* Pull Up 20K */
	P_5K_H  = 10,		/* Pull Up  5K */
	P_1K_H  = 12		/* Pull Up  1K */
} pull_type_t;

typedef enum {
	DISABLE = 0,	/* Disable */
	ENABLE = 1,	/* Enable */
} park_mode_enb_t;

typedef enum {
	VOLT_3_3 = 0,	/* Working on 3.3 Volts */
	VOLT_1_8 = 1,	/* Working on 1.8 Volts */
} voltage_t;

typedef enum {
	DISABLE_HS = 0,	/* Disable high speed mode */
	ENABLE_HS  = 1,	/* Enable high speed mode */
} hs_mode_t;

typedef enum {
	PULL_UP = 0,	/* On Die Termination Up */
	PULL_DOWN  = 1,	/* On Die Termination Down */
} odt_up_dn_t;

typedef enum {
	DISABLE_OD = 0,	/* On Die Termination Disable */
	ENABLE_OD  = 1,	/* On Die Termination Enable */
} odt_en_t;

typedef enum {
	ONE_BIT = 1,
	TWO_BIT = 3,
	THREE_BIT = 7,
	FOUR_BIT = 15,
	FIVE_BIT = 31,
	SIX_BIT = 63,
	SEVEN_BIT = 127,
	EIGHT_BIT = 255
} bit_t;

typedef enum {
	M0 = 0,
	M1,
	M2,
	M3,
	M4,
	M5,
	M6,
	M7,
	M8,
	M9,
	M10,
	M11,
	M12,
	M13,
} mode_list_t;

typedef enum {
	L0 = 0,
	L1 = 1,
	L2 = 2,
	L3 = 3,
	L4 = 4,
	L5 = 5,
	L6 = 6,
	L7 = 7,
	L8 = 8,
	L9 = 9,
	L10 = 10,
	L11 = 11,
	L12 = 12,
	L13 = 13,
	L14 = 14,
	L15 = 15,
} int_select_t;

typedef enum {
	INT_DIS = 0,
	trig_edge_low = PAD_TRIG_EDGE_LOW,
	trig_edge_high = PAD_TRIG_EDGE_HIGH,
	trig_edge_both = PAD_TRIG_EDGE_BOTH,
	trig_level_high = PAD_TRIG_EDGE_LEVEL | (0 << 4),
	trig_level_low = PAD_TRIG_EDGE_LEVEL | (4 << 4),
} int_type_t;

typedef enum {
	glitch_disable = 0,
	en_edge_detect,
	en_rx_data,
	en_edge_rx_data,
} glitch_cfg;

typedef enum {
	maskable = 0,
	non_maskable,
} mask_t;

typedef enum {
	GPE = 0,
	SMI,
	SCI,
} gpe_config_t;

/*
 * InvertRxTx 7:4
 * 0 - No Inversion
 * 1 - Inversion
 * [0] RX Enable
 * [1] TX Enable
 * [2] RX Data
 * [3] TX Data
 */
typedef enum {
	no_inversion	  = 0,
	inv_rx_enable	 = 0x1,
	inv_tx_enable	 = 0x2,
	inv_rx_tx_enable  = 0x3,
	inv_rx_data	   = 0x4,
	inv_tx_data	   = 0x8,
} invert_rx_tx_t;

#define PAD_VAL_HIGH (1 << 0)

void setup_soc_gpios(struct soc_gpio_config *config, u8 enable_xdp_tap);
struct soc_gpio_config *mainboard_get_gpios(void);

static inline void ncore_select_func(int pad, int func)
{

}

/* These functions require that the input pad be configured as an input GPIO */

static inline int ssus_get_gpio(int pad)
{
	return 0;
}

static inline void ssus_disable_internal_pull(int pad)
{
}

typedef int gpio_t;

int get_gpio(int community_base, int pad0_offset);
uint16_t gpio_family_number(uint8_t community, uint8_t pad);
uint32_t *gpio_pad_config_reg(uint8_t community, uint8_t pad);

void lpc_init(void);
void lpc_set_low_power(void);

#endif /* _SOC_GPIO_H_ */
