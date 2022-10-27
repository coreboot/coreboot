/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_MSR_H
#define CPU_X86_MSR_H

#include <cpu/x86/msr_access.h> /* IWYU pragma: export */

/* Intel SDM: Table 2-1
 * IA-32 architectural MSR: Extended Feature Enable Register
 *
 * AMD64 Programmers Manual vol2 Revision 3.30 and/or the device's BKDG
 */

#define IA32_EFER	0xC0000080
#define  EFER_NXE	(1 << 11)
#define  EFER_LMA	(1 << 10)
#define  EFER_LME	(1 << 8)
#define  EFER_SCE	(1 << 0)

/* Page attribute type MSR */
#define TSC_MSR				0x10
#define IA32_PLATFORM_ID		0x17
#define IA32_APIC_BASE_MSR_INDEX	0x1B
#define IA32_FEATURE_CONTROL		0x3a
#define  FEATURE_CONTROL_LOCK_BIT	(1 << 0)
#define  FEATURE_ENABLE_VMX		(1 << 2)
#define  SMRR_ENABLE			(1 << 3)
#define  CPUID_VMX			(1 << 5)
#define  CPUID_SMX			(1 << 6)
#define  CPUID_DCA			(1 << 18)
#define  CPUID_X2APIC			(1 << 21)
#define  CPUID_AES			(1 << 25)
#define  SGX_GLOBAL_ENABLE		(1 << 18)
#define  PLATFORM_INFO_SET_TDP		(1 << 29)
#define IA32_BIOS_UPDT_TRIG		0x79
#define IA32_BIOS_SIGN_ID		0x8b
#define IA32_MPERF			0xe7
#define IA32_APERF			0xe8
/* STM */
#define IA32_SMM_MONITOR_CTL_MSR	0x9B
#define SMBASE_RO_MSR			0x98
#define  IA32_SMM_MONITOR_VALID		(1 << 0)
#define IA32_MCG_CAP			0x179
#define  MCG_CTL_P			(1 << 8)
#define  MCA_BANKS_MASK			0xff
#define IA32_PERF_STATUS		0x198
#define IA32_PERF_CTL			0x199
#define IA32_THERM_INTERRUPT		0x19b
#define IA32_MISC_ENABLE		0x1a0
#define  FAST_STRINGS_ENABLE_BIT	(1 << 0)
#define  SPEED_STEP_ENABLE_BIT		(1 << 16)
#define IA32_ENERGY_PERF_BIAS		0x1b0
#define  ENERGY_POLICY_PERFORMANCE	0
#define  ENERGY_POLICY_NORMAL		6
#define  ENERGY_POLICY_POWERSAVE	15
#define  ENERGY_POLICY_MASK		0xf
#define IA32_PACKAGE_THERM_INTERRUPT	0x1b2
#define SMRR_PHYSBASE_MSR		0x1F2
#define SMRR_PHYSMASK_MSR		0x1F3
#define IA32_PLATFORM_DCA_CAP		0x1f8
#define  DCA_TYPE0_EN			(1 << 0)
#define IA32_PAT			0x277
#define IA32_MC0_CTL			0x400
#define IA32_MC_CTL(bank)		(IA32_MC0_CTL + 4 * (bank))
#define IA32_MC0_STATUS			0x401
#define IA32_MC_STATUS(bank)		(IA32_MC0_STATUS + 4 * (bank))
#define  MCA_STATUS_HI_VAL		(1UL << (63 - 32))
#define  MCA_STATUS_HI_OVERFLOW		(1UL << (62 - 32))
#define  MCA_STATUS_HI_UC		(1UL << (61 - 32))
#define  MCA_STATUS_HI_EN		(1UL << (60 - 32))
#define  MCA_STATUS_HI_MISCV		(1UL << (59 - 32))
#define  MCA_STATUS_HI_ADDRV		(1UL << (58 - 32))
#define  MCA_STATUS_HI_PCC		(1UL << (57 - 32))
#define  MCA_STATUS_HI_COREID_VAL	(1UL << (56 - 32))
#define  MCA_STATUS_HI_CECC		(1UL << (46 - 32))
#define  MCA_STATUS_HI_UECC		(1UL << (45 - 32))
#define  MCA_STATUS_HI_DEFERRED		(1UL << (44 - 32))
#define  MCA_STATUS_HI_POISON		(1UL << (43 - 32))
#define  MCA_STATUS_HI_SUBLINK		(1UL << (41 - 32))
#define  MCA_STATUS_HI_ERRCOREID_MASK	(0xf << 0)
#define  MCA_STATUS_LO_ERRCODE_EXT_SH	16
#define  MCA_STATUS_LO_ERRCODE_EXT_MASK	(0x3f << MCA_STATUS_LO_ERRCODE_EXT_SH)
#define  MCA_STATUS_LO_ERRCODE_MASK	(0xffff << 0)
#define IA32_MC0_ADDR			0x402
#define IA32_MC_ADDR(bank)		(IA32_MC0_ADDR + 4 * (bank))
#define IA32_MC0_MISC			0x403
#define IA32_MC_MISC(bank)		(IA32_MC0_MISC + 4 * (bank))
#define IA32_VMX_BASIC_MSR		0x480
#define  VMX_BASIC_HI_DUAL_MONITOR	(1UL << (49 - 32))
#define IA32_VMX_MISC_MSR		0x485

