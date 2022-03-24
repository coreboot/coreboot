/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __ARCH_BARRIER_H__
#define __ARCH_BARRIER_H__

static inline void mb(void)  { asm volatile("fence"); }
static inline void rmb(void) { asm volatile("fence"); }
static inline void wmb(void) { asm volatile("fence"); }

#define barrier() { asm volatile ("fence" ::: "memory"); }

#endif /* __ARCH_BARRIER_H__ */
