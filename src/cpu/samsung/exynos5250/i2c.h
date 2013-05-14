/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CPU_SAMSUNG_EXYNOS5250_I2C_H
#define CPU_SAMSUNG_EXYNOS5250_I2C_H

#include "periph.h"

struct s3c24x0_i2c {
	u32	iiccon;
	u32	iicstat;
	u32	iicadd;
	u32	iicds;
	u32	iiclc;
};

struct s3c24x0_i2c_bus {
	int bus_num;
	struct s3c24x0_i2c *regs;
	enum periph_id periph_id;
};

void i2c_init(unsigned bus, int speed, int slaveadd);

#endif /* CPU_SAMSUNG_EXYNOS5250_I2C_H */
