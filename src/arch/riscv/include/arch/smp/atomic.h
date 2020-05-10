/* SPDX-License-Identifier: BSD-4-Clause-UC */

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
