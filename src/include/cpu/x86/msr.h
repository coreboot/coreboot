#ifndef CPU_X86_MSR_H
#define CPU_X86_MSR_H

#include <compiler.h>

/* Intel SDM: Table 2-1
 * IA-32 architectural MSR: Extended Feature Enable Register
 */
#define IA32_EFER	0xC0000080
#define  EFER_NXE	(1 << 11)
#define  EFER_LMA	(1 << 10)
#define  EFER_LME	(1 << 8)
#define  EFER_SCE	(1 << 0)

/* Page attribute type MSR */
#define MSR_IA32_PAT			0x277
#define MSR_IA32_MPERF			0xe7
#define MSR_IA32_APERF			0xe8
#define MSR_IA32_PM_ENABLE		0x770
#define MSR_IA32_HWP_CAPABILITIES	0x771
#define MSR_IA32_HWP_REQUEST		0x774
#define MSR_IA32_HWP_STATUS		0x777

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

typedef struct msr_struct {
	unsigned int lo;
	unsigned int hi;
} msr_t;

typedef struct msrinit_struct {
	unsigned int index;
	msr_t msr;
} msrinit_t;

#if IS_ENABLED(CONFIG_SOC_SETS_MSRS)
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
#endif /* __ROMCC__ */

#endif /* CPU_X86_MSR_H */
