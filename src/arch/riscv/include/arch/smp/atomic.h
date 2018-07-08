/*
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * Copyright (c) 2018, HardenedLinux.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Regents nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 * SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#ifndef _RISCV_ATOMIC_H
#define _RISCV_ATOMIC_H

#include <arch/encoding.h>

typedef struct { volatile int counter; } atomic_t;

#define disable_irqsave() clear_csr(mstatus, MSTATUS_MIE)
#define enable_irqrestore(flags) set_csr(mstatus, (flags) & MSTATUS_MIE)

#define atomic_set(v, val) ((v)->counter = (val))
#define atomic_read(v)     ((v)->counter)

#ifdef __riscv_atomic
# define atomic_add(v, inc)       __sync_fetch_and_add(&((v)->counter), inc)
# define atomic_swap(v, swp)      __sync_lock_test_and_set(&((v)->counter), swp)
# define atomic_cas(v, cmp, swp)  __sync_val_compare_and_swap(&((v)->counter), \
					cmp, swp)
# define atomic_inc(v)            atomic_add(v, 1)
# define atomic_dec(v)            atomic_add(v, -1)
#else
static inline int atomic_add(atomic_t *v, int inc)
{
	long flags = disable_irqsave();
	int res = v->counter;
	v->counter += inc;
	enable_irqrestore(flags);
	return res;
}

static inline int atomic_swap(atomic_t *v, int swp)
{
	long flags = disable_irqsave();
	int res = v->counter;
	v->counter = swp;
	enable_irqrestore(flags);
	return res;
}

static inline int atomic_cas(atomic_t *v, int cmp, int swp)
{
	long flags = disable_irqsave();
	int res = v->counter;
	v->counter = (res == cmp ? swp : res);
	enable_irqrestore(flags);
	return res;
}

static inline int atomic_inc(atomic_t *v)
{
	return atomic_add(v, 1);
}

static inline int atomic_dec(atomic_t *v)
{
	return atomic_add(v, -1);
}
#endif //__riscv_atomic

#endif //_RISCV_ATOMIC_H
