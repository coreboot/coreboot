/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google Inc.
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

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <gpio.h>

#define LTE3_PWROFF_L	GPP_D0

void variant_smi_sleep(u8 slp_typ)
{
	if (slp_typ == ACPI_S5) {
		/* Turn off LTE module */
		gpio_set(LTE3_PWROFF_L, 0);
	}
}
