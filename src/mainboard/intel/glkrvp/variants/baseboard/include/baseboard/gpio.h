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

#ifndef BASEBOARD_GPIO_H
#define BASEBOARD_GPIO_H

#include <soc/gpio.h>

/*
 * GPIO_11 for SCI is routed to GPE0_DW1 and maps to group GPIO_GPE_N_31_0
 * which is North community
 */
#if CONFIG(SOC_ESPI)
#define EC_SCI_GPI	GPE0A_ESPI_SCI_STS
#else
#define EC_SCI_GPI	GPE0_DW1_05
#endif

/* EC SMI */
#define EC_SMI_GPI	GPIO_41

#define GPE_EC_WAKE	GPE0_DW1_06

/*  Memory SKU GPIOs. */
#define MEM_CONFIG3	GPIO_45
#define MEM_CONFIG2	GPIO_38
#define MEM_CONFIG1	GPIO_102
#define MEM_CONFIG0	GPIO_101

#endif /* BASEBOARD_GPIO_H */