#define IA32_PM_ENABLE			0x770
#define HWP_ENABLE			0x1
#define IA32_HWP_CAPABILITIES		0x771
#define IA32_HWP_REQUEST		0x774
#define IA32_HWP_REQUEST_EPP_MASK	0xff000000
#define IA32_HWP_REQUEST_EPP_SHIFT	24
#define IA32_HWP_STATUS			0x777
#define IA32_L3_PROTECTED_WAYS		0xc85
#define IA32_SF_QOS_INFO		0xc87
#define  IA32_SF_WAY_COUNT_MASK		0x3f
#define IA32_PQR_ASSOC			0xc8f
/* MSR bits 33:32 encode slot number 0-3 */
#define  IA32_PQR_ASSOC_MASK		(1 << 0 | 1 << 1)
#define IA32_L3_MASK_1			0xc91
#define IA32_L3_MASK_2			0xc92

#define IA32_CR_SF_QOS_MASK_1		0x1891
#define IA32_CR_SF_QOS_MASK_2		0x1892

#ifndef __ASSEMBLER__

typedef struct msrinit_struct {
	unsigned int index;
	msr_t msr;
} msrinit_t;

/* Get MCA bank count from MSR */
static inline unsigned int mca_get_bank_count(void)
{
	msr_t msr = rdmsr(IA32_MCG_CAP);
	return msr.lo & MCA_BANKS_MASK;
}

/* Clear all MCA status registers */
static inline void mca_clear_status(void)
{
	const unsigned int num_banks = mca_get_bank_count();
	const msr_t msr = {.lo = 0, .hi = 0};

	for (unsigned int i = 0 ; i < num_banks ; i++)
		wrmsr(IA32_MC_STATUS(i), msr);
}

/* Helpers for interpreting MC[i]_STATUS */

static inline int mca_valid(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_VAL);
}

static inline int mca_over(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_OVERFLOW);
}

static inline int mca_uc(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_UC);
}

static inline int mca_en(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_EN);
}

static inline int mca_miscv(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_MISCV);
}

static inline int mca_addrv(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_ADDRV);
}

static inline int mca_pcc(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_PCC);
}

static inline int mca_idv(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_COREID_VAL);
}

static inline int mca_cecc(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_CECC);
}

static inline int mca_uecc(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_UECC);
}

static inline int mca_defd(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_DEFERRED);
}

static inline int mca_poison(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_POISON);
}

static inline int mca_sublink(msr_t msr)
{
	return !!(msr.hi & MCA_STATUS_HI_SUBLINK);
}

static inline uint16_t mca_err_code(msr_t reg)
{
	return reg.lo & MCA_STATUS_LO_ERRCODE_MASK;
}

static inline uint16_t mca_err_extcode(msr_t reg)
{
	return reg.lo & MCA_STATUS_LO_ERRCODE_EXT_MASK;
}

/* Machine Check errors may be categorized by type, as determined by the
 * Error Code field of MC[i]_STATUS.  The definitions below can typically
 * be found by searching the BKDG for a table called "Error Code Types".
 */
/* TLB Errors 0000 0000 0001 TTLL */
#define MCA_ERRCODE_TLB_DETECT		0xfff0
#define MCA_ERRCODE_TLB_TT_SH		2 /* Transaction Type */
#define MCA_ERRCODE_TLB_TT_MASK		(0x3 << MCA_ERRCODE_TLB_TT_SH)
#define MCA_ERRCODE_TLB_LL_SH		0 /* Cache Level */
#define MCA_ERRCODE_TLB_LL_MASK		(0x3 << MCA_ERRCODE_TLB_LL_SH)

/* Memory Errors 0000 0001 RRRR TTLL */
#define MCA_ERRCODE_MEM_DETECT		0xff00
#define MCA_ERRCODE_MEM_RRRR_SH		4 /* Memory Transaction Type */
#define MCA_ERRCODE_MEM_RRRR_MASK	(0xf << MCA_ERRCODE_MEM_RRRR_MASK)
#define MCA_ERRCODE_MEM_TT_SH		2 /* Transaction Type */
#define MCA_ERRCODE_MEM_TT_MASK		(0x3 << MCA_ERRCODE_MEM_TT_SH)
#define MCA_ERRCODE_MEM_LL_SH		0 /* Cache Level */
#define MCA_ERRCODE_MEM_LL_MASK		(0x3 << MCA_ERRCODE_MEM_LL_SH)

