/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef CPU_AMD_FAM15_H
#define CPU_AMD_FAM15_H

#include <types.h>
#include <cpu/x86/msr.h>

#define MCG_CAP				0x00000179
# define MCA_BANKS_MASK			0xff
#define MC0_CTL				0x00000400
#define MC0_STATUS			0x00000401
# define MCA_STATUS_HI_VAL		BIT(63 - 32)
# define MCA_STATUS_HI_OVERFLOW		BIT(62 - 32)
# define MCA_STATUS_HI_UC		BIT(61 - 32)
# define MCA_STATUS_HI_EN		BIT(60 - 32)
# define MCA_STATUS_HI_MISCV		BIT(59 - 32)
# define MCA_STATUS_HI_ADDRV		BIT(58 - 32)
# define MCA_STATUS_HI_PCC		BIT(57 - 32)
# define MCA_STATUS_HI_COREID_VAL	BIT(56 - 32)
# define MCA_STATUS_HI_CECC		BIT(46 - 32)
# define MCA_STATUS_HI_UECC		BIT(45 - 32)
# define MCA_STATUS_HI_DEFERRED		BIT(44 - 32)
# define MCA_STATUS_HI_POISON		BIT(43 - 32)
# define MCA_STATUS_HI_SUBLINK		BIT(41 - 32)
# define MCA_STATUS_HI_ERRCOREID_MASK	(0xf << 0)
# define MCA_STATUS_LO_ERRCODE_EXT_SH	16
# define MCA_STATUS_LO_ERRCODE_EXT_MASK	(0x3f << MCA_STATUS_LO_ERRCODE_EXT_SH)
# define MCA_STATUS_LO_ERRCODE_MASK	(0xffff << 0)
#define MC0_ADDR			0x00000402
#define MC0_MISC			0x00000403
#define MC0_CTL_MASK			0xC0010044

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

#define MSR_SMM_BASE			0xC0010111
#define MSR_TSEG_BASE			0xC0010112
#define MSR_SMM_MASK			0xC0010113
# define SMM_TSEG_VALID			(1 << 1)
# define SMM_TSEG_WB			(6 << 12)
#define HWCR_MSR			0xC0010015
# define SMM_LOCK			(1 << 0)
#define NB_CFG_MSR			0xC001001f

#define MMIO_CONF_BASE			0xC0010058
# define MMIO_BUS_RANGE_SHIFT		2
# define MMIO_RANGE_EN			(1 << 0)

#define PSTATE_0_MSR			0xC0010064

#define LS_CFG_MSR			0xC0011020
#define IC_CFG_MSR			0xC0011021
#define DC_CFG_MSR			0xC0011022
#define CU_CFG_MSR			0xC0011023
#define CU_CFG2_MSR			0xC001102A

#define CPU_ID_FEATURES_MSR		0xC0011004
#define CPU_ID_EXT_FEATURES_MSR		0xC0011005

#define CORE_PERF_BOOST_CTRL		0x15C

#endif /* CPU_AMD_FAM15_H */
