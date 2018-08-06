/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * lib_helpers.h: All library function prototypes and macros are defined in this
 * file.
 */

#ifndef __ARCH_LIB_HELPERS_H__
#define __ARCH_LIB_HELPERS_H__

#define EL0               0
#define EL1               1
#define EL2               2
#define EL3               3

#define CURRENT_EL_MASK   0x3
#define CURRENT_EL_SHIFT  2

#define SPSR_USE_L           0
#define SPSR_USE_H           1
#define SPSR_L_H_MASK        1
#define SPSR_M_SHIFT         4
#define SPSR_ERET_32         (1 << SPSR_M_SHIFT)
#define SPSR_ERET_64         (0 << SPSR_M_SHIFT)
#define SPSR_FIQ             (1 << 6)
#define SPSR_IRQ             (1 << 7)
#define SPSR_SERROR          (1 << 8)
#define SPSR_DEBUG           (1 << 9)
#define SPSR_EXCEPTION_MASK  (SPSR_FIQ | SPSR_IRQ | SPSR_SERROR | SPSR_DEBUG)

#define SCR_NS_SHIFT         0
#define SCR_NS_MASK          (1 << SCR_NS_SHIFT)
#define SCR_NS_ENABLE        (1 << SCR_NS_SHIFT)
#define SCR_NS_DISABLE       (0 << SCR_NS_SHIFT)
#define SCR_NS               SCR_NS_ENABLE
#define SCR_RES1             (0x3 << 4)
#define SCR_IRQ_SHIFT        2
#define SCR_IRQ_MASK         (1 << SCR_IRQ_SHIFT)
#define SCR_IRQ_ENABLE       (1 << SCR_IRQ_SHIFT)
#define SCR_IRQ_DISABLE      (0 << SCR_IRQ_SHIFT)
#define SCR_FIQ_SHIFT        2
#define SCR_FIQ_MASK         (1 << SCR_FIQ_SHIFT)
#define SCR_FIQ_ENABLE       (1 << SCR_FIQ_SHIFT)
#define SCR_FIQ_DISABLE      (0 << SCR_FIQ_SHIFT)
#define SCR_EA_SHIFT         3
#define SCR_EA_MASK          (1 << SCR_EA_SHIFT)
#define SCR_EA_ENABLE        (1 << SCR_EA_SHIFT)
#define SCR_EA_DISABLE       (0 << SCR_EA_SHIFT)
#define SCR_SMD_SHIFT        7
#define SCR_SMD_MASK         (1 << SCR_SMD_SHIFT)
#define SCR_SMD_DISABLE      (1 << SCR_SMD_SHIFT)
#define SCR_SMD_ENABLE       (0 << SCR_SMD_SHIFT)
#define SCR_HVC_SHIFT        8
#define SCR_HVC_MASK         (1 << SCR_HVC_SHIFT)
#define SCR_HVC_DISABLE      (0 << SCR_HVC_SHIFT)
#define SCR_HVC_ENABLE       (1 << SCR_HVC_SHIFT)
#define SCR_SIF_SHIFT        9
#define SCR_SIF_MASK         (1 << SCR_SIF_SHIFT)
#define SCR_SIF_ENABLE       (1 << SCR_SIF_SHIFT)
#define SCR_SIF_DISABLE      (0 << SCR_SIF_SHIFT)
#define SCR_RW_SHIFT         10
#define SCR_RW_MASK          (1 << SCR_RW_SHIFT)
#define SCR_LOWER_AARCH64    (1 << SCR_RW_SHIFT)
#define SCR_LOWER_AARCH32    (0 << SCR_RW_SHIFT)
#define SCR_ST_SHIFT         11
#define SCR_ST_MASK          (1 << SCR_ST_SHIFT)
#define SCR_ST_ENABLE        (1 << SCR_ST_SHIFT)
#define SCR_ST_DISABLE       (0 << SCR_ST_SHIFT)
#define SCR_TWI_SHIFT        12
#define SCR_TWI_MASK         (1 << SCR_TWI_SHIFT)
#define SCR_TWI_ENABLE       (1 << SCR_TWI_SHIFT)
#define SCR_TWI_DISABLE      (0 << SCR_TWI_SHIFT)
#define SCR_TWE_SHIFT        13
#define SCR_TWE_MASK         (1 << SCR_TWE_SHIFT)
#define SCR_TWE_ENABLE       (1 << SCR_TWE_SHIFT)
#define SCR_TWE_DISABLE      (0 << SCR_TWE_SHIFT)

