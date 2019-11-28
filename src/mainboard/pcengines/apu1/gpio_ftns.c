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
#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include "gpio_ftns.h"

uintptr_t find_gpio_base(void)
{
	uintptr_t base_addr;
	/* Get the ACPIMMIO base address */
	base_addr = pm_read32(0x24);
	base_addr &= 0xFFFFF000;
	return base_addr;
}

void configure_gpio(uintptr_t base_addr, u32 gpio, u8 iomux_ftn, u8 setting)
{
	u8 bdata;
	u8 *memptr;

	memptr = (u8 *)(base_addr + IOMUX_OFFSET + gpio);
	*memptr = iomux_ftn;

	memptr = (u8 *)(base_addr + GPIO_OFFSET + gpio);
	bdata = *memptr;
	bdata &= 0x07;
	bdata |= setting; /* set direction and data value */
	*memptr = bdata;
}

u8 read_gpio(uintptr_t base_addr, u32 gpio)
{
	u8 *memptr = (u8 *)(base_addr + GPIO_OFFSET + gpio);
	return (*memptr & GPIO_DATA_IN) ? 1 : 0;
}

int get_spd_offset(void)
{
	uintptr_t base_addr = find_gpio_base();
	u8 spd_offset = read_gpio(base_addr, GPIO_16);
	return spd_offset;
}
