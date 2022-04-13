/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_GPMR_H
#define SOC_INTEL_COMMON_BLOCK_GPMR_H

#include <types.h>
#include <intelblocks/pcr_gpmr.h>

uint32_t gpmr_read32(uint16_t offset);
void gpmr_write32(uint16_t offset, uint32_t val);
void gpmr_or32(uint16_t offset, uint32_t ordata);
enum cb_err enable_gpmr(uint32_t base, uint32_t size, uint32_t dest_id);

#endif /* SOC_INTEL_COMMON_BLOCK_GPMR_H */
