/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Siemens AG
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
#ifndef FSP_BROADWELL_DE_GPIO_H_
#define FSP_BROADWELL_DE_GPIO_H_

#include <stdint.h>

/* Chipset owned GPIO configuration registers */
#define GPIO_1_USE_SEL		0x00
#define GPIO_1_IO_SEL		0x04
#define GPIO_1_LVL		0x0c
#define GPIO_1_BLINK		0x18
#define GPIO_1_NMI_EN		0x28
#define GPIO_1_INVERT		0x2c
#define GPIO_2_USE_SEL		0x30
#define GPIO_2_IO_SEL		0x34
#define GPIO_2_LVL		0x38
#define GPIO_2_NMI_EN		0x3c
#define GPIO_3_USE_SEL		0x40
#define GPIO_3_IO_SEL		0x44
#define GPIO_3_LVL		0x48
#define GPIO_3_NMI_EN		0x50
#define REG_INVALID		0xff

/* The pin can either be a GPIO or connected to the native function. */
#define GPIO_MODE_NATIVE	0
#define GPIO_MODE_GPIO		1
/* Once configured as GPIO the pin can be an input or an output. */
#define GPIO_OUTPUT		0
#define GPIO_INPUT		1
#define GPIO_NMI_EN		1
/* For output GPIO mode the pin can either drive high or low level. */
#define GPIO_OUT_LEVEL_LOW	0
#define GPIO_OUT_LEVEL_HIGH	1
/* The following functions are only valid for GPIO bank 1. */
#define GPIO_OUT_BLINK		1
#define GPIO_IN_INVERT		1

#define GPIO_NUM_BANKS		3
#define MAX_GPIO_NUM		75	/* 0 based GPIO number */
#define GPIO_LIST_END		0xff

/* Define possible GPIO configurations. */
#define PCH_GPIO_END \
	{ .use_sel = GPIO_LIST_END }

#define PCH_GPIO_NATIVE(gpio) { \
	 .num = (gpio), \
	 .use_sel = GPIO_MODE_NATIVE }

#define PCH_GPIO_INPUT(gpio) { \
	.num = (gpio), \
	.use_sel = GPIO_MODE_GPIO, \
	.io_sel = GPIO_INPUT }

#define PCH_GPIO_INPUT_INVERT(gpio) { \
	.num = (gpio), \
	.use_sel = GPIO_MODE_GPIO, \
	.io_sel = GPIO_INPUT, \
	.invert_input = GPIO_IN_INVERT }

#define PCH_GPIO_INPUT_NMI(gpio) { \
	.num = (gpio), \
	.use_sel = GPIO_MODE_GPIO, \
	.io_sel = GPIO_INPUT, \
	.nmi_en = GPIO_NMI_EN }

#define PCH_GPIO_OUT_LOW(gpio) { \
	.num = (gpio), \
	.use_sel = GPIO_MODE_GPIO, \
	.io_sel = GPIO_OUTPUT, \
	.level = GPIO_OUT_LEVEL_LOW }

#define PCH_GPIO_OUT_HIGH(gpio) { \
	.num = (gpio), \
	.use_sel = GPIO_MODE_GPIO, \
	.io_sel = GPIO_OUTPUT, \
	.level = GPIO_OUT_LEVEL_HIGH }

#define PCH_GPIO_OUT_BLINK(gpio) { \
	.num = (gpio), \
	.use_sel = GPIO_MODE_GPIO, \
	.io_sel = GPIO_OUTPUT, \
	.blink_en = GPIO_OUT_BLINK }

struct gpio_config {
	uint8_t num;
	uint8_t use_sel;
	uint8_t io_sel;
	uint8_t level;
	uint8_t blink_en;
	uint8_t nmi_en;
	uint8_t invert_input;
} __packed;

/* Unfortunately the register layout is not linear between different GPIO banks.
 * In addition not every bank has all the functions so that some registers might
 * be missing on a particular bank. To make the code better readable introduce a
 * wrapper structure for the register addresses for every bank.
 */
struct gpio_config_regs {
	uint8_t use_sel;
	uint8_t io_sel;
	uint8_t level;
	uint8_t nmi_en;
	uint8_t blink_en;
	uint8_t invert_input;
};

/* Define gpio_t here to be able to use src/include/gpio.h for gpio_set() and
   gpio_get().*/
typedef uint8_t gpio_t;

/* Configure GPIOs with mainboard provided settings */
void init_gpios(const struct gpio_config config[]);

#endif /* FSP_BROADWELL_DE_GPIO_H_ */
