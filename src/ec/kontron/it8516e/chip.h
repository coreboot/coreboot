/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef EC_KONTRON_IT8516E_CHIP_H
#define EC_KONTRON_IT8516E_CHIP_H

#include "ec.h"

struct ec_kontron_it8516e_config {
	/*
	 * Fan settings (fan1: CPU; fan2: System)
	 * Can be overwritten by
	 *   systemp_type,
	 *   fan1_mode, fan2_mode,
	 *   fan1_target, fan2_target,
	 *   fan1_min, fan2_min, fan1_max, and fan2_max
	 * nvram options.
	 */
	enum it8516e_systemp_types default_systemp;
	enum it8516e_fan_modes default_fan_mode[2];
	u16 default_fan_target[2]; /* PWM: % / Speed: RPM / Thermal: degree C */
	u8 default_fan_min[2]; /* min PWM in % */
	u8 default_fan_max[2]; /* max PWM in % */
};

#endif /* EC_KONTRON_IT8516E_CHIP_H */
