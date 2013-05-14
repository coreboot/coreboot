/*
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
 */

#ifndef EXYNOS5250_GPIO_H_
#define EXYNOS5250_GPIO_H_

#include <cpu/samsung/exynos5250/cpu.h>	/* FIXME: for S5PC110_GPIO_BASE */

struct s5p_gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pull;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pull;
	unsigned char	res1[8];
};

struct s5pc100_gpio {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank b;
	struct s5p_gpio_bank c;
	struct s5p_gpio_bank d;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank g3;
	struct s5p_gpio_bank i;
	struct s5p_gpio_bank j0;
	struct s5p_gpio_bank j1;
	struct s5p_gpio_bank j2;
	struct s5p_gpio_bank j3;
	struct s5p_gpio_bank j4;
	struct s5p_gpio_bank k0;
	struct s5p_gpio_bank k1;
	struct s5p_gpio_bank k2;
	struct s5p_gpio_bank k3;
	struct s5p_gpio_bank l0;
	struct s5p_gpio_bank l1;
	struct s5p_gpio_bank l2;
	struct s5p_gpio_bank l3;
	struct s5p_gpio_bank l4;
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank h1;
	struct s5p_gpio_bank h2;
	struct s5p_gpio_bank h3;
};

struct s5pc110_gpio {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank b;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank g3;
	struct s5p_gpio_bank i;
	struct s5p_gpio_bank j0;
	struct s5p_gpio_bank j1;
	struct s5p_gpio_bank j2;
	struct s5p_gpio_bank j3;
	struct s5p_gpio_bank j4;
	struct s5p_gpio_bank mp0_1;
	struct s5p_gpio_bank mp0_2;
	struct s5p_gpio_bank mp0_3;
	struct s5p_gpio_bank mp0_4;
	struct s5p_gpio_bank mp0_5;
	struct s5p_gpio_bank mp0_6;
	struct s5p_gpio_bank mp0_7;
	struct s5p_gpio_bank mp1_0;
	struct s5p_gpio_bank mp1_1;
	struct s5p_gpio_bank mp1_2;
	struct s5p_gpio_bank mp1_3;
	struct s5p_gpio_bank mp1_4;
	struct s5p_gpio_bank mp1_5;
	struct s5p_gpio_bank mp1_6;
	struct s5p_gpio_bank mp1_7;
	struct s5p_gpio_bank mp1_8;
	struct s5p_gpio_bank mp2_0;
	struct s5p_gpio_bank mp2_1;
	struct s5p_gpio_bank mp2_2;
	struct s5p_gpio_bank mp2_3;
	struct s5p_gpio_bank mp2_4;
	struct s5p_gpio_bank mp2_5;
	struct s5p_gpio_bank mp2_6;
	struct s5p_gpio_bank mp2_7;
	struct s5p_gpio_bank mp2_8;
	struct s5p_gpio_bank res1[48];
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank h1;
	struct s5p_gpio_bank h2;
	struct s5p_gpio_bank h3;
};

/* functions */
void s5p_gpio_cfg_pin(struct s5p_gpio_bank *bank, int gpio, int cfg);
void s5p_gpio_direction_output(struct s5p_gpio_bank *bank, int gpio, int en);
void s5p_gpio_direction_input(struct s5p_gpio_bank *bank, int gpio);
void s5p_gpio_set_value(struct s5p_gpio_bank *bank, int gpio, int en);
unsigned int s5p_gpio_get_value(struct s5p_gpio_bank *bank, int gpio);
void s5p_gpio_set_pull(struct s5p_gpio_bank *bank, int gpio, int mode);
void s5p_gpio_set_drv(struct s5p_gpio_bank *bank, int gpio, int mode);
void s5p_gpio_set_rate(struct s5p_gpio_bank *bank, int gpio, int mode);

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8

static inline unsigned int s5p_gpio_base(int nr)
{
	return S5PC110_GPIO_BASE;
}

