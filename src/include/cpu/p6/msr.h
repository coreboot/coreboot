#ifndef CPU_P6_MSR_H
#define CPU_P6_MSR_H


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


struct tsc_struct {
	unsigned lo;
	unsigned hi;
};
typedef struct tsc_struct tsc_t;

static tsc_t rdtsc(void)
{
	tsc_t res;
	asm ("rdtsc"
		: "=a" (res.lo), "=d"(res.hi) /* outputs */
		: /* inputs */
		: /* Clobbers */
		);
	return res;
}
#endif

#ifdef __GNUC__

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

typedef struct tsc_struct 
{
	unsigned lo;
	unsigned hi;
} tsc_t;

static inline tsc_t rdtsc(void)
{
	tsc_t result;
	__asm__ __volatile__(
		"rdtsc"
		: "=a"  (result.lo), "=d" (result.hi)
		);
	return result;
}

typedef struct pmc_struct 
{
	unsigned lo;
	unsigned hi;
} pmc_t; 

static inline pmc_t rdpmc(unsigned counter)
{
	pmc_t result;
	__asm__ __volatile__(
		"rdpmc"
		: "=a" (result.lo), "=d" (result.hi)
		: "c" (counter)
		);
	return result;
}

#endif

#endif /* CPU_P6_MSR_H */
