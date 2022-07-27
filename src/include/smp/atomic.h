/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SMP_ATOMIC_H
#define SMP_ATOMIC_H

#if CONFIG(SMP)
#include <arch/smp/atomic.h>
#else

typedef struct { int counter; } atomic_t;
#define ATOMIC_INIT(i) { (i) }

/**
 * @file include/smp/atomic.h
 */

/**
 * atomic_read - read atomic variable
 * @param v: pointer of type atomic_t
 *
 * Atomically reads the value of v.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @param v: pointer of type atomic_t
 * @param i: required value
 *
 * Atomically sets the value of v to i.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_set(v, i)	(((v)->counter) = (i))

/**
 * atomic_inc - increment atomic variable
 * @param v: pointer of type atomic_t
 *
 * Atomically increments v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_inc(v)	(((v)->counter)++)

/**
 * atomic_dec - decrement atomic variable
 * @param v: pointer of type atomic_t
 *
 * Atomically decrements v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_dec(v)	(((v)->counter)--)

#endif /* CONFIG_SMP */

#endif /* SMP_ATOMIC_H */