#define s5pc110_gpio_get_nr(bank, pin) \
	((((((unsigned int)&(((struct s5pc110_gpio *)S5PC110_GPIO_BASE)->bank))\
	    - S5PC110_GPIO_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

/* Pin configurations */
#define GPIO_INPUT	0x0
#define GPIO_OUTPUT	0x1
#define GPIO_IRQ	0xf
#define GPIO_FUNC(x)	(x)

/* Pull mode */
#define GPIO_PULL_NONE	0x0
#define GPIO_PULL_DOWN	0x1
#define GPIO_PULL_UP	0x2

/* Drive Strength level */
#define GPIO_DRV_1X	0x0
#define GPIO_DRV_3X	0x1
#define GPIO_DRV_2X	0x2
#define GPIO_DRV_4X	0x3
#define GPIO_DRV_FAST	0x0
#define GPIO_DRV_SLOW	0x1

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8

/* Pin configurations */
#define EXYNOS_GPIO_INPUT	0x0
#define EXYNOS_GPIO_OUTPUT	0x1
#define EXYNOS_GPIO_IRQ	0xf
#define EXYNOS_GPIO_FUNC(x)	(x)

/* Pull mode */
#define EXYNOS_GPIO_PULL_NONE	0x0
#define EXYNOS_GPIO_PULL_DOWN	0x1
#define EXYNOS_GPIO_PULL_UP	0x3

/* Drive Strength level */
#define EXYNOS_GPIO_DRV_1X	0x0
#define EXYNOS_GPIO_DRV_3X	0x1
#define EXYNOS_GPIO_DRV_2X	0x2
#define EXYNOS_GPIO_DRV_4X	0x3
#define EXYNOS_GPIO_DRV_FAST	0x0
#define EXYNOS_GPIO_DRV_SLOW	0x1

#define EXYNOS5_GPIO_BASE0	0x11400000
#define EXYNOS5_GPIO_BASE1	0x13400000
#define EXYNOS5_GPIO_BASE2	0x10d10000
#define EXYNOS5_GPIO_BASE3	0x03860000

enum exynos5_gpio_port {
	/*
	 * Ordered by base address + offset.
	 * ETC registers are special, thus not included.
	 */

	/* base == EXYNOS_GPIO_BASE0 */
	EXYNOS5_GPA0 = EXYNOS5_GPIO_BASE0 + 0x0000,
	EXYNOS5_GPA1 = EXYNOS5_GPIO_BASE0 + 0x0020,
	EXYNOS5_GPA2 = EXYNOS5_GPIO_BASE0 + 0x0040,

	EXYNOS5_GPB0 = EXYNOS5_GPIO_BASE0 + 0x0060,
	EXYNOS5_GPB1 = EXYNOS5_GPIO_BASE0 + 0x0080,
	EXYNOS5_GPB2 = EXYNOS5_GPIO_BASE0 + 0x00a0,
	EXYNOS5_GPB3 = EXYNOS5_GPIO_BASE0 + 0x00c0,

	EXYNOS5_GPC0 = EXYNOS5_GPIO_BASE0 + 0x00e0,
	EXYNOS5_GPC1 = EXYNOS5_GPIO_BASE0 + 0x0100,
	EXYNOS5_GPC2 = EXYNOS5_GPIO_BASE0 + 0x0120,
	EXYNOS5_GPC3 = EXYNOS5_GPIO_BASE0 + 0x0140,

	EXYNOS5_GPD0 = EXYNOS5_GPIO_BASE0 + 0x0160,
	EXYNOS5_GPD1 = EXYNOS5_GPIO_BASE0 + 0x0180,

	EXYNOS5_GPY0 = EXYNOS5_GPIO_BASE0 + 0x01a0,
	EXYNOS5_GPY1 = EXYNOS5_GPIO_BASE0 + 0x01c0,
	EXYNOS5_GPY2 = EXYNOS5_GPIO_BASE0 + 0x01e0,
	EXYNOS5_GPY3 = EXYNOS5_GPIO_BASE0 + 0x0200,
	EXYNOS5_GPY4 = EXYNOS5_GPIO_BASE0 + 0x0220,
	EXYNOS5_GPY5 = EXYNOS5_GPIO_BASE0 + 0x0240,
	EXYNOS5_GPY6 = EXYNOS5_GPIO_BASE0 + 0x0260,

	EXYNOS5_GPX0 = EXYNOS5_GPIO_BASE0 + 0x0c00,
	EXYNOS5_GPX1 = EXYNOS5_GPIO_BASE0 + 0x0c20,
	EXYNOS5_GPX2 = EXYNOS5_GPIO_BASE0 + 0x0c40,
	EXYNOS5_GPX3 = EXYNOS5_GPIO_BASE0 + 0x0c60,

	/* base == EXYNOS_GPIO_BASE1 */
	EXYNOS5_GPE0 = EXYNOS5_GPIO_BASE1 + 0x0000,
	EXYNOS5_GPE1 = EXYNOS5_GPIO_BASE1 + 0x0020,

	EXYNOS5_GPF0 = EXYNOS5_GPIO_BASE1 + 0x0040,
	EXYNOS5_GPF1 = EXYNOS5_GPIO_BASE1 + 0x0060,

	EXYNOS5_GPG0 = EXYNOS5_GPIO_BASE1 + 0x0080,
	EXYNOS5_GPG1 = EXYNOS5_GPIO_BASE1 + 0x00a0,
	EXYNOS5_GPG2 = EXYNOS5_GPIO_BASE1 + 0x00c0,

	EXYNOS5_GPH0 = EXYNOS5_GPIO_BASE1 + 0x00e0,
	EXYNOS5_GPH1 = EXYNOS5_GPIO_BASE1 + 0x0100,

	/* base == EXYNOS_GPIO_BASE2 */
	EXYNOS5_GPV0 = EXYNOS5_GPIO_BASE2 + 0x0000,
	EXYNOS5_GPV1 = EXYNOS5_GPIO_BASE2 + 0x0020,
	EXYNOS5_GPV2 = EXYNOS5_GPIO_BASE2 + 0x0060,
	EXYNOS5_GPV3 = EXYNOS5_GPIO_BASE2 + 0x0080,
	EXYNOS5_GPV4 = EXYNOS5_GPIO_BASE2 + 0x00c0,

	/* base == EXYNOS_GPIO_BASE3 */
	EXYNOS5_GPZ = EXYNOS5_GPIO_BASE3 + 0x0000,
};

struct exynos5_gpio_part1 {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank a2;
	struct s5p_gpio_bank b0;
	struct s5p_gpio_bank b1;
	struct s5p_gpio_bank b2;
	struct s5p_gpio_bank b3;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank c2;
	struct s5p_gpio_bank c3;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank y0;
	struct s5p_gpio_bank y1;
	struct s5p_gpio_bank y2;
	struct s5p_gpio_bank y3;
	struct s5p_gpio_bank y4;
	struct s5p_gpio_bank y5;
	struct s5p_gpio_bank y6;
};

struct exynos5_gpio_part2 {
	struct s5p_gpio_bank x0;
	struct s5p_gpio_bank x1;
	struct s5p_gpio_bank x2;
	struct s5p_gpio_bank x3;
};

struct exynos5_gpio_part3 {
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank h1;
};

struct exynos5_gpio_part4 {
	struct s5p_gpio_bank v0;
	struct s5p_gpio_bank v1;
	struct s5p_gpio_bank v2;
	struct s5p_gpio_bank v3;
};

struct exynos5_gpio_part5 {
	struct s5p_gpio_bank v4;
};

struct exynos5_gpio_part6 {
	struct s5p_gpio_bank z;
};

enum {
	/* GPIO banks are split into this many parts */
	EXYNOS_GPIO_NUM_PARTS		= 6
};

/* A list of valid GPIO numbers for the asm-generic/gpio.h interface */
enum exynos5_gpio_pin {
	/* GPIO_PART1_STARTS */
	GPIO_A00,
	GPIO_A01,
	GPIO_A02,
	GPIO_A03,
	GPIO_A04,
	GPIO_A05,
	GPIO_A06,
	GPIO_A07,
	GPIO_A10,
	GPIO_A11,
	GPIO_A12,
	GPIO_A13,
	GPIO_A14,
	GPIO_A15,
	GPIO_A16,
	GPIO_A17,
	GPIO_A20,
	GPIO_A21,
	GPIO_A22,
	GPIO_A23,
	GPIO_A24,
	GPIO_A25,
	GPIO_A26,
	GPIO_A27,
	GPIO_B00,				/* 0x18 */
	GPIO_B01,
	GPIO_B02,
	GPIO_B03,
	GPIO_B04,
	GPIO_B05,
	GPIO_B06,
	GPIO_B07,
	GPIO_B10,
	GPIO_B11,
	GPIO_B12,
	GPIO_B13,
	GPIO_B14,
	GPIO_B15,
	GPIO_B16,
	GPIO_B17,
	GPIO_B20,
	GPIO_B21,
	GPIO_B22,
	GPIO_B23,
	GPIO_B24,
	GPIO_B25,
	GPIO_B26,
	GPIO_B27,
	GPIO_B30,
	GPIO_B31,
	GPIO_B32,
	GPIO_B33,
	GPIO_B34,
	GPIO_B35,
	GPIO_B36,
	GPIO_B37,
	GPIO_C00,				/* 0x38 */
	GPIO_C01,
	GPIO_C02,
	GPIO_C03,
	GPIO_C04,
	GPIO_C05,
	GPIO_C06,
	GPIO_C07,
	GPIO_C10,
	GPIO_C11,
	GPIO_C12,
	GPIO_C13,
	GPIO_C14,
	GPIO_C15,
	GPIO_C16,
	GPIO_C17,
	GPIO_C20,
	GPIO_C21,
	GPIO_C22,
	GPIO_C23,
	GPIO_C24,
	GPIO_C25,
	GPIO_C26,
	GPIO_C27,
	GPIO_C30,
	GPIO_C31,
	GPIO_C32,
	GPIO_C33,
	GPIO_C34,
	GPIO_C35,
	GPIO_C36,
	GPIO_C37,
	GPIO_D00,				/* 0x58 */
	GPIO_D01,
	GPIO_D02,
	GPIO_D03,
	GPIO_D04,
	GPIO_D05,
	GPIO_D06,
	GPIO_D07,
	GPIO_D10,
	GPIO_D11,
	GPIO_D12,
	GPIO_D13,
	GPIO_D14,
	GPIO_D15,
	GPIO_D16,
	GPIO_D17,
	GPIO_Y00,				/* 0x68 */
	GPIO_Y01,
	GPIO_Y02,
	GPIO_Y03,
	GPIO_Y04,
	GPIO_Y05,
	GPIO_Y06,
	GPIO_Y07,
	GPIO_Y10,
	GPIO_Y11,
	GPIO_Y12,
	GPIO_Y13,
	GPIO_Y14,
	GPIO_Y15,
	GPIO_Y16,
	GPIO_Y17,
	GPIO_Y20,
	GPIO_Y21,
	GPIO_Y22,
	GPIO_Y23,
	GPIO_Y24,
	GPIO_Y25,
	GPIO_Y26,
	GPIO_Y27,
	GPIO_Y30,
	GPIO_Y31,
	GPIO_Y32,
	GPIO_Y33,
	GPIO_Y34,
	GPIO_Y35,
	GPIO_Y36,
	GPIO_Y37,
	GPIO_Y40,
	GPIO_Y41,
	GPIO_Y42,
	GPIO_Y43,
	GPIO_Y44,
	GPIO_Y45,
	GPIO_Y46,
	GPIO_Y47,
	GPIO_Y50,
	GPIO_Y51,
	GPIO_Y52,
	GPIO_Y53,
	GPIO_Y54,
	GPIO_Y55,
	GPIO_Y56,
	GPIO_Y57,
	GPIO_Y60,
	GPIO_Y61,
	GPIO_Y62,
	GPIO_Y63,
	GPIO_Y64,
	GPIO_Y65,
	GPIO_Y66,
	GPIO_Y67,

	/* GPIO_PART2_STARTS */
	GPIO_MAX_PORT_PART_1,
	GPIO_X00 = GPIO_MAX_PORT_PART_1,	/* 0xa0 */
	GPIO_X01,
	GPIO_X02,
	GPIO_X03,
	GPIO_X04,
	GPIO_X05,
	GPIO_X06,
	GPIO_X07,
	GPIO_X10,
	GPIO_X11,
	GPIO_X12,
	GPIO_X13,
	GPIO_X14,
	GPIO_X15,
	GPIO_X16,
	GPIO_X17,
	GPIO_X20,
	GPIO_X21,
	GPIO_X22,
	GPIO_X23,
	GPIO_X24,
	GPIO_X25,
	GPIO_X26,
	GPIO_X27,
	GPIO_X30,
	GPIO_X31,
	GPIO_X32,
	GPIO_X33,
	GPIO_X34,
	GPIO_X35,
	GPIO_X36,
	GPIO_X37,

	/* GPIO_PART3_STARTS */
	GPIO_MAX_PORT_PART_2,
	GPIO_E00 = GPIO_MAX_PORT_PART_2,	/* 0xc0 */
	GPIO_E01,
	GPIO_E02,
	GPIO_E03,
	GPIO_E04,
	GPIO_E05,
	GPIO_E06,
	GPIO_E07,
	GPIO_E10,
	GPIO_E11,
	GPIO_E12,
	GPIO_E13,
	GPIO_E14,
	GPIO_E15,
	GPIO_E16,
	GPIO_E17,
	GPIO_F00,				/* 0xd0 */
	GPIO_F01,
	GPIO_F02,
	GPIO_F03,
	GPIO_F04,
	GPIO_F05,
	GPIO_F06,
	GPIO_F07,
	GPIO_F10,
	GPIO_F11,
	GPIO_F12,
	GPIO_F13,
	GPIO_F14,
	GPIO_F15,
	GPIO_F16,
	GPIO_F17,
	GPIO_G00,
	GPIO_G01,
	GPIO_G02,
	GPIO_G03,
	GPIO_G04,
	GPIO_G05,
	GPIO_G06,
	GPIO_G07,
	GPIO_G10,
	GPIO_G11,
	GPIO_G12,
	GPIO_G13,
	GPIO_G14,
	GPIO_G15,
	GPIO_G16,
	GPIO_G17,
	GPIO_G20,
	GPIO_G21,
	GPIO_G22,
	GPIO_G23,
	GPIO_G24,
	GPIO_G25,
	GPIO_G26,
	GPIO_G27,
	GPIO_H00,
	GPIO_H01,
	GPIO_H02,
	GPIO_H03,
	GPIO_H04,
	GPIO_H05,
	GPIO_H06,
	GPIO_H07,
	GPIO_H10,
	GPIO_H11,
	GPIO_H12,
	GPIO_H13,
	GPIO_H14,
	GPIO_H15,
	GPIO_H16,
	GPIO_H17,

	/* GPIO_PART4_STARTS */
	GPIO_MAX_PORT_PART_3,
	GPIO_V00 = GPIO_MAX_PORT_PART_3,
	GPIO_V01,
	GPIO_V02,
	GPIO_V03,
	GPIO_V04,
	GPIO_V05,
	GPIO_V06,
	GPIO_V07,
	GPIO_V10,
	GPIO_V11,
	GPIO_V12,
	GPIO_V13,
	GPIO_V14,
	GPIO_V15,
	GPIO_V16,
	GPIO_V17,
	GPIO_V20,
	GPIO_V21,
	GPIO_V22,
	GPIO_V23,
	GPIO_V24,
	GPIO_V25,
	GPIO_V26,
	GPIO_V27,
	GPIO_V30,
	GPIO_V31,
	GPIO_V32,
	GPIO_V33,
	GPIO_V34,
	GPIO_V35,
	GPIO_V36,
	GPIO_V37,

	/* GPIO_PART5_STARTS */
	GPIO_MAX_PORT_PART_4,
	GPIO_V40 = GPIO_MAX_PORT_PART_4,
	GPIO_V41,
	GPIO_V42,
	GPIO_V43,
	GPIO_V44,
	GPIO_V45,
	GPIO_V46,
	GPIO_V47,

	/* GPIO_PART6_STARTS */
	GPIO_MAX_PORT_PART_5,
	GPIO_Z0 = GPIO_MAX_PORT_PART_5,
	GPIO_Z1,
	GPIO_Z2,
	GPIO_Z3,
	GPIO_Z4,
	GPIO_Z5,
	GPIO_Z6,
	GPIO_MAX_PORT
};

#define gpio_status		gpio_info

/**
 * Set GPIO pin configuration.
 *
 * @param gpio	GPIO pin
 * @param cfg	Either GPIO_INPUT, GPIO_OUTPUT, or GPIO_IRQ
 */
void gpio_cfg_pin(int gpio, int cfg);

/**
 * Set GPIO pull mode.
 *
 * @param gpio	GPIO pin
 * @param mode	Either GPIO_PULL_DOWN or GPIO_PULL_UP
 */
void gpio_set_pull(int gpio, int mode);

/**
 * Set GPIO drive strength level.
 *
 * @param gpio	GPIO pin
 * @param mode	Either GPIO_DRV_1X, GPIO_DRV_2X, GPIO_DRV_3X, or GPIO_DRV_4X
 */
void gpio_set_drv(int gpio, int mode);

/**
 * Set GPIO drive rate.
 *
 * @param gpio	GPIO pin
 * @param mode	Either GPIO_DRV_FAST or GPIO_DRV_SLOW
 */
void gpio_set_rate(int gpio, int mode);

/* FIXME(dhendrix) use generic arch/gpio.h API instead */
//int gpio_direction_input(unsigned gpio);
//int gpio_direction_output(unsigned gpio, int value);

/**
 * Decode a list of GPIOs into an integer.
 *
 * TODO(sjg@chromium.org): This could perhaps become a generic function?
 *
 * Each GPIO pin can be put into three states using external resistors:
 *	- pulled up
 *	- pulled down
 *	- not connected
 *
 * Read each GPIO in turn to produce an integer value. The first GPIO
 * produces a number 1 * (0 to 2), the second produces 3 * (0 to 2), etc.
 * In this way, each GPIO increases the number of possible states by a
 * factor of 3.
 *
 * @param gpio_list	List of GPIO numbers to decode
 * @param count		Number of GPIOs in list
 * @return -1 if the value cannot be determined, or any GPIO number is
 *		invalid. Otherwise returns the calculated value
 */
int gpio_decode_number(unsigned gpio_list[], int count);

/*
 * similar to gpio_decode_number, but reads only a single GPIO
 *
 * @param gpio		GPIO to read
 * @return -1 if the value cannot be determined. Otherwise returns
 *              the corresponding MVL3 enum value.
 */
int gpio_read_mvl3(unsigned gpio);

void gpio_info(void);

#endif	/* EXYNOS5250_GPIO_H_ */
