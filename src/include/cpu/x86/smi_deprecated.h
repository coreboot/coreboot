/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __X86_SMI_DEPRECATED_H__
#define __X86_SMI_DEPRECATED_H__

#include <stdint.h>

#if CONFIG(PARALLEL_MP) || !CONFIG(HAVE_SMI_HANDLER)
/* Empty stubs for platforms without SMI handlers. */
static inline void smm_init(void) { }
static inline void smm_init_completion(void) { }
#else
void smm_init(void);
void smm_init_completion(void);
#endif

/* Entry from smmhandler.S. */
void smi_handler(void);

#endif
