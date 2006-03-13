#ifndef CPU_X86_TSC_H
#define CPU_X86_TSC_H

struct tsc_struct {
	unsigned lo;
	unsigned hi;
};
typedef struct tsc_struct tsc_t;

static tsc_t rdtsc(void)
{
	tsc_t res;
	__asm__ __volatile__ (
		"rdtsc"
		: "=a" (res.lo), "=d"(res.hi) /* outputs */
		);
	return res;
}

#ifndef __ROMCC__
static inline unsigned long long rdtscll(void)
{
	unsigned long long val;
	asm volatile ("rdtsc" : "=A" (val));
	return val;
}
#endif


#endif /* CPU_X86_TSC_H */
