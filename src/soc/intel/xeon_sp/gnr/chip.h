/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <intelblocks/cfg.h>
#include <soc/acpi.h>
#include <gpio.h>
#include <soc/irq.h>
#include <stdint.h>

struct soc_intel_xeon_sp_gnr_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	bool vtd_support;
	uint8_t debug_print_level;
	uint16_t serial_io_uart_debug_io_base;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	uint32_t tcc_offset;
	enum acpi_cstate_mode cstate_states;
};

typedef struct soc_intel_xeon_sp_gnr_config config_t;

#endif
