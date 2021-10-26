/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DEVICE_DRAM_SPD_H
#define DEVICE_DRAM_SPD_H

#include <smbios.h>
#include <types.h>

const char *spd_manufacturer_name(const uint16_t mod_id);

struct spd_info {
	uint16_t type_detail;
	uint8_t form_factor;
};

void get_spd_info(smbios_memory_type memory_type, uint8_t module_type, struct spd_info *info);
uint8_t convert_form_factor_to_module_type(smbios_memory_type memory_type,
		smbios_memory_form_factor form_factor);

#endif /* DEVICE_DRAM_SPD_H */
