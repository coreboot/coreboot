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

/*
 * Kahlee doesn't use MEM_CONFIG GPIOs, but they are required to build
 * the baseboard weak memory_sku function.
 */
#define MEM_CONFIG0	0
#define MEM_CONFIG1	0
#define MEM_CONFIG2	0
#define MEM_CONFIG3	0

/* CDX03 doesn't have a CR50 interrupt pin */
#define H1_PCH_INT		0

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_142
#define GPIO_EC_IN_RW		GPIO_15

/* PCIe reset pins */
#define PCIE_0_RST		GPIO_119
#define PCIE_1_RST		0
#define PCIE_2_RST		0
#define PCIE_3_RST		0

#endif /* _ACPI__ */

/* AGPIO22 -> GPE3 */
#define EC_SCI_GPI		3

/* TODO: Fix this */
/* GPIO_S5_07 is EC_SMI#, but it is bit 23 in GPE_STS and ALT_GPIO_SMI. */
#define EC_SMI_GPI		23

#endif /* __VARIANT_GPIO_H__ */
