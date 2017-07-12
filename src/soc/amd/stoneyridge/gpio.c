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
#include <gpio.h>
#include <soc/gpio.h>

/* The following functions must be implemented by SoC/board code. */

int gpio_get(gpio_t gpio_num)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);

	return !!(reg & GPIO_PIN_STS);
}


void gpio_set(gpio_t gpio_num, int value)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);
	reg &= ~GPIO_OUTPUT_MASK;
	reg |=  !!value << GPIO_OUTPUT_SHIFT;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input_pulldown(gpio_t gpio_num)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);
	reg &= ~GPIO_PULLUP_ENABLE;
	reg |=  GPIO_PULLDOWN_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input_pullup(gpio_t gpio_num)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);
	reg &= ~GPIO_PULLDOWN_ENABLE;
	reg |=  GPIO_PULLUP_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input(gpio_t gpio_num)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);
	reg &= ~GPIO_OUTPUT_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_output(gpio_t gpio_num, int value)
{
	uint32_t reg;

	reg = read32((void *)(uintptr_t)gpio_num);
	reg |=  GPIO_OUTPUT_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}
