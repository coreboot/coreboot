/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio_legacy_gpio100.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include "gpio_ftns.h"

void configure_gpio(u8 gpio, u8 iomux_ftn, u8 setting)
{
	u8 bdata;

	iomux_write8(gpio, iomux_ftn);

	bdata = gpio_100_read8(gpio);
	bdata &= 0x07;
	bdata |= setting; /* set direction and data value */
	gpio_100_write8(gpio, bdata);
}

u8 read_gpio(u8 gpio)
{
	return (gpio_100_read8(gpio) & GPIO_DATA_IN) ? 1 : 0;
}

int get_spd_offset(void)
{
	u8 spd_offset = read_gpio(GPIO_16);
	return spd_offset;
}
