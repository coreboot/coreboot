/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_BOOTBLOCK_H_
#define SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_BOOTBLOCK_H_

#include <stdint.h>

void bootblock_mainboard_early_init(void);
void bootblock_soc_early_init(void);
void bootblock_soc_init(void);
void bootblock_mainboard_init(void);

void bootblock_main(const uint64_t reg_x0,
		    const uint64_t reg_pc);

#endif /* SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_BOOTBLOCK_H_ */
