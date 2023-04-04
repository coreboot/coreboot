/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <gpio.h>
#include <intelblocks/cfg.h>
#include <soc/acpi.h>
#include <soc/irq.h>
#include <stdint.h>

#define MAX_PCH_PCIE_PORT 20

/**
 UPD_PCH_PCIE_PORT:
 ForceEnable           -  Enable/Disable PCH PCIe port
 PortLinkSpeed         -  Port Link Speed. Use PCIE_LINK_SPEED to set
**/
struct pch_pcie_port {
	uint8_t ForceEnable;
	uint8_t PortLinkSpeed;
};

struct soc_intel_xeon_sp_spr_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Struct for configuring PCH PCIe port */
	struct pch_pcie_port pch_pci_port[MAX_PCH_PCIE_PORT];

	/**
	 * Device Interrupt Polarity Control
	 * ipc0 - IRQ-00-31 - 1: Active low to IOAPIC, 0: Active high to IOAPIC
	 * ipc1 - IRQ-32-63 - 1: Active low to IOAPIC, 0: Active high to IOAPIC
	 * ipc2 - IRQ-64-95 - 1: Active low to IOAPIC, 0: Active high to IOAPIC
	 * ipc3 - IRQ-96-119 - 1: Active low to IOAPIC, 0: Active high to IOAPIC
	 */
	uint32_t ipc0;
	uint32_t ipc1;
	uint32_t ipc2;
	uint32_t ipc3;

	uint64_t turbo_ratio_limit;
	uint64_t turbo_ratio_limit_cores;

	uint32_t pstate_req_ratio;

	uint8_t vtd_support;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* TCC activation offset */
	uint32_t tcc_offset;

	enum acpi_cstate_mode cstate_states;
};

typedef struct soc_intel_xeon_sp_spr_config config_t;

#endif
