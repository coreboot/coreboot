/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include "smbios.h"
#include <string.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/dram/ddr3.h>
#include <dimm_info_util.h>

#define EXTENSION_BUS_WIDTH_8BITS	8

/* Fill the SMBIOS memory information from FSP MEM_INFO_DATA_HOB in CBMEM.*/
void dimm_info_fill(struct dimm_info *dimm, u32 dimm_capacity, u8 ddr_type,
		u32 frequency, u8 rank_per_dimm, u8 channel_id, u8 dimm_id,
		const char *module_part_num, size_t module_part_number_size,
		const u8 *module_serial_num, u16 data_width, u32 vdd_voltage,
		bool ecc_support, u16 mod_id, u8 mod_type)
{
	dimm->mod_id = mod_id;
	/* Translate to DDR2 module type field that SMBIOS code expects. */
	switch (mod_type) {
	case SPD_DDR3_DIMM_TYPE_SO_DIMM:
		dimm->mod_type = SPD_SODIMM;
		break;
	case SPD_DDR3_DIMM_TYPE_72B_SO_CDIMM:
		dimm->mod_type = SPD_72B_SO_CDIMM;
		break;
	case SPD_DDR3_DIMM_TYPE_72B_SO_RDIMM:
		dimm->mod_type = SPD_72B_SO_RDIMM;
		break;
	case SPD_DDR3_DIMM_TYPE_UDIMM:
		dimm->mod_type = SPD_UDIMM;
		break;
	case SPD_DDR3_DIMM_TYPE_RDIMM:
		dimm->mod_type = SPD_RDIMM;
		break;
	case SPD_DDR3_DIMM_TYPE_UNDEFINED:
	default:
		dimm->mod_type = SPD_UNDEFINED;
		break;
	}
	dimm->dimm_size = dimm_capacity;
	dimm->ddr_type = ddr_type;
	dimm->ddr_frequency = frequency;
	dimm->rank_per_dimm = rank_per_dimm;
	dimm->channel_num = channel_id;
	dimm->dimm_num = dimm_id;
	if (vdd_voltage > 0xFFFF) {
		dimm->vdd_voltage = 0xFFFF;
	} else {
		dimm->vdd_voltage = vdd_voltage;
	}

	strncpy((char *)dimm->module_part_number,
			module_part_num,
			MIN(sizeof(dimm->module_part_number),
				module_part_number_size));
	if (module_serial_num)
		memcpy(dimm->serial, module_serial_num,
		       DIMM_INFO_SERIAL_SIZE);

	uint16_t total_width = data_width;

	if (ecc_support)
		total_width += EXTENSION_BUS_WIDTH_8BITS;

	dimm->bus_width = smbios_bus_width_to_spd_width(ddr_type, total_width, data_width);
}
