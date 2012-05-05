#ifndef SMP_ATOMIC_H
#define SMP_ATOMIC_H

#if CONFIG_SMP
#include <arch/smp/atomic.h>
#else

typedef struct { int counter; } atomic_t;
#define ATOMIC_INIT(i) { (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_set(v,i)		(((v)->counter) = (i))


/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_inc(v)	(((v)->counter)++)


/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define atomic_dec(v)	(((v)->counter)--)


#endif /* CONFIG_SMP */

#endif /* SMP_ATOMIC_H */
