/*
 * This file is part of the coreboot project.
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

#ifndef _CIMX_SB_GPIO_OEM_H_
#define _CIMX_SB_GPIO_OEM_H_

#define SB_GPIO_REG02   2
#define SB_GPIO_REG09   9
#define SB_GPIO_REG10   10
#define SB_GPIO_REG15   15
#define SB_GPIO_REG17   17
#define SB_GPIO_REG21   21
#define SB_GPIO_REG25   25
#define SB_GPIO_REG28   28

/* FCH GPIO access helpers */
#define FCH_IOMUX(gpio_nr) (*(u8*)((uintptr_t)ACPI_MMIO_BASE+IOMUX_BASE+(gpio_nr)))
#define FCH_PMIO(reg_nr) (*(u8*)((uintptr_t)ACPI_MMIO_BASE+PMIO_BASE+(reg_nr)))
#define FCH_GPIO(gpio_nr) (*(volatile u8*)((uintptr_t)ACPI_MMIO_BASE+GPIO_BASE+(gpio_nr)))

static inline u8 fch_gpio_state(unsigned int gpio_nr)
{
	return FCH_GPIO(gpio_nr) >> 7;
}

#endif
