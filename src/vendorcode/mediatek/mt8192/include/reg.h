/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef REG_H
#define REG_H

#define REG32(addr) ((volatile uint32_t *)(uintptr_t)(addr))

#define read32(addr)       (*REG32(addr))
#define write32(addr, val) (*REG32(addr) = (val))

#endif
