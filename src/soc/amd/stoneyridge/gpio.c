/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/gpio.h>

static uintptr_t gpio_get_address(gpio_t gpio_num)
{
	uintptr_t gpio_address;

	if (gpio_num < 64)
		gpio_address = GPIO_BANK0_CONTROL(gpio_num);
	else if (gpio_num < 128)
		gpio_address = GPIO_BANK1_CONTROL(gpio_num);
	else
		gpio_address = GPIO_BANK2_CONTROL(gpio_num);

	return gpio_address;
}

int gpio_get(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);

	return !!(reg & GPIO_PIN_STS);
}

void gpio_set(gpio_t gpio_num, int value)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_OUTPUT_MASK;
	reg |=  !!value << GPIO_OUTPUT_SHIFT;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input_pulldown(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_PULLUP_ENABLE;
	reg |=  GPIO_PULLDOWN_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input_pullup(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_PULLDOWN_ENABLE;
	reg |=  GPIO_PULLUP_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_OUTPUT_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_output(gpio_t gpio_num, int value)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg |=  GPIO_OUTPUT_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
	gpio_set(gpio_num, value);
}

const char *gpio_acpi_path(gpio_t gpio)
{
	return "\\_SB.GPIO";
}

uint16_t gpio_acpi_pin(gpio_t gpio)
{
	return gpio;
}

void gpio_set_interrupt(gpio_t gpio, uint32_t flags)
{
	uintptr_t gpio_address = gpio_get_address(gpio);
	uint32_t reg = read32((void *)gpio_address);

	/* Clear registers that are being updated */
	reg &= ~(GPIO_TRIGGER_MASK | GPIO_ACTIVE_MASK | GPIO_INTERRUPT_MASK);

	/* Clear any extra bits in the flags */
	flags &= (GPIO_TRIGGER_MASK | GPIO_ACTIVE_MASK | GPIO_INTERRUPT_MASK);

	write32((void *)gpio_address, reg | flags);
}

int gpio_interrupt_status(gpio_t gpio)
{
	uintptr_t gpio_address = gpio_get_address(gpio);
	uint32_t reg = read32((void *)gpio_address);

	if (reg & GPIO_INT_STATUS) {
		/* Clear interrupt status, preserve wake status */
		reg &= ~GPIO_WAKE_STATUS;
		write32((void *)gpio_address, reg);
		return 1;
	}

	return 0;
}
