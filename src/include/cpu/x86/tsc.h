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
/* Simple 32- to 64-bit multiplication. Uses 16-bit words to avoid overflow.
 * This code is used to prevent use of libgcc's umoddi3.
 */
static inline void multiply_to_tsc(tsc_t *const tsc, const u32 a, const u32 b)
{
	tsc->lo = (a & 0xffff) * (b & 0xffff);
	tsc->hi = ((tsc->lo >> 16)
		+ ((a & 0xffff) * (b >> 16))
		+ ((b & 0xffff) * (a >> 16)));
	tsc->lo = ((tsc->hi & 0xffff) << 16) | (tsc->lo & 0xffff);
	tsc->hi = ((a >> 16) * (b >> 16)) + (tsc->hi >> 16);
}

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
