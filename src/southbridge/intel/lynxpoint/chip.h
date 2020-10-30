/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_LYNXPOINT_CHIP_H
#define SOUTHBRIDGE_INTEL_LYNXPOINT_CHIP_H

#include <stdint.h>

struct southbridge_intel_lynxpoint_config {
	/**
	 * GPI Routing configuration for LynxPoint-H
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

	uint32_t gpe0_en_1;
	uint32_t gpe0_en_2;
	uint32_t gpe0_en_3;
	uint32_t gpe0_en_4;
	uint32_t alt_gp_smi_en;

	/* SATA configuration */
	uint8_t sata_port_map;
	uint32_t sata_port0_gen3_tx;
	uint32_t sata_port1_gen3_tx;
	uint32_t sata_port0_gen3_dtle;
	uint32_t sata_port1_gen3_dtle;

	/*
	 * SATA DEVSLP Mux
	 * 0 = port 0 DEVSLP on DEVSLP0/GPIO33
	 * 1 = port 3 DEVSLP on DEVSLP0/GPIO33
	 */
	uint8_t sata_devslp_mux;

	/*
	 * DEVSLP Disable
	 * 0: DEVSLP is enabled
	 * 1: DEVSLP is disabled
	 */
	uint8_t sata_devslp_disable;

	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable linear PCIe Root Port function numbers starting at zero */
	uint8_t pcie_port_coalesce;

	/* Force root port ASPM configuration with port bitmap */
	uint8_t pcie_port_force_aspm;

	/* Put SerialIO devices into ACPI mode instead of a PCI device */
	uint8_t sio_acpi_mode;

	/* I2C voltage select: 0=3.3V 1=1.8V */
	uint8_t sio_i2c0_voltage;
	uint8_t sio_i2c1_voltage;

	/*
	 * Clock Disable Map:
	 * [21:16] = CLKOUT_PCIE# 5-0
	 *    [24] = CLKOUT_ITPXDP
	 */
	uint32_t icc_clock_disable;

	/* Route USB ports to XHCI per default */
	uint8_t xhci_default;

	/* Information for the ACPI FADT. */
	bool docking_supported;
};

#endif	/* SOUTHBRIDGE_INTEL_LYNXPOINT_CHIP_H */
