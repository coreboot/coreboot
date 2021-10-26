/* SPDX-License-Identifier: GPL-2.0-only */

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
uint8_t smbios_bus_width_to_spd_width(uint8_t ddr_type, uint16_t total_width,
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
uint8_t smbios_form_factor_to_spd_mod_type(smbios_memory_type memory_type,
		smbios_memory_form_factor form_factor);

#endif
