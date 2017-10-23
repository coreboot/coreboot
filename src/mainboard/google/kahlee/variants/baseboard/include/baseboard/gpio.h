/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
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

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#ifndef __ACPI__
#include <soc/gpio.h>

# define MEM_CONFIG0		GPIO_0
# define MEM_CONFIG1		GPIO_0
# define MEM_CONFIG2		GPIO_0
# define MEM_CONFIG3		GPIO_0

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_0
#define GPIO_EC_IN_RW		GPIO_0

#endif /* _ACPI__ */

#define EC_SCI_GPI		0

#define EC_SMI_GPI		0

#endif /* __BASEBOARD_GPIO_H__ */
