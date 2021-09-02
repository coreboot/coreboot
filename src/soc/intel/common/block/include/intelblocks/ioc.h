/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_IOC_H
#define SOC_INTEL_COMMON_BLOCK_IOC_H

#include <types.h>

void ioc_reg_write32(uint32_t offset, uint32_t value);
uint32_t ioc_reg_read32(uint32_t offset);
void ioc_reg_or32(uint32_t offset, uint32_t ordata);

#endif	/* SOC_INTEL_COMMON_BLOCK_IOC_H */
