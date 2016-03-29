/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#ifndef __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_I2C_H
#define __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_I2C_H

void i2c_init(unsigned int bus, unsigned int hz);
void software_i2c_attach(unsigned bus);
void software_i2c_detach(unsigned bus);

#endif  /* ! __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_I2C_H */
