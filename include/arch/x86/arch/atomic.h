/* this implementation is from Linux. */
#ifndef ARCH_ATOMIC_H
#define ARCH_ATOMIC_H

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
struct atomic { volatile int counter; };

#define ATOMIC_INIT(i)	{ (i) }

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

/**
 * atomic_read - read atomic variable
 * @v: pointer of type struct atomic
 * 
 * Atomically reads the value of @v.  Note that the guaranteed
 * useful range of an struct atomic is only 24 bits.
 */ 
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @v: pointer of type struct atomic
 * @i: required value
 * 
 * Atomically sets the value of @v to @i.  Note that the guaranteed
 * useful range of an struct atomic is only 24 bits.
 */ 
#define atomic_set(v,i)		(((v)->counter) = (i))

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type struct atomic
 * 
 * Atomically increments @v by 1.  Note that the guaranteed
 * useful range of an struct atomic is only 24 bits.
 */ 
static __inline__ __attribute__((always_inline)) void atomic_inc(struct atomic *v)
{
	__asm__ __volatile__(
		"lock ; incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type struct atomic
 * 
 * Atomically decrements @v by 1.  Note that the guaranteed
 * useful range of an struct atomic is only 24 bits.
 */ 
static __inline__ __attribute__((always_inline)) void atomic_dec(struct atomic *v)
{
	__asm__ __volatile__(
		"lock ; decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}



#endif /* ARCH_ATOMIC_H */