#define HCR_RW_SHIFT         31
#define HCR_LOWER_AARCH64    (1 << HCR_RW_SHIFT)
#define HCR_LOWER_AARCH32    (0 << HCR_RW_SHIFT)

#define SCTLR_MMU_ENABLE     1
#define SCTLR_MMU_DISABLE    0
#define SCTLR_ACE_SHIFT      1
#define SCTLR_ACE_ENABLE     (1 << SCTLR_ACE_SHIFT)
#define SCTLR_ACE_DISABLE    (0 << SCTLR_ACE_SHIFT)
#define SCTLR_CACHE_SHIFT    2
#define SCTLR_CACHE_ENABLE   (1 << SCTLR_CACHE_SHIFT)
#define SCTLR_CACHE_DISABLE  (0 << SCTLR_CACHE_SHIFT)
#define SCTLR_SAE_SHIFT      3
#define SCTLR_SAE_ENABLE     (1 << SCTLR_SAE_SHIFT)
#define SCTLR_SAE_DISABLE    (0 << SCTLR_SAE_SHIFT)
#define SCTLR_RES1           ((0x3 << 4) | (0x1 << 11) | (0x1 << 16) |	\
			      (0x1 << 18) | (0x3 << 22) | (0x3 << 28))
#define SCTLR_ICE_SHIFT      12
#define SCTLR_ICE_ENABLE     (1 << SCTLR_ICE_SHIFT)
#define SCTLR_ICE_DISABLE    (0 << SCTLR_ICE_SHIFT)
#define SCTLR_WXN_SHIFT      19
#define SCTLR_WXN_ENABLE     (1 << SCTLR_WXN_SHIFT)
#define SCTLR_WXN_DISABLE    (0 << SCTLR_WXN_SHIFT)
#define SCTLR_ENDIAN_SHIFT   25
#define SCTLR_LITTLE_END     (0 << SCTLR_ENDIAN_SHIFT)
#define SCTLR_BIG_END        (1 << SCTLR_ENDIAN_SHIFT)

#define CPTR_EL3_TCPAC_SHIFT	(31)
#define CPTR_EL3_TTA_SHIFT	(20)
#define CPTR_EL3_TFP_SHIFT	(10)
#define CPTR_EL3_TCPAC_DISABLE	(0 << CPTR_EL3_TCPAC_SHIFT)
#define CPTR_EL3_TCPAC_ENABLE	(1 << CPTR_EL3_TCPAC_SHIFT)
#define CPTR_EL3_TTA_DISABLE	(0 << CPTR_EL3_TTA_SHIFT)
#define CPTR_EL3_TTA_ENABLE	(1 << CPTR_EL3_TTA_SHIFT)
#define CPTR_EL3_TFP_DISABLE	(0 << CPTR_EL3_TFP_SHIFT)
#define CPTR_EL3_TFP_ENABLE	(1 << CPTR_EL3_TFP_SHIFT)

#define CPACR_TTA_SHIFT	(28)
#define CPACR_TTA_ENABLE	(1 << CPACR_TTA_SHIFT)
#define CPACR_TTA_DISABLE	(0 << CPACR_TTA_SHIFT)
#define CPACR_FPEN_SHIFT	(20)
/*
 * ARMv8-A spec: Values 0b00 and 0b10 both seem to enable traps from el0 and el1
 * for fp reg access.
 */
#define CPACR_TRAP_FP_EL0_EL1	(0 << CPACR_FPEN_SHIFT)
#define CPACR_TRAP_FP_EL0	(1 << CPACR_FPEN_SHIFT)
#define CPACR_TRAP_FP_DISABLE	(3 << CPACR_FPEN_SHIFT)

#define DAIF_DBG_BIT      (1<<3)
#define DAIF_ABT_BIT      (1<<2)
#define DAIF_IRQ_BIT      (1<<1)
#define DAIF_FIQ_BIT      (1<<0)

#ifndef __ASSEMBLER__

#include <stdint.h>

#define MAKE_REGISTER_ACCESSORS(reg) \
	static inline uint64_t raw_read_##reg(void) \
	{ \
		uint64_t value; \
		__asm__ __volatile__("mrs %0, " #reg "\n\t" \
				     : "=r" (value) : : "memory"); \
		return value; \
	} \
	static inline void raw_write_##reg(uint64_t value) \
	{ \
		__asm__ __volatile__("msr " #reg ", %0\n\t" \
				     : : "r" (value) : "memory"); \
	}

