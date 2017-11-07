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

#ifndef __VARIANT_GPIO_H__
#define __VARIANT_GPIO_H__

#ifndef __ACPI__
#include <soc/gpio.h>

#define MEM_CONFIG0	GPIO_135
#define MEM_CONFIG1	GPIO_140
#define MEM_CONFIG2	GPIO_144
/*
 * Kahlee only uses 3 GPIOs to determine memory configuration, but other
 * variants use 4. MEM_CONFIG3 must be defined so that the weak baseboard
 * version of the variant_board_id() function can compile.
 */
#define MEM_CONFIG3	0

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_142
#define GPIO_EC_IN_RW		GPIO_15

#endif /* _ACPI__ */

/* AGPIO22 -> GPE3 */
#define EC_SCI_GPI		3

/* TODO: Fix this */
/* GPIO_S5_07 is EC_SMI#, but it is bit 23 in GPE_STS and ALT_GPIO_SMI. */
#define EC_SMI_GPI		23

#endif /* __VARIANT_GPIO_H__ */
