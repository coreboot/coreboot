/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Samsung Electronics
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

#ifndef CPU_SAMSUNG_EXYNOS5420_GPIO_H
#define CPU_SAMSUNG_EXYNOS5420_GPIO_H

struct gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pull;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pull;
	unsigned char	res1[8];
};

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8

/* Pin configurations */
#define GPIO_INPUT	0x0
#define GPIO_OUTPUT	0x1
#define GPIO_IRQ	0xf
#define GPIO_FUNC(x)	(x)

/* Pull mode */
#define GPIO_PULL_NONE	0x0
#define GPIO_PULL_DOWN	0x1
#define GPIO_PULL_UP	0x3

/* Drive Strength level */
#define GPIO_DRV_1X	0x0
#define GPIO_DRV_3X	0x1
#define GPIO_DRV_2X	0x2
#define GPIO_DRV_4X	0x3
#define GPIO_DRV_FAST	0x0
#define GPIO_DRV_SLOW	0x1

#define EXYNOS5_GPIO_BASE0	0x13400000
#define EXYNOS5_GPIO_BASE1	0x13400c00
#define EXYNOS5_GPIO_BASE2	0x13410000
#define EXYNOS5_GPIO_BASE3	0x14000000
#define EXYNOS5_GPIO_BASE4	0x14010000
#define EXYNOS5_GPIO_BASE5	0x03860000

enum exynos5_gpio_port {
	/*
	 * Ordered by base address + offset.
	 * ETC registers are special, thus not included.
	 */

	/* base == EXYNOS_GPIO_BASE0 */
	EXYNOS5_GPY7 = EXYNOS5_GPIO_BASE0 + 0x0000,

	/* base == EXYNOS_GPIO_BASE1 */
	EXYNOS5_GPX0 = EXYNOS5_GPIO_BASE1 + 0x0000,
	EXYNOS5_GPX1 = EXYNOS5_GPIO_BASE1 + 0x0020,
	EXYNOS5_GPX2 = EXYNOS5_GPIO_BASE1 + 0x0040,
	EXYNOS5_GPX3 = EXYNOS5_GPIO_BASE1 + 0x0060,

	/* base == EXYNOS_GPIO_BASE2 */
	EXYNOS5_GPC0 = EXYNOS5_GPIO_BASE2 + 0x0000,
	EXYNOS5_GPC1 = EXYNOS5_GPIO_BASE2 + 0x0020,
	EXYNOS5_GPC2 = EXYNOS5_GPIO_BASE2 + 0x0040,
	EXYNOS5_GPC3 = EXYNOS5_GPIO_BASE2 + 0x0060,
	EXYNOS5_GPC4 = EXYNOS5_GPIO_BASE2 + 0x0080,

	EXYNOS5_GPD1 = EXYNOS5_GPIO_BASE2 + 0x00a0,

	EXYNOS5_GPY0 = EXYNOS5_GPIO_BASE2 + 0x00c0,
	EXYNOS5_GPY1 = EXYNOS5_GPIO_BASE2 + 0x00e0,
	EXYNOS5_GPY2 = EXYNOS5_GPIO_BASE2 + 0x0100,
	EXYNOS5_GPY3 = EXYNOS5_GPIO_BASE2 + 0x0120,
	EXYNOS5_GPY4 = EXYNOS5_GPIO_BASE2 + 0x0140,
	EXYNOS5_GPY5 = EXYNOS5_GPIO_BASE2 + 0x0160,
	EXYNOS5_GPY6 = EXYNOS5_GPIO_BASE2 + 0x0180,

	/* base == EXYNOS5_GPIO_BASE3 */
	EXYNOS5_GPE0 = EXYNOS5_GPIO_BASE3 + 0x0000,
	EXYNOS5_GPE1 = EXYNOS5_GPIO_BASE3 + 0x0020,

	EXYNOS5_GPF0 = EXYNOS5_GPIO_BASE3 + 0x0040,
	EXYNOS5_GPF1 = EXYNOS5_GPIO_BASE3 + 0x0060,

	EXYNOS5_GPG0 = EXYNOS5_GPIO_BASE3 + 0x0080,
	EXYNOS5_GPG1 = EXYNOS5_GPIO_BASE3 + 0x00a0,
	EXYNOS5_GPG2 = EXYNOS5_GPIO_BASE3 + 0x00c0,

	EXYNOS5_GPJ4 = EXYNOS5_GPIO_BASE3 + 0x00e0,

	/* base == EXYNOS5_GPIO_BASE4 */
	EXYNOS5_GPA0 = EXYNOS5_GPIO_BASE4 + 0x0000,
	EXYNOS5_GPA1 = EXYNOS5_GPIO_BASE4 + 0x0020,
	EXYNOS5_GPA2 = EXYNOS5_GPIO_BASE4 + 0x0040,

	EXYNOS5_GPB0 = EXYNOS5_GPIO_BASE4 + 0x0060,
	EXYNOS5_GPB1 = EXYNOS5_GPIO_BASE4 + 0x0080,
	EXYNOS5_GPB2 = EXYNOS5_GPIO_BASE4 + 0x00a0,
	EXYNOS5_GPB3 = EXYNOS5_GPIO_BASE4 + 0x00c0,
	EXYNOS5_GPB4 = EXYNOS5_GPIO_BASE4 + 0x00e0,

	EXYNOS5_GPH0 = EXYNOS5_GPIO_BASE4 + 0x0100,

	/* base == EXYNOS5_GPIO_BASE5 */
	EXYNOS5_GPZ0 = EXYNOS5_GPIO_BASE5 + 0x0000,
};

