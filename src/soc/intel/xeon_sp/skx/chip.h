/*
 * This file is part of the coreboot project.
 *
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


#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <stdint.h>
#include <intelblocks/cfg.h>
#include <soc/irq.h>

struct soc_intel_xeon_sp_skx_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

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

	uint32_t vtd_support;
	uint32_t coherency_support;
	uint32_t ats_support;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;
};

extern struct chip_operations soc_intel_xeon_sp_ops;

typedef struct soc_intel_xeon_sp_skx_config config_t;

#endif
