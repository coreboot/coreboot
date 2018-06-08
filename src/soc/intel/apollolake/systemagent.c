/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cpu/cpu.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/util.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/systemagent.h>

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ PCIEXBAR, CONFIG_MMCONF_BASE_ADDRESS, CONFIG_SA_PCIEX_LENGTH,
				"PCIEXBAR" },
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
	};

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));
}

int soc_get_uncore_prmmr_base_and_mask(uint64_t *prmrr_base,
			uint64_t *prmrr_mask)
{
	const void *hob;
	size_t hob_size, prmrr_size;
	uint64_t phys_address_mask;
	const uint8_t prmrr_phys_base_guid[16] = {
		0x38, 0x3a, 0x81, 0x9f, 0xb0, 0x6f, 0xa7, 0x4f,
		0xaf, 0x79, 0x8a, 0x4e, 0x74, 0xdd, 0x48, 0x33
	};
	const uint8_t prmrr_size_guid[16] = {
		0x44, 0xed, 0x0b, 0x99, 0x4e, 0x9b, 0x26, 0x42,
		0xa5, 0x97, 0x28, 0x36, 0x76, 0x6b, 0x5c, 0x41
	};

	hob = fsp_find_extension_hob_by_guid(prmrr_phys_base_guid,
			&hob_size);
	if (!hob) {
		printk(BIOS_ERR, "Failed to locate PRMRR base hob\n");
		return -1;
	}
	if (hob_size != sizeof(uint64_t)) {
		printk(BIOS_ERR, "Incorrect PRMRR base hob size\n");
		return -1;
	}
	*prmrr_base = *(uint64_t *) hob;

	hob = fsp_find_extension_hob_by_guid(prmrr_size_guid,
			&hob_size);
	if (!hob) {
		printk(BIOS_ERR, "Failed to locate PRMRR size hob\n");
		return -1;
	}
	if (hob_size != sizeof(uint64_t)) {
		printk(BIOS_ERR, "Incorrect PRMRR base hob size\n");
		return -1;
	}
	prmrr_size = *(uint64_t *) hob;
	phys_address_mask = (1ULL << cpu_phys_address_size()) - 1;
	*prmrr_mask = phys_address_mask & ~(uint64_t)(prmrr_size - 1);

	return 0;
}