#define MAKE_REGISTER_ACCESSORS_EL123(reg) \
	MAKE_REGISTER_ACCESSORS(reg##_el1) \
	MAKE_REGISTER_ACCESSORS(reg##_el2) \
	MAKE_REGISTER_ACCESSORS(reg##_el3)

/* Architectural register accessors */
MAKE_REGISTER_ACCESSORS_EL123(actlr)
MAKE_REGISTER_ACCESSORS_EL123(afsr0)
MAKE_REGISTER_ACCESSORS_EL123(afsr1)
MAKE_REGISTER_ACCESSORS(aidr_el1)
MAKE_REGISTER_ACCESSORS_EL123(amair)
MAKE_REGISTER_ACCESSORS(ccsidr_el1)
MAKE_REGISTER_ACCESSORS(clidr_el1)
MAKE_REGISTER_ACCESSORS(cntfrq_el0)
MAKE_REGISTER_ACCESSORS(cnthctl_el2)
MAKE_REGISTER_ACCESSORS(cnthp_ctl_el2)
MAKE_REGISTER_ACCESSORS(cnthp_cval_el2)
MAKE_REGISTER_ACCESSORS(cnthp_tval_el2)
MAKE_REGISTER_ACCESSORS(cntkctl_el1)
MAKE_REGISTER_ACCESSORS(cntp_ctl_el0)
MAKE_REGISTER_ACCESSORS(cntp_cval_el0)
MAKE_REGISTER_ACCESSORS(cntp_tval_el0)
MAKE_REGISTER_ACCESSORS(cntpct_el0)
MAKE_REGISTER_ACCESSORS(cntps_ctl_el1)
MAKE_REGISTER_ACCESSORS(cntps_cval_el1)
MAKE_REGISTER_ACCESSORS(cntps_tval_el1)
MAKE_REGISTER_ACCESSORS(cntv_ctl_el0)
MAKE_REGISTER_ACCESSORS(cntv_cval_el0)
MAKE_REGISTER_ACCESSORS(cntv_tval_el0)
MAKE_REGISTER_ACCESSORS(cntvct_el0)
MAKE_REGISTER_ACCESSORS(cntvoff_el2)
MAKE_REGISTER_ACCESSORS(contextidr_el1)
MAKE_REGISTER_ACCESSORS(cpacr_el1)
MAKE_REGISTER_ACCESSORS(cptr_el2)
MAKE_REGISTER_ACCESSORS(cptr_el3)
MAKE_REGISTER_ACCESSORS(csselr_el1)
MAKE_REGISTER_ACCESSORS(ctr_el0)
MAKE_REGISTER_ACCESSORS(currentel)
MAKE_REGISTER_ACCESSORS(daif)
MAKE_REGISTER_ACCESSORS(dczid_el0)
MAKE_REGISTER_ACCESSORS_EL123(elr)
MAKE_REGISTER_ACCESSORS_EL123(esr)
MAKE_REGISTER_ACCESSORS_EL123(far)
MAKE_REGISTER_ACCESSORS(fpcr)
MAKE_REGISTER_ACCESSORS(fpsr)
MAKE_REGISTER_ACCESSORS(hacr_el2)
MAKE_REGISTER_ACCESSORS(hcr_el2)
MAKE_REGISTER_ACCESSORS(hpfar_el2)
MAKE_REGISTER_ACCESSORS(hstr_el2)
MAKE_REGISTER_ACCESSORS(isr_el1)
MAKE_REGISTER_ACCESSORS_EL123(mair)
MAKE_REGISTER_ACCESSORS(midr_el1)
MAKE_REGISTER_ACCESSORS(mpidr_el1)
MAKE_REGISTER_ACCESSORS(nzcv)
MAKE_REGISTER_ACCESSORS(par_el1)
MAKE_REGISTER_ACCESSORS(revdir_el1)
MAKE_REGISTER_ACCESSORS_EL123(rmr)
MAKE_REGISTER_ACCESSORS_EL123(rvbar)
MAKE_REGISTER_ACCESSORS(scr_el3)
MAKE_REGISTER_ACCESSORS_EL123(sctlr)
MAKE_REGISTER_ACCESSORS(sp_el0)
MAKE_REGISTER_ACCESSORS(sp_el1)
MAKE_REGISTER_ACCESSORS(sp_el2)
MAKE_REGISTER_ACCESSORS(spsel)
MAKE_REGISTER_ACCESSORS_EL123(spsr)
MAKE_REGISTER_ACCESSORS(spsr_abt)
MAKE_REGISTER_ACCESSORS(spsr_fiq)
MAKE_REGISTER_ACCESSORS(spsr_irq)
MAKE_REGISTER_ACCESSORS(spsr_und)
MAKE_REGISTER_ACCESSORS_EL123(tcr)
MAKE_REGISTER_ACCESSORS_EL123(tpidr)
MAKE_REGISTER_ACCESSORS_EL123(ttbr0)
MAKE_REGISTER_ACCESSORS(ttbr1_el1)
MAKE_REGISTER_ACCESSORS_EL123(vbar)
MAKE_REGISTER_ACCESSORS(vmpidr_el2)
MAKE_REGISTER_ACCESSORS(vpidr_el2)
MAKE_REGISTER_ACCESSORS(vtcr_el2)
MAKE_REGISTER_ACCESSORS(vttbr_el2)

/* Special DAIF accessor functions */
static inline void enable_debug_exceptions(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t"
			     : : "i" (DAIF_DBG_BIT)  : "memory");
}

static inline void enable_serror_exceptions(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t"
			     : : "i" (DAIF_ABT_BIT)  : "memory");
}

static inline void enable_irq(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t"
			     : : "i" (DAIF_IRQ_BIT)  : "memory");
}

