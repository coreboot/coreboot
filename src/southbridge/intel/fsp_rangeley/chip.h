/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef SOUTHBRIDGE_INTEL_RANGELEY_CHIP_H
#define SOUTHBRIDGE_INTEL_RANGELEY_CHIP_H

#include <arch/acpi.h>

struct southbridge_intel_fsp_rangeley_config {

	/**
	 * GPI Routing configuration
	 *
	 * Only the lower two bits have a meaning:
	 * 00: No effect
	 * 01: SMI# (if corresponding ALT_GPI_SMI_EN bit is also set)
	 * 10: SCI (if corresponding GPIO_EN bit is also set)
	 * 11: reserved
	 */
	uint8_t gpi0_routing;
	uint8_t gpi1_routing;
	uint8_t gpi2_routing;
	uint8_t gpi3_routing;
	uint8_t gpi4_routing;
	uint8_t gpi5_routing;
	uint8_t gpi6_routing;
	uint8_t gpi7_routing;
	uint8_t gpi8_routing;
	uint8_t gpi9_routing;
	uint8_t gpi10_routing;
	uint8_t gpi11_routing;
	uint8_t gpi12_routing;
	uint8_t gpi13_routing;
	uint8_t gpi14_routing;
	uint8_t gpi15_routing;

	uint32_t gpe0_en;
	uint16_t alt_gp_smi_en;

	/* IDE configuration */
	uint32_t ide_legacy_combined;
	uint32_t sata_ahci;
	uint8_t sata_port_map;
	uint32_t sata_port0_gen3_tx;
	uint32_t sata_port1_gen3_tx;

	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable linear PCIe Root Port function numbers starting at zero */
	uint8_t pcie_port_coalesce;

	/* Override PCIe ASPM */
	uint8_t pcie_aspm_f0;
	uint8_t pcie_aspm_f1;
	uint8_t pcie_aspm_f2;
	uint8_t pcie_aspm_f3;
	uint8_t pcie_aspm_f4;
	uint8_t pcie_aspm_f5;
	uint8_t pcie_aspm_f6;
	uint8_t pcie_aspm_f7;

	/* ACPI configuration */
	uint8_t  fadt_pm_profile;
	uint16_t fadt_boot_arch;

};

#endif				/* SOUTHBRIDGE_INTEL_RANGELEY_CHIP_H */
