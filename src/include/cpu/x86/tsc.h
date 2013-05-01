#ifndef CPU_X86_TSC_H
#define CPU_X86_TSC_H

#if CONFIG_TSC_SYNC_MFENCE
#define TSC_SYNC "mfence\n"
#elif CONFIG_TSC_SYNC_LFENCE
#define TSC_SYNC "lfence\n"
#else
#define TSC_SYNC
#endif

struct tsc_struct {
	unsigned lo;
	unsigned hi;
};
typedef struct tsc_struct tsc_t;

static inline tsc_t rdtsc(void)
{
	tsc_t res;
	asm volatile (
		TSC_SYNC
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
		TSC_SYNC
		"rdtsc"
		: "=A" (val)
	);
	return val;
}
#endif

#if CONFIG_TSC_CONSTANT_RATE
unsigned long tsc_freq_mhz(void);
#endif

#endif /* CPU_X86_TSC_H */
