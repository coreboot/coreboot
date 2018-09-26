/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <dimm_info_util.h>
#include <smbios.h>
#include <spd.h>
#include <console/console.h>

uint8_t smbios_bus_width_to_spd_width(uint16_t total_width, uint16_t data_width)
{
	uint8_t out;

	/* Lookup table as defined in the JEDEC Standard. */
	switch (data_width) {
	case 64:
		out = MEMORY_BUS_WIDTH_64;
		break;
	case 32:
		out = MEMORY_BUS_WIDTH_32;
		break;
	case 16:
		out = MEMORY_BUS_WIDTH_16;
		break;
	case 8:
		out = MEMORY_BUS_WIDTH_8;
		break;
	default:
		printk(BIOS_NOTICE, "Unknown memory size %hu", data_width);
		/*
		 * The SMBIOS spec says we should set 0xFFFF on an unknown
		 * value, but we don't have a way of passing that signal via SPD
		 * encoded values.
		 */
		out = MEMORY_BUS_WIDTH_8;
		break;
	}

	uint16_t extension_bits = total_width - data_width;

	switch (extension_bits) {
	case 8:
		out |= SPD_ECC_8BIT;
		break;
	case 0:
		/* No extension bits */
		break;
	default:
		printk(BIOS_NOTICE, "Unknown number of extension bits %hu",
		       extension_bits);
		break;
	}

	return out;
}

uint32_t smbios_memory_size_to_mib(uint16_t memory_size, uint32_t extended_size)
{
	/* Memory size is unknown */
	if (memory_size == 0xFFFF)
		return 0;
	/* (32 GiB - 1 MiB) or greater is expressed in the extended size. */
	else if (memory_size == 0x7FFF)
		return extended_size;
	/* When the MSB is flipped, the value is specified in kilobytes */
	else if (memory_size & 0x8000)
		return (memory_size ^ 0x8000) / KiB;
	/* Value contains MiB */
	else
		return memory_size;
}

uint8_t
smbios_form_factor_to_spd_mod_type(smbios_memory_form_factor form_factor)
{
	/* This switch reverses the switch in smbios.c */
	switch (form_factor) {
	case MEMORY_FORMFACTOR_DIMM:
		return SPD_UDIMM;
	case MEMORY_FORMFACTOR_RIMM:
		return SPD_RDIMM;
	case MEMORY_FORMFACTOR_SODIMM:
		return SPD_SODIMM;
	default:
		return SPD_UNDEFINED;
	}
}
