/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#ifndef SOC_INTEL_COMMON_LPSS_I2C_H
#define SOC_INTEL_COMMON_LPSS_I2C_H

#include <device/i2c.h>
#include <stdint.h>

/*
 * Return the base address for this bus controller.
 *
 * This function *must* be implemented by the SOC and return the appropriate
 * base address for the I2C registers that correspond to the provided bus.
 */
uintptr_t lpss_i2c_base_address(unsigned bus);

/*
 * Initialize this bus controller and set the speed.
 *
 * The bus speed can be passed in Hz or using values from device/i2c.h and
 * will default to I2C_SPEED_FAST if it is not provided.
 *
 * The SOC *must* define CONFIG_SOC_INTEL_COMMON_LPSS_I2C_CLOCK for the
 * bus speed calculation to be correct.
 */
void lpss_i2c_init(unsigned bus, enum i2c_speed speed);

#endif
