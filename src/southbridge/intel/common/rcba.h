/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H
#define SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H

#define DEFAULT_RCBA		((u8 *)CONFIG_FIXED_RCBA_MMIO_BASE)

/* Root Complex Register Block */
#define RCBA 0xf0
#define RCBA_ENABLE 0x01

#define RCBA8(x)  (*((volatile u8  *)(DEFAULT_RCBA + (x))))
#define RCBA16(x) (*((volatile u16 *)(DEFAULT_RCBA + (x))))
#define RCBA32(x) (*((volatile u32 *)(DEFAULT_RCBA + (x))))
#define RCBA64(x) (*((volatile u64 *)(DEFAULT_RCBA + (x))))

#define RCBA_AND_OR(bits, x, and, or) \
	(RCBA##bits(x) = ((RCBA##bits(x) & (and)) | (or)))
#define RCBA8_AND_OR(x, and, or)  RCBA_AND_OR(8, x, and, or)
#define RCBA16_AND_OR(x, and, or) RCBA_AND_OR(16, x, and, or)
#define RCBA32_AND_OR(x, and, or) RCBA_AND_OR(32, x, and, or)
#define RCBA32_OR(x, or) RCBA_AND_OR(32, x, ~0UL, or)

#endif /* SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H */
