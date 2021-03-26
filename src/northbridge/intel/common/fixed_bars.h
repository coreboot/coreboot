/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H
#define NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H

_Static_assert(CONFIG_FIXED_MCHBAR_MMIO_BASE != 0, "MCHBAR base address is zero");
_Static_assert(CONFIG_FIXED_DMIBAR_MMIO_BASE != 0, "DMIBAR base address is zero");
_Static_assert(CONFIG_FIXED_EPBAR_MMIO_BASE  != 0,  "EPBAR base address is zero");

#include <stdint.h>

#define MCHBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_MCHBAR_MMIO_BASE + (x))))
#define MCHBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_MCHBAR_MMIO_BASE + (x))))
#define MCHBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_MCHBAR_MMIO_BASE + (x))))

#define MCHBAR8_AND(x,  and) (MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and) (MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,  or) (MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x, or) (MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x, or) (MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or) (MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or) (MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

#define DMIBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))
#define DMIBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))
#define DMIBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))

#define EPBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))
#define EPBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))
#define EPBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))

#endif	/* ! NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H */
