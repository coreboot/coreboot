/*
 *
 * Copyright 2018 Google Inc
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __ARCH_LIB_HELPERS_H__
#define __ARCH_LIB_HELPERS_H__

#define SCTLR_M		(1 << 0)	/* MMU enable			*/
#define SCTLR_A		(1 << 1)	/* Alignment check enable	*/
#define SCTLR_C		(1 << 2)	/* Data/unified cache enable	*/
#define SCTLR_SA	(1 << 3)	/* Stack alignment check enable	*/
#define SCTLR_NAA	(1 << 6)	/* non-aligned access STA/LDR	*/
#define SCTLR_I		(1 << 12)	/* Instruction cache enable	*/
#define SCTLR_ENDB	(1 << 13)	/* Pointer auth (data B)	*/
#define SCTLR_WXN	(1 << 19)	/* Write permission implies XN	*/
#define SCTLR_IESB	(1 << 21)	/* Implicit error sync event	*/
#define SCTLR_EE	(1 << 25)	/* Exception endianness	(BE)	*/
#define SCTLR_ENDA	(1 << 27)	/* Pointer auth (data A)	*/
#define SCTLR_ENIB	(1 << 30)	/* Pointer auth (insn B)	*/
#define SCTLR_ENIA	(1 << 31)	/* Pointer auth (insn A)	*/
#define SCTLR_RES1           ((0x3 << 4) | (0x1 << 11) | (0x1 << 16) |	\
			      (0x1 << 18) | (0x3 << 22) | (0x3 << 28))

#define DAIF_DBG_BIT      (1 << 3)
#define DAIF_ABT_BIT      (1 << 2)
#define DAIF_IRQ_BIT      (1 << 1)
#define DAIF_FIQ_BIT      (1 << 0)

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

#define MAKE_REGISTER_ACCESSORS_EL12(reg) \
	MAKE_REGISTER_ACCESSORS(reg##_el1) \
	MAKE_REGISTER_ACCESSORS(reg##_el2)

/* Architectural register accessors */
MAKE_REGISTER_ACCESSORS_EL12(actlr)
MAKE_REGISTER_ACCESSORS_EL12(afsr0)
MAKE_REGISTER_ACCESSORS_EL12(afsr1)
MAKE_REGISTER_ACCESSORS(aidr_el1)
MAKE_REGISTER_ACCESSORS_EL12(amair)
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
MAKE_REGISTER_ACCESSORS(csselr_el1)
MAKE_REGISTER_ACCESSORS(ctr_el0)
MAKE_REGISTER_ACCESSORS(currentel)
MAKE_REGISTER_ACCESSORS(daif)
MAKE_REGISTER_ACCESSORS(dczid_el0)
MAKE_REGISTER_ACCESSORS_EL12(elr)
MAKE_REGISTER_ACCESSORS_EL12(esr)
MAKE_REGISTER_ACCESSORS_EL12(far)
MAKE_REGISTER_ACCESSORS(fpcr)
MAKE_REGISTER_ACCESSORS(fpsr)
MAKE_REGISTER_ACCESSORS(hacr_el2)
MAKE_REGISTER_ACCESSORS(hcr_el2)
MAKE_REGISTER_ACCESSORS(hpfar_el2)
MAKE_REGISTER_ACCESSORS(hstr_el2)
MAKE_REGISTER_ACCESSORS(isr_el1)
MAKE_REGISTER_ACCESSORS_EL12(mair)
MAKE_REGISTER_ACCESSORS_EL12(mdcr)
MAKE_REGISTER_ACCESSORS(mdscr_el1)
MAKE_REGISTER_ACCESSORS(midr_el1)
MAKE_REGISTER_ACCESSORS(mpidr_el1)
MAKE_REGISTER_ACCESSORS(nzcv)
MAKE_REGISTER_ACCESSORS(oslar_el1)
MAKE_REGISTER_ACCESSORS(oslsr_el1)
MAKE_REGISTER_ACCESSORS(par_el1)
MAKE_REGISTER_ACCESSORS(revdir_el1)
MAKE_REGISTER_ACCESSORS_EL12(rmr)
MAKE_REGISTER_ACCESSORS_EL12(rvbar)
MAKE_REGISTER_ACCESSORS_EL12(sctlr)
MAKE_REGISTER_ACCESSORS(sp_el0)
MAKE_REGISTER_ACCESSORS(sp_el1)
MAKE_REGISTER_ACCESSORS(spsel)
MAKE_REGISTER_ACCESSORS_EL12(spsr)
MAKE_REGISTER_ACCESSORS(spsr_abt)
MAKE_REGISTER_ACCESSORS(spsr_fiq)
MAKE_REGISTER_ACCESSORS(spsr_irq)
MAKE_REGISTER_ACCESSORS(spsr_und)
MAKE_REGISTER_ACCESSORS_EL12(tcr)
MAKE_REGISTER_ACCESSORS_EL12(tpidr)
MAKE_REGISTER_ACCESSORS_EL12(ttbr0)
MAKE_REGISTER_ACCESSORS(ttbr1_el1)
MAKE_REGISTER_ACCESSORS_EL12(vbar)
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

static inline void tlbiallis_el1(void)
{
	__asm__ __volatile__("tlbi alle1is\n\t" : : : "memory");
}

static inline void tlbiallis_el2(void)
{
	__asm__ __volatile__("tlbi alle2is\n\t" : : : "memory");
}

static inline void tlbivaa_el1(uint64_t va)
{
	__asm__ __volatile__("tlbi vaae1, %0\n\t" : : "r" (va) : "memory");
}

/* Memory barrier */
/* data memory barrier */
#define dmb_opt(opt)  asm volatile ("dmb " #opt : : : "memory")
/* data sync barrier */
#define dsb_opt(opt)  asm volatile ("dsb " #opt : : : "memory")
/* instruction sync barrier */
#define isb_opt(opt)  asm volatile ("isb " #opt : : : "memory")

#define dmb() dmb_opt(sy)
#define dsb() dsb_opt(sy)
#define isb() isb_opt()

#endif	/* __ASSEMBLER__ */

#endif /* __ARCH_LIB_HELPERS_H__ */
