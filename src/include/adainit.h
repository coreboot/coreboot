/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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

#if CONFIG(RAMSTAGE_ADA)
void ramstage_adainit(void);
#else
static inline void ramstage_adainit(void) {}
#endif

#endif /* _ADAINIT_H */