/* Bus Errors 0000 1PPT RRRR IILL */
#define MCA_ERRCODE_BUS_DETECT		0xf800
#define MCA_ERRCODE_BUS_PP_SH		9 /* Participation Processor */
#define MCA_ERRCODE_BUS_PP_MASK		(0x3 << MCA_ERRCODE_BUS_PP_SH)
#define MCA_ERRCODE_BUS_T_SH		8 /* Timeout */
#define MCA_ERRCODE_BUS_T_MASK		(0x1 << MCA_ERRCODE_BUS_T_SH)
#define MCA_ERRCODE_BUS_RRRR_SH		4 /* Memory Transaction Type */
#define MCA_ERRCODE_BUS_RRRR_MASK	(0xf << MCA_ERRCODE_BUS_RRRR_SH)
#define MCA_ERRCODE_BUS_II_SH		2 /* Memory or IO */
#define MCA_ERRCODE_BUS_II_MASK		(0x3 << MCA_ERRCODE_BUS_II_SH)
#define MCA_ERRCODE_BUS_LL_SH		0 /* Cache Level */
#define MCA_ERRCODE_BUS_LL_MASK		(0x3 << MCA_ERRCODE_BUS_LL_SH)

/* Int. Unclassified Errors 0000 01UU 0000 0000 */
#define MCA_ERRCODE_INT_DETECT		0xfc00
#define MCA_ERRCODE_INT_UU_SH		8 /* Internal Error Type */
#define MCA_ERRCODE_INT_UU_MASK		(0x3 << MCA_ERRCODE_INT_UU_SH)

#define MCA_BANK_LS 0 /* Load-store, including DC */
#define MCA_BANK_IF 1 /* Instruction Fetch, including IC */
#define MCA_BANK_CU 2 /* Combined Unit, including L2 */
/* bank 3 reserved */
#define MCA_BANK_NB 4 /* Northbridge, including IO link */
#define MCA_BANK_EX 5 /* Execution Unit */
#define MCA_BANK_FP 6 /* Floating Point */

enum mca_err_code_types {
	MCA_ERRTYPE_UNKNOWN,
	MCA_ERRTYPE_TLB,
	MCA_ERRTYPE_MEM,
	MCA_ERRTYPE_BUS,
	MCA_ERRTYPE_INT
};

static inline enum mca_err_code_types mca_err_type(msr_t reg)
{
	uint16_t error = mca_err_code(reg);
	if (error & MCA_ERRCODE_BUS_DETECT) /* this order must be maintained */
		return MCA_ERRTYPE_BUS;
	if (error & MCA_ERRCODE_INT_DETECT)
		return MCA_ERRTYPE_INT;
	if (error & MCA_ERRCODE_MEM_DETECT)
		return MCA_ERRTYPE_MEM;
	if (error & MCA_ERRCODE_TLB_DETECT)
		return MCA_ERRTYPE_TLB;
	return MCA_ERRTYPE_UNKNOWN;
}

/**
 * Helper for reading a MSR
 *
 * @param[in] reg	The MSR.
 */
static inline uint64_t msr_read(unsigned int reg)
{
	msr_t msr = rdmsr(reg);
	return (((uint64_t)msr.hi << 32) | msr.lo);
}

/**
 * Helper for writing a MSR
 *
 * @param[in] reg	The MSR.
 * @param[in] value	The value to be written to the MSR.
 */
static inline void msr_write(unsigned int reg, uint64_t value)
{
	msr_t msr = {
		.lo = (unsigned int)value,
		.hi = (unsigned int)(value >> 32)
	};
	wrmsr(reg, msr);
}

/**
 * Helper for (un)setting MSR bitmasks
 *
 * @param[in] reg	The MSR.
 * @param[in] unset	Bitmask with ones to the bits to unset from the MSR.
 * @param[in] set	Bitmask with ones to the bits to set from the MSR.
 */
static inline void msr_unset_and_set(unsigned int reg, uint64_t unset, uint64_t set)
{
	msr_t msr;

	msr = rdmsr(reg);
	msr.lo &= (unsigned int)~unset;
	msr.hi &= (unsigned int)~(unset >> 32);
	msr.lo |= (unsigned int)set;
	msr.hi |= (unsigned int)(set >> 32);
	wrmsr(reg, msr);
}

/**
 * Helper for setting MSR bitmasks
 *
 * @param[in] reg	The MSR.
 * @param[in] set	Bitmask with ones to the bits to set from the MSR.
 */
static inline void msr_set(unsigned int reg, uint64_t set)
{
	msr_unset_and_set(reg, 0, set);
}

/**
 * Helper for unsetting MSR bitmasks
 *
 * @param[in] reg	The MSR.
 * @param[in] unset	Bitmask with ones to the bits to unset from the MSR.
 */
static inline void msr_unset(unsigned int reg, uint64_t unset)
{
	msr_unset_and_set(reg, unset, 0);
}

#endif /* __ASSEMBLER__ */
#endif /* CPU_X86_MSR_H */
