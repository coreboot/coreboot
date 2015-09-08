/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
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
 * Foundation, Inc.
 */

#ifndef _INTEL_COMMON_ACPI_H_
#define _INTEL_COMMON_ACPI_H_

#include <stdint.h>

/*
 * SOC specific handler to provide the wake source data for ACPI _SWS.
 *
 * @pm1:  PM1_STS register with only enabled events set
 * @gpe0: GPE0_STS registers with only enabled events set
 *
 * return the number of registers in the gpe0 array or -1 if nothing
 * is provided by this function.
 */
int soc_fill_acpi_wake(uint32_t *pm1, uint32_t **gpe0);

#endif
