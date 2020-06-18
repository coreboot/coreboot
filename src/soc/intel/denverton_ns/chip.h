/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_DENVERTON_NS_CHIP_H
#define SOC_INTEL_DENVERTON_NS_CHIP_H

#include <stdint.h>

struct soc_intel_denverton_ns_config {
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
	uint16_t ir05_routing;
	uint16_t ir06_routing;
	uint16_t ir07_routing;
	uint16_t ir08_routing;
	uint16_t ir09_routing;
	uint16_t ir10_routing;
	uint16_t ir11_routing;
	uint16_t ir12_routing;

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

	/* TCC activation offset */
	uint32_t tcc_offset;
};

typedef struct soc_intel_denverton_ns_config config_t;

#endif /* SOC_INTEL_FSP_DENVERTON_NS_CHIP_H */
