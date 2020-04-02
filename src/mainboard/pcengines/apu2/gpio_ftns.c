/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/mmio.h>
#include <FchPlatform.h>
#include "gpio_ftns.h"

static u32 gpio_read_wrapper(u32 gpio)
{
	if (gpio < 0x100)
		return gpio0_read32(gpio & 0xff);
	else if (gpio >= 0x100 && gpio < 0x200)
		return gpio1_read32(gpio & 0xff);
	else if (gpio >= 0x200 && gpio < 0x300)
		return gpio2_read32(gpio & 0xff);

	die("Invalid GPIO");
}

static void gpio_write_wrapper(u32 gpio, u32 setting)
{
	if (gpio < 0x100)
		gpio0_write32(gpio & 0xff, setting);
	else if (gpio >= 0x100 && gpio < 0x200)
		gpio1_write32(gpio & 0xff, setting);
	else if (gpio >= 0x200 && gpio < 0x300)
		gpio2_write32(gpio & 0xff, setting);
}

void configure_gpio(u8 iomux_gpio, u8 iomux_ftn, u32 gpio, u32 setting)
{
	u32 bdata;

	bdata =  gpio_read_wrapper(gpio);
	/* out the data value to prevent glitches */
	bdata |= (setting & GPIO_OUTPUT_ENABLE);
	gpio_write_wrapper(gpio, bdata);

	/* set direction and data value */
	bdata |= (setting & (GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE
			| GPIO_PULL_UP_ENABLE | GPIO_PULL_DOWN_ENABLE));
	gpio_write_wrapper(gpio, bdata);

	iomux_write8(iomux_gpio, iomux_ftn & 0x3);
}

u8 read_gpio(u32 gpio)
{
	return (gpio_read_wrapper(gpio) & GPIO_PIN_STS) ? 1 : 0;
}

void write_gpio(u32 gpio, u8 value)
{
	u32 status = gpio_read_wrapper(gpio);
	status &= ~GPIO_OUTPUT_VALUE;
	status |= (value > 0) ? GPIO_OUTPUT_VALUE : 0;
	gpio_write_wrapper(gpio, status);
}

int get_spd_offset(void)
{
	u8 index = 0;
	/*
	 * One SPD file contains all 4 options, determine which index to
	 * read here, then call into the standard routines.
	 */
	if (gpio1_read8(0x02) & BIT0)
		index |= BIT0;
	if (gpio1_read8(0x06) & BIT0)
		index |= BIT1;

	return index;
}
