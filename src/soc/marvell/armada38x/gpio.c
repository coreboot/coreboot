/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Marvell Inc.
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
#include <soc/common.h>
#include <console/console.h>

#define MV_GPIO_MAX_NUM 59

#define MV_GPP_IN 0xFFFFFFFF /* GPP input */
#define MV_GPP_OUT 0 /* GPP output */

#define MV_GPP_REGS_BASE(unit) (0x18100 + ((unit)*0x40))
#define GPP_DATA_OUT_EN_REG(grp) (MV_GPP_REGS_BASE(grp) + 0x04)
#define GPP_DATA_IN_REG(grp) (MV_GPP_REGS_BASE(grp) + 0x10)
#define GPP_DATA_OUT_REG(grp) (MV_GPP_REGS_BASE(grp) + 0x00)

static void gpp_reg_set(u32 group, u32 reg_offs, u32 mask, u32 value);
static int mv_gpp_type_set(u32 group, u32 mask, u32 value);
static u32 mv_gpp_value_get(u32 group, u32 mask);
static int mv_gpp_value_set(u32 group, u32 mask, u32 value);

void gpp_reg_set(u32 group, u32 reg_offs, u32 mask, u32 value)
{
	u32 gpp_data;

	gpp_data = mrvl_reg_read(reg_offs);
	gpp_data &= ~mask;
	gpp_data |= (value & mask);
	mrvl_reg_write(reg_offs, gpp_data);
}

int mv_gpp_type_set(u32 group, u32 mask, u32 value)
{
	gpp_reg_set(group, GPP_DATA_OUT_EN_REG(group), mask, value);
	return MV_OK;
}

u32 mv_gpp_value_get(u32 group, u32 mask)
{
	u32 gpp_data;

	gpp_data = mrvl_reg_read(GPP_DATA_IN_REG(group));
	gpp_data &= mask;
	return gpp_data;
}

int mv_gpp_value_set(u32 group, u32 mask, u32 value)
{
	u32 out_enable;

	/* verify that the gpp pin is configured as output*/
	/* Note that in the register out enabled -> bit = '0'.*/
	out_enable = ~(mrvl_reg_read(GPP_DATA_OUT_EN_REG(group)));
	if ((out_enable & mask) != mask) {
		printk(BIOS_ERR,
			"Mask and out_enable mismatch(mask:0x%x, out_enable:0x%x).\n",
			mask, (out_enable & mask));
		return MV_ERROR;
	}
	gpp_reg_set(group, GPP_DATA_OUT_REG(group), mask, value);
	return MV_OK;
}

static inline int gpio_not_valid(gpio_t gpio)
{
	return (gpio > MV_GPIO_MAX_NUM);
}

int gpio_get(gpio_t gpio)
{
	u32 group = 0;
	u32 gpp_data;

	if (gpio_not_valid(gpio))
		return MV_BAD_PARAM;
	if (gpio >= 32) {
		group = 1;
		gpio -= 32;
	}

	mv_gpp_type_set(group, (1 << gpio), MV_GPP_IN & (1 << gpio));
	gpp_data = mv_gpp_value_get(group, (1 << gpio));
	return (gpp_data != 0);
}

void gpio_set(gpio_t gpio, int value)
{
	u32 group = 0;

	if (gpio_not_valid(gpio))
		return;
	if (gpio >= 32) {
		group = 1;
		gpio -= 32;
	}

	mv_gpp_type_set(group, (1 << gpio), MV_GPP_OUT & (1 << gpio));
	mv_gpp_value_set(group, (1 << gpio), (value ? (1 << gpio) : 0));
}
