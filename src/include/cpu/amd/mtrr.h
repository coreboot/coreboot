/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_AMD_MTRR_H
#define CPU_AMD_MTRR_H

#define MTRR_IORR0_BASE	0xC0010016
#define MTRR_IORR0_MASK	0xC0010017
#define MTRR_IORR1_BASE	0xC0010018
#define MTRR_IORR1_MASK	0xC0010019

#define MTRR_READ_MEM			(1 << 4)
#define MTRR_WRITE_MEM			(1 << 3)

#define SYSCFG_MSR			0xC0010010
#define SYSCFG_MSR_TOM2WB		(1 << 22)
#define SYSCFG_MSR_TOM2En		(1 << 21)
#define SYSCFG_MSR_MtrrVarDramEn	(1 << 20)
#define SYSCFG_MSR_MtrrFixDramModEn	(1 << 19)
#define SYSCFG_MSR_MtrrFixDramEn	(1 << 18)
#define SYSCFG_MSR_UcLockEn		(1 << 17)
#define SYSCFG_MSR_ChxToDirtyDis	(1 << 16)
#define SYSCFG_MSR_ClVicBlkEn		(1 << 11)
#define SYSCFG_MSR_SetDirtyEnO		(1 << 10)
#define SYSCFG_MSR_SetDirtyEnS		(1 <<  9)
#define SYSCFG_MSR_SetDirtyEnE		(1 <<  8)
#define SYSCFG_MSR_SysVicLimitMask	((1 << 8) - (1 << 5))
#define SYSCFG_MSR_SysAckLimitMask	((1 << 5) - (1 << 0))

#define IORRBase_MSR(reg) (0xC0010016 + 2 * (reg))
#define IORRMask_MSR(reg) (0xC0010016 + 2 * (reg) + 1)

#if defined(__ASSEMBLER__)
#define TOP_MEM		0xC001001A
#define TOP_MEM2	0xC001001D
#else
#define TOP_MEM		0xC001001Aul
#define TOP_MEM2	0xC001001Dul
#endif

#if !defined(__ASSEMBLER__)

#include <cpu/x86/msr.h>
#include <stdint.h>

struct device;
void add_uma_resource_below_tolm(struct device *nb, int idx);

static __always_inline msr_t rdmsr_amd(unsigned int index)
{
	msr_t result;
	__asm__ __volatile__ (
		"rdmsr"
		: "=a" (result.lo), "=d" (result.hi)
		: "c"(index), "D"(0x9c5a203a)
		);
	return result;
}

static __always_inline void wrmsr_amd(unsigned int index, msr_t msr)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi), "D" (0x9c5a203a)
		);
}

static inline uint64_t amd_topmem(void)
{
	return rdmsr(TOP_MEM).lo;
}

static inline uint64_t amd_topmem2(void)
{
	msr_t msr = rdmsr(TOP_MEM2);
	return (uint64_t)msr.hi << 32 | msr.lo;
}
#endif

#endif /* CPU_AMD_MTRR_H */
