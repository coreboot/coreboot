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

# define MEM_CONFIG0		GPIO_139
# define MEM_CONFIG1		GPIO_142
# define MEM_CONFIG2		GPIO_131
# define MEM_CONFIG3		GPIO_132

/* CR50 interrupt pin */
#define H1_PCH_INT		GPIO_9

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_122
#define GPIO_EC_IN_RW		GPIO_15

/* PCIe reset pins */
#define PCIE_0_RST		GPIO_70
#define PCIE_1_RST		0
#define PCIE_2_RST		GPIO_40
#define PCIE_3_RST		0

#endif /* _ACPI__ */

#define EC_SCI_GPI		22

#define EC_SMI_GPI		6

#endif /* __BASEBOARD_GPIO_H__ */
