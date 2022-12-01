/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ADAINIT_H
#define _ADAINIT_H

/**
 * @file adainit.h
 *
 * Ada supports some complex constructs that result in code for runtime
 * initialization. It's also possible to have explicit procedures for
 * package level initialization (e.g. you can initialize huge arrays in
 * a loop instead of cluttering the binary).
 *
 * When an Ada main() is in charge, GNAT emits the call to the initia-
 * lizations automatically. When not, we have to call it explicitly.
 */

#if CONFIG(ROMSTAGE_ADA)
void romstage_adainit(void);
#else
static inline void romstage_adainit(void) {}
#endif

#if CONFIG(RAMSTAGE_ADA)
void ramstage_adainit(void);
#else
static inline void ramstage_adainit(void) {}
#endif

#endif /* _ADAINIT_H */