enum {
	/* GPIO banks are split into this many parts */
	EXYNOS_GPIO_NUM_PARTS		= 6
};

/* A list of valid GPIO numbers for the asm-generic/gpio.h interface */
enum exynos5_gpio_pin {
	/* GPIO_PART1_STARTS */
	GPIO_Y70,
	GPIO_Y71,
	GPIO_Y72,
	GPIO_Y73,
	GPIO_Y74,
	GPIO_Y75,
	GPIO_Y76,
	GPIO_Y77,

	/* GPIO_PART2_STARTS */
	GPIO_MAX_PORT_PART_1,
	GPIO_X00 = GPIO_MAX_PORT_PART_1,	/* 0x08 */
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
	GPIO_C00 = GPIO_MAX_PORT_PART_2,	/* 0x28 */
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
	GPIO_C40,
	GPIO_C41,
	GPIO_C42,
	GPIO_C43,
	GPIO_C44,
	GPIO_C45,
	GPIO_C46,
	GPIO_C47,

	GPIO_D10,				/* 0x50 */
	GPIO_D11,
	GPIO_D12,
	GPIO_D13,
	GPIO_D14,
	GPIO_D15,
	GPIO_D16,
	GPIO_D17,

	GPIO_Y00,				/* 0x58 */
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

	/* GPIO_PART4_STARTS */
	GPIO_MAX_PORT_PART_3,
	GPIO_E00 = GPIO_MAX_PORT_PART_3,	/* 0x90 */
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

	GPIO_F00,				/* 0xa0 */
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

	GPIO_G00,				/* 0xb0 */
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

	GPIO_J40,				/* 0xc8 */
	GPIO_J41,
	GPIO_J42,
	GPIO_J43,
	GPIO_J44,
	GPIO_J45,
	GPIO_J46,
	GPIO_J47,

	/* GPIO_PART5_STARTS */
	GPIO_MAX_PORT_PART_4,
	GPIO_A00 = GPIO_MAX_PORT_PART_4,	/* 0xd0 */
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

	GPIO_B00,				/* 0xe8 */
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
	GPIO_B40,
	GPIO_B41,
	GPIO_B42,
	GPIO_B43,
	GPIO_B44,
	GPIO_B45,
	GPIO_B46,
	GPIO_B47,

	GPIO_H00,				/* 0x110 */
	GPIO_H01,
	GPIO_H02,
	GPIO_H03,
	GPIO_H04,
	GPIO_H05,
	GPIO_H06,
	GPIO_H07,

	/* GPIO_PART6_STARTS */
	GPIO_MAX_PORT_PART_5,
	GPIO_Z00 = GPIO_MAX_PORT_PART_5,	/* 0x118 */
	GPIO_Z01,
	GPIO_Z02,
	GPIO_Z03,
	GPIO_Z04,
	GPIO_Z05,
	GPIO_Z06,
	GPIO_Z07,
	GPIO_MAX_PORT
};

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

/*
 * reads only a single GPIO
 *
 * @param gpio		GPIO to read
 * @return -1 if the value cannot be determined. Otherwise returns
 *              the corresponding MVL3 enum value.
 */
int gpio_read_mvl3(unsigned gpio);

void gpio_info(void);

/*
 * Generic GPIO API for U-Boot
 *
 * GPIOs are numbered from 0 to GPIO_COUNT-1 which value is defined
 * by the SOC/architecture.
 *
 * Each GPIO can be an input or output. If an input then its value can
 * be read as 0 or 1. If an output then its value can be set to 0 or 1.
 * If you try to write an input then the value is undefined. If you try
 * to read an output, barring something very unusual,  you will get
 * back the value of the output that you previously set.
 *
 * In some cases the operation may fail, for example if the GPIO number
 * is out of range, or the GPIO is not available because its pin is
 * being used by another function. In that case, functions may return
 * an error value of -1.
 */

/**
 * Make a GPIO an input.
 *
 * @param gpio	GPIO number
 * @return 0 if ok, -1 on error
 */
int gpio_direction_input(unsigned gpio);

/**
 * Make a GPIO an output, and set its value.
 *
 * @param gpio	GPIO number
 * @param value	GPIO value (0 for low or 1 for high)
 * @return 0 if ok, -1 on error
 */
int gpio_direction_output(unsigned gpio, int value);

/**
 * Get a GPIO's value. This will work whether the GPIO is an input
 * or an output.
 *
 * @param gpio	GPIO number
 * @return 0 if low, 1 if high, -1 on error
 */
int gpio_get_value(unsigned gpio);

/**
 * Set an output GPIO's value. The GPIO must already be an output or
 * this function may have no effect.
 *
 * @param gpio	GPIO number
 * @param value	GPIO value (0 for low or 1 for high)
 * @return 0 if ok, -1 on error
 */
int gpio_set_value(unsigned gpio, int value);

/*
 * Many-value logic (3 states). This can be used for inputs whereby presence
 * of external pull-up or pull-down resistors can be added to overcome internal
 * pull-ups/pull-downs and force a single value.
 *
 * Thus, external pull resistors can force a 0 or 1 and if the value changes
 * along with internal pull-up/down enable then the input is floating.
 *
 *     Vpd | Vpu | MVL
 *    -----------------
 *      0  |  0  | 0
 *    -----------------
 *      0  |  1  | Z    <-- floating input will follow internal pull up/down
 *    -----------------
 *      1  |  1  | 1
 */
enum mvl3 {
	LOGIC_0,
	LOGIC_1,
	LOGIC_Z,		/* high impedence / tri-stated / floating */
};

#endif	/* CPU_SAMSUNG_EXYNOS5420_GPIO_H */
