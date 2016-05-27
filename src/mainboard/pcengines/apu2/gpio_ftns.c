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
#include "gpio_ftns.h"

void configure_gpio(uintptr_t base_addr, u32 iomux_gpio, u8 iomux_ftn, u32 gpio, u32 setting)
{
	u8 bdata;
	u8 *memptr;

	memptr = (u8 *)(base_addr + IOMUX_OFFSET + iomux_gpio);
	*memptr = iomux_ftn;

	memptr = (u8 *)(base_addr + GPIO_OFFSET + gpio);
	bdata = *memptr;
	bdata &= 0x07;
	bdata |= setting; /* set direction and data value */
	*memptr = bdata;
}
