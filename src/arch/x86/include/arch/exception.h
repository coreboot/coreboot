/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <arch/cpu.h>

#if CONFIG(IDT_IN_EVERY_STAGE) || ENV_RAMSTAGE
asmlinkage void exception_init(void);
#else
static inline void exception_init(void) { /* not implemented */ }
#endif

#endif
