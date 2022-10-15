/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_VTD_H
#define SOC_INTEL_COMMON_BLOCK_VTD_H

#include <stdint.h>

/*
 * Enable DMA protection by setting PMR registers in VT-d for whole DRAM memory.
 */
void vtd_enable_dma_protection(void);
/*
 * Get DMA buffer base and size.
 */
void *vtd_get_dma_buffer(size_t *size);

#endif /* SOC_INTEL_COMMON_BLOCK_VTD_H */
