/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8772F_CHIP_H
#define SUPERIO_ITE_IT8772F_CHIP_H

#include <device/device.h>
#include <types.h>

#include "it8772f.h"

struct superio_ite_it8772f_config {
	/* GPIO SimpleIO register values via devicetree.cb */
	u8 gpio_set1;
	u8 gpio_set2;
	u8 gpio_set3;
	u8 gpio_set4;
	u8 gpio_set5;
	u8 gpio_set6;

	bool skip_keyboard;

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
	 * Enable a FAN for software control.
	 */
	bool fan1_enable;
	bool fan2_enable;
	bool fan3_enable;

	/*
	 * Default FAN speed
	 */
	u8 fan2_speed;
	u8 fan3_speed;
};

#endif /* SUPERIO_ITE_IT8772F_CHIP_H */
