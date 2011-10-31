#ifndef CPU_X86_MSR_H
#define CPU_X86_MSR_H

#if defined(__ROMCC__)

typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
	return __builtin_rdmsr(index);
}

static void wrmsr(unsigned long index, msr_t msr)
{
	__builtin_wrmsr(index, msr.lo, msr.hi);
}

#else

typedef struct msr_struct
{
	unsigned lo;
	unsigned hi;
} msr_t;

typedef struct msrinit_struct
{
        unsigned index;
        msr_t msr;
} msrinit_t;

/* The following functions require the always_inline due to AMD
 * function STOP_CAR_AND_CPU that disables cache as
 * ram, the cache as ram stack can no longer be used. Called
 * functions must be inlined to avoid stack usage. Also, the
 * compiler must keep local variables register based and not
 * allocated them from the stack. With gcc 4.5.0, some functions
 * declared as inline are not being inlined. This patch forces
 * these functions to always be inlined by adding the qualifier
 * __attribute__((always_inline)) to their declaration.
 */
static inline __attribute__((always_inline)) msr_t rdmsr(unsigned index)
{
	msr_t result;
	__asm__ __volatile__ (
		"rdmsr"
		: "=a" (result.lo), "=d" (result.hi)
		: "c" (index)
		);
	return result;
}

static inline __attribute__((always_inline)) void wrmsr(unsigned index, msr_t msr)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi)
		);
}

#endif /* __ROMCC__ */

#endif /* CPU_X86_MSR_H */
