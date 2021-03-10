/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_SMN_H
#define AMD_BLOCK_SMN_H

#include <types.h>

uint32_t smn_read32(uint32_t reg);
void smn_write32(uint32_t reg, uint32_t val);

#endif /* AMD_BLOCK_SMN_H */
