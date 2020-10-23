/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_BROADWELL_PCH_CHIP_H_
#define _SOC_INTEL_BROADWELL_PCH_CHIP_H_

#include <stdint.h>

struct soc_intel_broadwell_pch_config {
	/* GPE configuration */
	uint32_t gpe0_en_1;
	uint32_t gpe0_en_2;
	uint32_t gpe0_en_3;
	uint32_t gpe0_en_4;

	/* GPIO SMI configuration */
	uint32_t alt_gp_smi_en;

	/* IDE configuration */
	uint8_t sata_port_map;
	uint32_t sata_port0_gen3_tx;
	uint32_t sata_port1_gen3_tx;
	uint32_t sata_port2_gen3_tx;
	uint32_t sata_port3_gen3_tx;
	uint32_t sata_port0_gen3_dtle;
	uint32_t sata_port1_gen3_dtle;
	uint32_t sata_port2_gen3_dtle;
	uint32_t sata_port3_gen3_dtle;

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

	/* Generic IO decode ranges */
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

	/* Enable ADSP power gating features */
	uint8_t adsp_d3_pg_enable;
	uint8_t adsp_sram_pg_enable;

	/*
	 * Clock Disable Map:
	 * [21:16] = CLKOUT_PCIE# 5-0
	 *    [24] = CLKOUT_ITPXDP
	 */
	uint32_t icc_clock_disable;

	/* Deep SX enable */
	int deep_sx_enable_ac;
	int deep_sx_enable_dc;
};

#endif
