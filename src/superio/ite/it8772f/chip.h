/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 */

#ifndef SUPERIO_ITE_IT8772F_CHIP_H
#define SUPERIO_ITE_IT8772F_CHIP_H

#include <device/device.h>
#include "it8772f.h"

struct superio_ite_it8772f_config {
	/* GPIO SimpleIO register values via devicetree.cb */
	u8 gpio_set1;
	u8 gpio_set2;
	u8 gpio_set3;
	u8 gpio_set4;
	u8 gpio_set5;
	u8 gpio_set6;

	u8 skip_keyboard;

	/*
	 * Enable external temperature sensor to use PECI GetTemp()
	 * command and store in register TMPIN 1, 2, or 3.
	 */
	u8 peci_tmpin;

	/*
	 * Enable thermal mode on tmpinx.
	 */
	enum thermal_mode tmpin1_mode;
	enum thermal_mode tmpin2_mode;

	/*
	 * Enable a FAN for sofware control.
	 */
	u8 fan1_enable;
	u8 fan2_enable;
	u8 fan3_enable;

	/*
	 * Default FAN speed
	 */
	u8 fan2_speed;
	u8 fan3_speed;
};

#endif /* SUPERIO_ITE_IT8772F_CHIP_H */
