/*
 * This file is part of the coreboot project.
 *
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
#ifndef _SOC_CANNONLAKE_GPIO_COMMON_H_
#define _SOC_CANNONLAKE_GPIO_COMMON_H_

#define GPIORXSTATE_MASK			0x1
#define GPIORXSTATE_SHIFT			1
#define GPIOTXSTATE_MASK			0x1
#define GPIOPADMODE_MASK			0xC00
#define GPIOPADMODE_SHIFT			10
#define GPIOTXBUFDIS_MASK			0x100
#define GPIORXBUFDIS_MASK			0x200

#endif
