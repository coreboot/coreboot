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

#include <stdint.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <soc/iomap.h>
#include <soc/gpio.h>

/* Use a wrapper for register addresses for different banks. */
const static struct gpio_config_regs regs[GPIO_NUM_BANKS] = {
	[0] = { .use_sel = GPIO_1_USE_SEL, .io_sel = GPIO_1_IO_SEL,
		.level = GPIO_1_LVL, .nmi_en = GPIO_1_NMI_EN,
		.blink_en = GPIO_1_BLINK, .invert_input = GPIO_1_INVERT },
	[1] = { .use_sel = GPIO_2_USE_SEL, .io_sel = GPIO_2_IO_SEL,
		.level = GPIO_2_LVL, .nmi_en = GPIO_2_NMI_EN,
		.blink_en = REG_INVALID, .invert_input = REG_INVALID },
	[2] = { .use_sel = GPIO_3_USE_SEL, .io_sel = GPIO_3_IO_SEL,
		.level = GPIO_3_LVL, .nmi_en = GPIO_3_NMI_EN,
		.blink_en = REG_INVALID, .invert_input = REG_INVALID },
	};

#define SETUP_GPIO_REG(reg, bit, bank) { uint32_t val; \
			val = inl(GPIO_BASE_ADDRESS + regs[(bank)].reg); \
			val &= ~(1 << (bit)); \
			val |= ((pin->reg) << (bit)); \
			outl(val, GPIO_BASE_ADDRESS + regs[(bank)].reg); }

/* Initialize the GPIOs as defined on mainboard level. */
void init_gpios(const struct gpio_config config[])
{
	uint8_t bank, bit;
	const struct gpio_config *pin;

	if (!config)
		return;
	/* Set up every GPIO in the table to the requested function. */
	for (pin = config; pin->use_sel != GPIO_LIST_END; pin++) {
		/* Skip unsupported GPIO numbers. */
		if (pin->num > MAX_GPIO_NUM || pin->num == 13)
			continue;
		bank = pin->num / 32;
		bit = pin->num % 32;
		if (pin->use_sel == GPIO_MODE_GPIO) {
			/* Setting level register first avoids possible short
			 * pulses on the pin if the output level differs from
			 * the register default value.
			 */
			if (pin->io_sel == GPIO_OUTPUT)
				SETUP_GPIO_REG(level, bit, bank);
			/* Now set the GPIO direction and NMI selection. */
			SETUP_GPIO_REG(io_sel, bit, bank);
			SETUP_GPIO_REG(nmi_en, bit, bank);
		}
		/* Now set the pin mode as requested */
		SETUP_GPIO_REG(use_sel, bit, bank);
		/* The extended functions like inverting and blinking are only
		 * supported by GPIOs on bank 0.
		 */
		if (bank)
			continue;
		/* Blinking is available only for outputs */
		if (pin->io_sel == GPIO_OUTPUT)
			SETUP_GPIO_REG(blink_en, bit, bank);
		/* Inverting is available only for inputs */
		if (pin->io_sel == GPIO_INPUT)
			SETUP_GPIO_REG(invert_input, bit, bank);
	}
}

/* Get GPIO pin value */
int gpio_get(gpio_t gpio)
{
	uint8_t bank, bit;

	bank = gpio / 32;
	bit = gpio % 32;
	return (inl(GPIO_BASE_ADDRESS + regs[bank].level) & (1 << bit)) ? 1 : 0;
}

/* Set GPIO pin value */
void gpio_set(gpio_t gpio, int value)
{
	uint32_t reg;
	uint8_t bank, bit;

	bank = gpio / 32;
	bit = gpio % 32;
	reg = inl(GPIO_BASE_ADDRESS + regs[bank].level);
	reg &= ~(1 << bit);
	reg |= (!!value << bit);
	outl(reg, GPIO_BASE_ADDRESS + regs[bank].level);
}
