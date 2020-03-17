/*
 * This file is part of the coreboot project.
 *
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

#ifndef SOUTHBRIDGE_INTEL_I82801JX_CHIP_H
#define SOUTHBRIDGE_INTEL_I82801JX_CHIP_H

#include <stdint.h>

enum {
	THTL_DEF = 0, THTL_87_5 = 1, THTL_75_0 = 2, THTL_62_5 = 3,
	THTL_50_0 = 4, THTL_37_5 = 5, THTL_25_0 = 6, THTL_12_5 = 7
};

struct southbridge_intel_i82801jx_config {
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
	uint8_t sata_port_map : 6;
	int sata_clock_request : 1;
	int sata_traffic_monitor : 1;

	int c4onc3_enable:1;
	int c5_enable : 1;
	int c6_enable : 1;
	int c3_latency;
	int p_cnt_throttling_supported:1;
	int docking_supported:1;

	int throttle_duty : 3;

	/* Bit mask to tell whether a PCIe slot is implemented as slot. */
	int pcie_slot_implemented : 6;

	/* Power limits for PCIe ports. Values are in 10^(-scale) watts. */
	struct {
		uint8_t value : 8;
		uint8_t scale : 2;
	} pcie_power_limits[6];

	uint8_t pcie_hotplug_map[8];

	/* Additional LPC IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;
};

#endif				/* SOUTHBRIDGE_INTEL_I82801JX_CHIP_H */
