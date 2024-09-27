/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_CHIP_H_
#define _SOC_SNOWRIDGE_CHIP_H_

#include <fsp/soc_binding.h>
#include <intelblocks/cfg.h>
#include <stdint.h>

/**
 * @brief Total number of domains. SNR needs two additional domains to handle
 * additional root bus in stack 2 (Intel Dynamic Load Balancer) and 7 (UBox1).
 */
#define MAX_DOMAIN (BL_MAX_SOCKET * BL_MAX_LOGIC_IIO_STACK + 2)

struct snr_domain {
	uint8_t enabled;
	uint8_t personality;
	uint8_t bus_base;
	uint8_t bus_limit;
	uint16_t io_base;
	uint16_t io_limit;
	uint32_t mem32_base;
	uint32_t mem32_limit;
	uint64_t mem64_base;
	uint64_t mem64_limit;
	struct device *dev;
};

struct soc_intel_snowridge_config {
	struct soc_intel_common_config common_soc_config;

	uint32_t tcc_offset; /**< Needed by `common/block/cpulib.c`. */
	uint8_t eist_enable;

	struct snr_domain domain[MAX_DOMAIN];
};

typedef struct soc_intel_snowridge_config config_t;

#endif // _SOC_SNOWRIDGE_CHIP_H_
