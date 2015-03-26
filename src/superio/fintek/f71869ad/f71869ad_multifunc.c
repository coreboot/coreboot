/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include "chip.h"
#include "fintek_internal.h"

#define MULTI_FUNC_SEL_REG1 0x28
#define MULTI_FUNC_SEL_REG2 0x29
#define MULTI_FUNC_SEL_REG3 0x2A
#define MULTI_FUNC_SEL_REG4 0x2B
#define MULTI_FUNC_SEL_REG5 0x2C

void f71869ad_multifunc_init(struct device *dev)
{
	const struct superio_fintek_f71869ad_config *conf = dev->chip_info;

	pnp_enter_conf_mode(dev);

	/* multi-func select reg1 */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG1,
			conf->multi_function_register_1);

	/* multi-func select reg2 (CLK_TUNE_EN=0) */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG2,
			conf->multi_function_register_2);

	/* multi-func select reg3 (CLK_TUNE_EN=0) */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG3,
			conf->multi_function_register_3);

	/* multi-func select reg4 (CLK_TUNE_EN=0) */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG4,
			conf->multi_function_register_4);

	/* multi-func select reg5 (CLK_TUNE_EN=0) */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG5,
			conf->multi_function_register_5);

	pnp_exit_conf_mode(dev);
}
