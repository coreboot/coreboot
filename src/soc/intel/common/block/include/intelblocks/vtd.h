/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_VTD_H
#define SOC_INTEL_COMMON_BLOCK_VTD_H

#include <device/mmio.h>
#include <stdint.h>

/* VT-d specification: https://cdrdv2.intel.com/v1/dl/getContent/671081 */
#define VER_REG		0x0
#define CAP_REG		0x8
#define  CAP_PMR_LO	BIT(5)
#define  CAP_PMR_HI	BIT(6)
#define PMEN_REG	0x64
#define   PMEN_EPM	BIT(31)
#define   PMEN_PRS	BIT(0)
#define PLMBASE_REG	0x68
#define PLMLIMIT_REG	0x6C
#define PHMBASE_REG	0x70
#define PHMLIMIT_REG	0x78

static __always_inline uint32_t vtd_read32(uintptr_t vtd_base, uint32_t reg)
{
	return read32p(vtd_base + reg);
}

static __always_inline void vtd_write32(uintptr_t vtd_base, uint32_t reg, uint32_t value)
{
	return write32p(vtd_base + reg, value);
}

static __always_inline uint64_t vtd_read64(uintptr_t vtd_base, uint32_t reg)
{
	return read64p(vtd_base + reg);
}

static __always_inline void vtd_write64(uintptr_t vtd_base, uint32_t reg, uint64_t value)
{
	return write64p(vtd_base + reg, value);
}

/*
 * Enable DMA protection by setting PMR registers in VT-d for whole DRAM memory.
 */
void vtd_enable_dma_protection(void);
/*
 * Get DMA buffer base and size.
 */
void *vtd_get_dma_buffer(size_t *size);

#endif /* SOC_INTEL_COMMON_BLOCK_VTD_H */
