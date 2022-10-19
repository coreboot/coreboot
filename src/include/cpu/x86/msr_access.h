/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_MSR_ACCESS_H
#define CPU_X86_MSR_ACCESS_H

#ifndef __ASSEMBLER__
#include <types.h>

typedef union msr_union {
	struct {
		unsigned int lo;
		unsigned int hi;
	};
	uint64_t raw;
} msr_t;
_Static_assert(sizeof(msr_t) == sizeof(uint64_t), "Incorrect size for msr_t");

#if CONFIG(SOC_SETS_MSRS)
msr_t soc_msr_read(unsigned int index);
void soc_msr_write(unsigned int index, msr_t msr);

/* Handle MSR references in the other source code */
static __always_inline msr_t rdmsr(unsigned int index)
{
	return soc_msr_read(index);
}

static __always_inline void wrmsr(unsigned int index, msr_t msr)
{
	soc_msr_write(index, msr);
}
#else /* CONFIG_SOC_SETS_MSRS */

/* The following functions require the __always_inline due to AMD
 * function STOP_CAR_AND_CPU that disables cache as
 * RAM, the cache as RAM stack can no longer be used. Called
 * functions must be inlined to avoid stack usage. Also, the
 * compiler must keep local variables register based and not
 * allocated them from the stack. With gcc 4.5.0, some functions
 * declared as inline are not being inlined. This patch forces
 * these functions to always be inlined by adding the qualifier
 * __always_inline to their declaration.
 */
static __always_inline msr_t rdmsr(unsigned int index)
{
	msr_t result;
	__asm__ __volatile__ (
		"rdmsr"
		: "=a" (result.lo), "=d" (result.hi)
		: "c" (index)
		);
	return result;
}

static __always_inline void wrmsr(unsigned int index, msr_t msr)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi)
		);
}

#endif /* CONFIG_SOC_SETS_MSRS */
#endif /* __ASSEMBLER__ */
#endif /* CPU_X86_MSR_ACCESS_H */
