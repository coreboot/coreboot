/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_I82801GX_CHIP_H
#define SOUTHBRIDGE_INTEL_I82801GX_CHIP_H

#include <types.h>

enum sata_mode {
	SATA_MODE_AHCI = 0,
	SATA_MODE_IDE_LEGACY_COMBINED,
	SATA_MODE_IDE_PLAIN,
};

struct southbridge_intel_i82801gx_config {
	/**
	 * Interrupt Routing configuration
	 * If bit7 is 1, the interrupt is disabled.
	 */
	uint8_t pirqa_routing;
	uint8_t pirqb_routing;
	uint8_t pirqc_routing;
	uint8_t pirqd_routing;
	uint8_t pirqe_routing;
	uint8_t pirqf_routing;
	uint8_t pirqg_routing;
	uint8_t pirqh_routing;

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
	bool ide_enable_primary;
	bool ide_enable_secondary;
	enum sata_mode sata_mode;
	uint32_t sata_ports_implemented;

	/* Enable linear PCIe Root Port function numbers starting at zero */
	bool pcie_port_coalesce;

	int c4onc3_enable:1;
	int docking_supported:1;
	int p_cnt_throttling_supported:1;
	int c3_latency;

	/* Additional LPC IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;
};

#endif				/* SOUTHBRIDGE_INTEL_I82801GX_CHIP_H */
