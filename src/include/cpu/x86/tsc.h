#ifndef CPU_X86_TSC_H
#define CPU_X86_TSC_H

struct tsc_struct {
	unsigned lo;
	unsigned hi;
};
typedef struct tsc_struct tsc_t;

static inline tsc_t rdtsc(void)
{
	tsc_t res;
	asm volatile (
#if CONFIG_TSC_SYNC_MFENCE
		"mfence\n"
#endif
#if CONFIG_TSC_SYNC_LFENCE
		"lfence\n"
#endif
		"rdtsc"
		: "=a" (res.lo), "=d"(res.hi) /* outputs */
	);
	return res;
}

#if !defined(__ROMCC__)
/* Too many registers for ROMCC */
static inline unsigned long long rdtscll(void)
{
	unsigned long long val;
	asm volatile (
#if CONFIG_TSC_SYNC_MFENCE
		"mfence\n"
#endif
#if CONFIG_TSC_SYNC_LFENCE
		"lfence\n"
#endif
		"rdtsc"
		: "=A" (val)
	);
	return val;
}
#endif

#endif /* CPU_X86_TSC_H */
