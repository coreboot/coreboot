/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#define asmlinkage

static inline unsigned int smp_processor_id(void) { return 0; }

struct cpu_driver { };

#endif /* __ARCH_CPU_H__ */
