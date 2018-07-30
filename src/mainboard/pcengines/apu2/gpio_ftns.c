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

	bdata = read32((const volatile void *)(ACPI_MMIO_BASE + GPIO_OFFSET
		+ gpio));
	/* out the data value to prevent glitches */
	bdata |= (setting & GPIO_OUTPUT_ENABLE);
	write32((volatile void *)(ACPI_MMIO_BASE + GPIO_OFFSET + gpio), bdata);

	/* set direction and data value */
	bdata |= (setting & (GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE
			| GPIO_PULL_UP_ENABLE | GPIO_PULL_DOWN_ENABLE));
	write32((volatile void *)(ACPI_MMIO_BASE + GPIO_OFFSET + gpio), bdata);

	write8((volatile void *)(ACPI_MMIO_BASE + IOMUX_OFFSET + iomux_gpio),
			iomux_ftn & 0x3);
}

u8 read_gpio(u32 gpio)
{
	u32 status = read32((const volatile void *)(ACPI_MMIO_BASE + GPIO_OFFSET
			+ gpio));

	return (status & GPIO_PIN_STS) ? 1 : 0;
}

void write_gpio(u32 gpio, u8 value)
{
	u32 status = read32((const volatile void *)(ACPI_MMIO_BASE + GPIO_OFFSET
			+ gpio));
	status &= ~GPIO_OUTPUT_VALUE;
	status |= (value > 0) ? GPIO_OUTPUT_VALUE : 0;
	write32((volatile void *)(ACPI_MMIO_BASE + GPIO_OFFSET + gpio), status);
}

int get_spd_offset(void)
{
	u8 index = 0;
	/* One SPD file contains all 4 options, determine which index to
	 * read here, then call into the standard routines.
	 */
	u8 *gpio_bank0_ptr = (u8 *)(ACPI_MMIO_BASE + GPIO_BANK0_BASE);
	if (*(gpio_bank0_ptr + (0x40 << 2) + 2) & BIT0) index |= BIT0;
	if (*(gpio_bank0_ptr + (0x41 << 2) + 2) & BIT0) index |= BIT1;

	return index;
}
