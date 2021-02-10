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

#define DMIBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))
#define DMIBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))
#define DMIBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))

#define EPBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))
#define EPBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))
#define EPBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))

#endif	/* ! NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H */
