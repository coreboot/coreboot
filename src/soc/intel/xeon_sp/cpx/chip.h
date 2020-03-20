/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <stdint.h>
#include <intelblocks/cfg.h>

struct soc_intel_xeon_sp_cpx_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;
};

extern struct chip_operations soc_intel_xeon_sp_cpx_ops;

typedef struct soc_intel_xeon_sp_cpx_config config_t;

#endif
