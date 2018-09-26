/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _DIMM_INFO_UTIL_H_
#define _DIMM_INFO_UTIL_H_

#include <smbios.h>
#include <stdint.h>

/**
 * Convert the SMBIOS bit widths into an SPD encoded width.
 *
 * Use this when setting dimm_info.bus_width if the raw SPD values are not
 * available.
 */
uint8_t smbios_bus_width_to_spd_width(uint16_t total_width,
				      uint16_t data_width);

/**
 * Convert the SMBIOS size values into the total number of MiB.
 *
 * Use this when setting dimm_info.dimm_size.
 */
uint32_t smbios_memory_size_to_mib(uint16_t memory_size,
				   uint32_t extended_size);

/**
 * Convert the SMBIOS form factor to the SPD module type.
 *
 * Use this when setting dimm_info.mod_type.
 */
uint8_t
smbios_form_factor_to_spd_mod_type(smbios_memory_form_factor form_factor);

#endif
