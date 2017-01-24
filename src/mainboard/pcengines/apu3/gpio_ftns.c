/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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
#include <southbridge/amd/cimx/cimx_util.h>
#include "FchPlatform.h"
#include "gpio_ftns.h"

void configure_gpio(uintptr_t base_addr, u32 iomux_gpio, u8 iomux_ftn, u32 setting)
{
	u8 bdata;
	u8 *memptr;
	/* we are interested in bits 16:23 of GPIO configuration space */
	memptr = (u8 *)(base_addr + GPIO_OFFSET + (iomux_gpio << 2) + 2);
	bdata = *memptr;
	/* out the data value to prevent glitches */
	bdata |= (setting & GPIO_OUTPUT_ENABLE);
	*memptr = bdata;

	/* set direction and data value */
	bdata |= (setting & (GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE | GPIO_PULL_UP_ENABLE | GPIO_PULL_DOWN_ENABLE));
	*memptr = bdata;

	memptr = (u8 *)(base_addr + IOMUX_OFFSET + iomux_gpio);
	*memptr = iomux_ftn;

}

u8 read_gpio(uintptr_t base_addr, u32 iomux_gpio)
{
	u8 *memptr = (u8 *)(base_addr + GPIO_OFFSET + (iomux_gpio << 2) + 2);
	return (*memptr & GPIO_OUTPUT_VALUE) ? 1 : 0;
}

void write_gpio(uintptr_t base_addr, u32 iomux_gpio, u8 value)
{
	u8 *memptr = (u8 *)(base_addr + GPIO_OFFSET + (iomux_gpio << 2) + 2);

	*memptr |= (value > 0) ? GPIO_OUTPUT_VALUE : 0;
}
