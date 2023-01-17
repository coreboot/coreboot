/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <gpio.h>
#include <intelblocks/cfg.h>
#include <soc/acpi.h>
#include <soc/irq.h>
#include <stdint.h>

#define MAX_PCH_PCIE_PORT   20

/**
 UPD_PCH_PCIE_PORT:
 ForceEnable           -  Enable/Disable PCH PCIe port
 PortLinkSpeed         -  Port Link Speed. Use PCIE_LINK_SPEED to set
**/
struct pch_pcie_port {
	uint8_t    ForceEnable;
	uint8_t    PortLinkSpeed;
};

/**
 PCIe Link Speed Selection
 **/
typedef enum {
	PcieAuto = 0,
	PcieGen1,
	PcieGen2,
	PcieGen3
} pcie_link_speed;

/**
 enum for DDR Frequency Limit
 **/
enum ddr_freq_limit {
	DDR_AUTO = 0x0,
	DDR_1333 = 0x5,
	DDR_1600 = 0x7,
	DDR_1866 = 0x9,
	DDR_2133 = 0xb,
	DDR_2400 = 0xd,
	DDR_2666 = 0xf,
	DDR_2933 = 0x11,
	DDR_3200 = 0x13
};

struct soc_intel_xeon_sp_cpx_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Struct for configuring PCH PCIe port */
	struct pch_pcie_port pch_pci_port[MAX_PCH_PCIE_PORT];

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
	 * Device Interrupt Routing configuration
	 * Interrupt Pin x Route.
	 * 0h = PIRQA#
	 * 1h = PIRQB#
	 * 2h = PIRQC#
	 * 3h = PIRQD#
	 * 4h = PIRQE#
	 * 5h = PIRQF#
	 * 6h = PIRQG#
	 * 7h = PIRQH#
	 */
	uint16_t ir00_routing;
	uint16_t ir01_routing;
	uint16_t ir02_routing;
	uint16_t ir03_routing;
	uint16_t ir04_routing;

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
	uint8_t x2apic;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* TCC activation offset */
	uint32_t tcc_offset;

	enum acpi_cstate_mode cstate_states;
};

typedef struct soc_intel_xeon_sp_cpx_config config_t;

#endif