static inline void enable_fiq(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t"
			     : : "i" (DAIF_FIQ_BIT)  : "memory");
}

static inline void disable_debug_exceptions(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t"
			     : : "i" (DAIF_DBG_BIT)  : "memory");
}

static inline void disable_serror_exceptions(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t"
			     : : "i" (DAIF_ABT_BIT)  : "memory");
}

static inline void disable_irq(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t"
			     : : "i" (DAIF_IRQ_BIT)  : "memory");
}

static inline void disable_fiq(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t"
			     : : "i" (DAIF_FIQ_BIT)  : "memory");
}

/* Cache maintenance system instructions */
static inline void dccisw(uint64_t cisw)
{
	__asm__ __volatile__("dc cisw, %0\n\t" : : "r" (cisw) : "memory");
}

static inline void dccivac(uint64_t civac)
{
	__asm__ __volatile__("dc civac, %0\n\t" : : "r" (civac) : "memory");
}

static inline void dccsw(uint64_t csw)
{
	__asm__ __volatile__("dc csw, %0\n\t" : : "r" (csw) : "memory");
}

static inline void dccvac(uint64_t cvac)
{
	__asm__ __volatile__("dc cvac, %0\n\t" : : "r" (cvac) : "memory");
}

static inline void dccvau(uint64_t cvau)
{
	__asm__ __volatile__("dc cvau, %0\n\t" : : "r" (cvau) : "memory");
}

static inline void dcisw(uint64_t isw)
{
	__asm__ __volatile__("dc isw, %0\n\t" : : "r" (isw) : "memory");
}

static inline void dcivac(uint64_t ivac)
{
	__asm__ __volatile__("dc ivac, %0\n\t" : : "r" (ivac) : "memory");
}

static inline void dczva(uint64_t zva)
{
	__asm__ __volatile__("dc zva, %0\n\t" : : "r" (zva) : "memory");
}

static inline void iciallu(void)
{
	__asm__ __volatile__("ic iallu\n\t" : : : "memory");
}

static inline void icialluis(void)
{
	__asm__ __volatile__("ic ialluis\n\t" : : : "memory");
}

static inline void icivau(uint64_t ivau)
{
	__asm__ __volatile__("ic ivau, %0\n\t" : : "r" (ivau) : "memory");
}

/* TLB maintenance instructions */
static inline void tlbiall_el1(void)
{
	__asm__ __volatile__("tlbi alle1\n\t" : : : "memory");
}

static inline void tlbiall_el2(void)
{
	__asm__ __volatile__("tlbi alle2\n\t" : : : "memory");
}

static inline void tlbiall_el3(void)
{
	__asm__ __volatile__("tlbi alle3\n\t" : : : "memory");
}

static inline void tlbiallis_el1(void)
{
	__asm__ __volatile__("tlbi alle1is\n\t" : : : "memory");
}

static inline void tlbiallis_el2(void)
{
	__asm__ __volatile__("tlbi alle2is\n\t" : : : "memory");
}

static inline void tlbiallis_el3(void)
{
	__asm__ __volatile__("tlbi alle3is\n\t" : : : "memory");
}

static inline void tlbivaa_el1(uint64_t va)
{
	__asm__ __volatile__("tlbi vaae1, %0\n\t" : : "r" (va) : "memory");
}

#endif /* __ASSEMBLER__ */

#endif /* __ARCH_LIB_HELPERS_H__ */
