#ifndef CPU_X86_MSR_H
#define CPU_X86_MSR_H


#ifdef __ROMCC__

typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
	return __builtin_rdmsr(index);
}

static void wrmsr(unsigned long index, msr_t msr)
{
	__builtin_wrmsr(index, msr.lo, msr.hi);
}

#endif /* __ROMCC__ */

#if defined(__GNUC__) && !defined(__ROMCC__)

typedef struct msr_struct 
{
	unsigned lo;
	unsigned hi;
} msr_t;

static inline msr_t rdmsr(unsigned index)
{
	msr_t result;
	__asm__ __volatile__ (
		"rdmsr"
		: "=a" (result.lo), "=d" (result.hi)
		: "c" (index)
		);
	return result;
}

static inline void wrmsr(unsigned index, msr_t msr)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi)
		);
}

#endif /* __GNUC__ */


#endif /* CPU_X86_MSR_H */
