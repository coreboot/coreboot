/*
 * This file is part of the depthcharge project.
 *
 * Copyright (C) 2018-2019, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __I2C_QCOM_HEADER___
#define __I2C_QCOM_HEADER___

#include <device/i2c.h>

void i2c_init(unsigned int bus, enum i2c_speed speed);

#endif /* __I2C_QCOM_HEADER */
