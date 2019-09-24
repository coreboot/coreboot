/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014-2019 Siemens AG
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

#ifndef __SOC_INTEL_FSP_BAYTRAIL_I2C_H__
#define __SOC_INTEL_FSP_BAYTRAIL_I2C_H__

#define I2C_SOFTWARE_RESET	0x804
#define  I2C_RESET_APB		(1 << 1)
#define  I2C_RESET_FUNC		(1 << 0)

#endif	/* __SOC_INTEL_FSP_BAYTRAIL_I2C_H__ */
