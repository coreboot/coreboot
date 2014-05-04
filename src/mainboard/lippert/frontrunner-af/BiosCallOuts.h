/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#ifndef _BIOS_CALLOUT_H_
#define _BIOS_CALLOUT_H_

#include <northbridge/amd/agesa/family14/fam14_callouts.h>
#include "SB800.h"

/* CALLOUT Initialization */
AGESA_STATUS GetBiosCallout(UINT32 Func, UINT32 Data, VOID *ConfigPtr);

/* FCH GPIO access helpers */
#define FCH_IOMUX(gpio_nr) (*(u8*)(ACPI_MMIO_BASE+IOMUX_BASE+(gpio_nr)))
#define FCH_GPIO(gpio_nr) (*(volatile u8*)(ACPI_MMIO_BASE+GPIO_BASE+(gpio_nr)))
static inline u8 fch_gpio_state(unsigned int gpio_nr)
{
	return FCH_GPIO(gpio_nr) >> 7;
}

#endif //_BIOS_CALLOUT_H_
