/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_BD82X6X_CHIP_H
#define SOUTHBRIDGE_INTEL_BD82X6X_CHIP_H

#include <southbridge/intel/common/spi.h>
#include <types.h>
#include <southbridge/intel/bd82x6x/pch.h>

struct southbridge_intel_bd82x6x_config {
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
	uint8_t sata_port_map;
	uint32_t sata_port0_gen3_tx;
	uint32_t sata_port1_gen3_tx;

	/**
	 * SATA Interface Speed Support Configuration
	 *
	 * Only the lower two bits have a meaning:
	 * 00 - No effect (leave as chip default)
	 * 01 - 1.5 Gb/s maximum speed
	 * 10 - 3.0 Gb/s maximum speed
	 * 11 - 6.0 Gb/s maximum speed
	 */
	uint8_t sata_interface_speed_support;

	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable linear PCIe Root Port function numbers starting at zero */
	bool pcie_port_coalesce;

	/* Override PCIe ASPM */
	uint8_t pcie_aspm[8];

	bool docking_supported;

	bool pcie_hotplug_map[8];

	/* Ports which can be routed to either EHCI or xHCI.  */
	uint32_t xhci_switchable_ports;
	/* Ports which support SuperSpeed (USB 3.0 additional lanes).  */
	uint32_t superspeed_capable_ports;
	/* Overcurrent Mapping for USB 3.0 Ports */
	uint32_t xhci_overcurrent_mapping;

	uint32_t spi_uvscc;
	uint32_t spi_lvscc;
	struct intel_swseq_spi_config spi;
	struct southbridge_usb_port usb_port_config[14];
};

#endif				/* SOUTHBRIDGE_INTEL_BD82X6X_CHIP_H */
