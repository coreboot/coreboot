/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

/* TODO: Implement using SEV/WFE if this is ever actually used. */
static inline void cpu_relax(void) { }

#define asmlinkage

struct cpu_driver { };

#endif /* __ARCH_CPU_H__ */
