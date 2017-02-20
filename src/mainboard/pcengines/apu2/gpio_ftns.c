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

void configure_gpio(u32 iomux_gpio, u8 iomux_ftn, u32 gpio, u32 setting)
{
	u32 bdata;
	u32 *memptr;
	u8  *iomuxptr;

	memptr = (u32 *)(ACPI_MMIO_BASE + GPIO_OFFSET + gpio);
	bdata = *memptr;

	/* out the data value to prevent glitches */
	bdata |= (setting & GPIO_OUTPUT_ENABLE);
	*memptr = bdata;

	/* set direction and data value */
	bdata |= (setting & (GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE | GPIO_PULL_UP_ENABLE | GPIO_PULL_DOWN_ENABLE));
	*memptr = bdata;

	iomuxptr = (u8 *)(ACPI_MMIO_BASE + IOMUX_OFFSET + iomux_gpio);
	*iomuxptr = iomux_ftn & 0x3;
}

u8 read_gpio(u32 gpio)
{
	u32 *memptr = (u32 *)(ACPI_MMIO_BASE + GPIO_OFFSET + gpio);
	return (*memptr & GPIO_PIN_STS) ? 1 : 0;
}

void write_gpio(u32 gpio, u8 value)
{
	u32 *memptr = (u32 *)(ACPI_MMIO_BASE + GPIO_OFFSET + gpio);
	*memptr |= (value > 0) ? GPIO_OUTPUT_VALUE : 0;
}

int get_spd_offset(void)
{
	u8 index = 0;
	/* One SPD file contains all 4 options, determine which
	 * index to
	 * read here, then call into the standard
	 * routines.
	 */

	index = read_gpio(GPIO_49);
	index |= read_gpio(GPIO_50) << 1;

	return index;
}
