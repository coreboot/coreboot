/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 *
 * Reference: ARM Architecture Reference Manual, ARMv8-A edition
 * pstate.c: This file defines all the library functions for accessing
 * PSTATE and special purpose registers
 */

#include <stdint.h>

#include <arch/lib_helpers.h>

/* CurrentEL */
uint32_t raw_read_current_el(void)
{
	uint32_t current_el;

	__asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");

	return current_el;
}

uint32_t get_current_el(void)
{
	uint32_t current_el = raw_read_current_el();
	return ((current_el >> CURRENT_EL_SHIFT) & CURRENT_EL_MASK);
}

/* DAIF */
uint32_t raw_read_daif(void)
{
	uint32_t daif;

	__asm__ __volatile__("mrs %0, DAIF\n\t" : "=r" (daif) :  : "memory");

	return daif;
}

void raw_write_daif(uint32_t daif)
{
	__asm__ __volatile__("msr DAIF, %0\n\t" : : "r" (daif) : "memory");
}

void enable_debug_exceptions(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_DBG_BIT)  : "memory");
}

void enable_serror_exceptions(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_ABT_BIT)  : "memory");
}

void enable_irq(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_IRQ_BIT)  : "memory");
}

void enable_fiq(void)
{
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_FIQ_BIT)  : "memory");
}

void disable_debug_exceptions(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_DBG_BIT)  : "memory");
}

void disable_serror_exceptions(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_ABT_BIT)  : "memory");
}

void disable_irq(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_IRQ_BIT)  : "memory");
}

void disable_fiq(void)
{
	__asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_FIQ_BIT)  : "memory");
}

/* DLR_EL0 */
uint64_t raw_read_dlr_el0(void)
{
	uint64_t dlr_el0;

	__asm__ __volatile__("mrs %0, DLR_EL0\n\t" : "=r" (dlr_el0) :  : "memory");

	return dlr_el0;
}
void raw_write_dlr_el0(uint64_t dlr_el0)
{
	__asm__ __volatile__("msr DLR_EL0, %0\n\t" : : "r" (dlr_el0) : "memory");
}

/* DSPSR_EL0 */
uint64_t raw_read_dspsr_el0(void)
{
	uint64_t dspsr_el0;

	__asm__ __volatile__("mrs %0, DSPSR_EL0\n\t" : "=r" (dspsr_el0) :  : "memory");

	return dspsr_el0;
}
void raw_write_dspsr_el0(uint64_t dspsr_el0)
{
	__asm__ __volatile__("msr DSPSR_EL0, %0\n\t" : : "r" (dspsr_el0) : "memory");
}

/* ELR */
uint64_t raw_read_elr_el1(void)
{
	uint64_t elr_el1;

	__asm__ __volatile__("mrs %0, ELR_EL1\n\t" : "=r" (elr_el1) :  : "memory");

	return elr_el1;
}

void raw_write_elr_el1(uint64_t elr_el1)
{
	__asm__ __volatile__("msr ELR_EL1, %0\n\t" : : "r" (elr_el1) : "memory");
}

uint64_t raw_read_elr_el2(void)
{
	uint64_t elr_el2;

	__asm__ __volatile__("mrs %0, ELR_EL2\n\t" : "=r" (elr_el2) :  : "memory");

	return elr_el2;
}

void raw_write_elr_el2(uint64_t elr_el2)
{
	__asm__ __volatile__("msr ELR_EL2, %0\n\t" : : "r" (elr_el2) : "memory");
}

uint64_t raw_read_elr_el3(void)
{
	uint64_t elr_el3;

	__asm__ __volatile__("mrs %0, ELR_EL3\n\t" : "=r" (elr_el3) :  : "memory");

	return elr_el3;
}

void raw_write_elr_el3(uint64_t elr_el3)
{
	__asm__ __volatile__("msr ELR_EL3, %0\n\t" : : "r" (elr_el3) : "memory");
}

uint64_t raw_read_elr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_elr(el);
}

void raw_write_elr_current(uint64_t elr)
{
	uint32_t el = get_current_el();
	raw_write_elr(elr, el);
}

uint64_t raw_read_elr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_elr, elr, uint64_t, el);
}

void raw_write_elr(uint64_t elr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_elr, elr, el);
}

/* FPCR */
uint32_t raw_read_fpcr(void)
{
	uint32_t fpcr;

	__asm__ __volatile__("mrs %0, FPCR\n\t" : "=r" (fpcr) :  : "memory");

	return fpcr;
}

void raw_write_fpcr(uint32_t fpcr)
{
	__asm__ __volatile__("msr FPCR, %0\n\t" : : "r" (fpcr) : "memory");
}

/* FPSR */
uint32_t raw_read_fpsr(void)
{
	uint32_t fpsr;

	__asm__ __volatile__("mrs %0, FPSR\n\t" : "=r" (fpsr) :  : "memory");

	return fpsr;
}

void raw_write_fpsr(uint32_t fpsr)
{
	__asm__ __volatile__("msr FPSR, %0\n\t" : : "r" (fpsr) : "memory");
}

/* NZCV */
uint32_t raw_read_nzcv(void)
{
	uint32_t nzcv;

	__asm__ __volatile__("mrs %0, NZCV\n\t" : "=r" (nzcv) :  : "memory");

	return nzcv;
}

void raw_write_nzcv(uint32_t nzcv)
{
	__asm__ __volatile__("msr NZCV, %0\n\t" : : "r" (nzcv) : "memory");
}

/* SP */
uint64_t raw_read_sp_el0(void)
{
	uint64_t sp_el0;

	__asm__ __volatile__("mrs %0, SP_EL0\n\t" : "=r" (sp_el0) :  : "memory");

	return sp_el0;
}

void raw_write_sp_el0(uint64_t sp_el0)
{
	__asm__ __volatile__("msr SP_EL0, %0\n\t" : : "r" (sp_el0) : "memory");
}

uint64_t raw_read_sp_el1(void)
{
	uint64_t sp_el1;

	__asm__ __volatile__("mrs %0, SP_EL1\n\t" : "=r" (sp_el1) :  : "memory");

	return sp_el1;
}

void raw_write_sp_el1(uint64_t sp_el1)
{
	__asm__ __volatile__("msr SP_EL1, %0\n\t" : : "r" (sp_el1) : "memory");
}

uint64_t raw_read_sp_el2(void)
{
	uint64_t sp_el2;

	__asm__ __volatile__("mrs %0, SP_EL2\n\t" : "=r" (sp_el2) :  : "memory");

	return sp_el2;
}

void raw_write_sp_el2(uint64_t sp_el2)
{
	__asm__ __volatile__("msr SP_EL2, %0\n\t" : : "r" (sp_el2) : "memory");
}

/* SPSel */
uint32_t raw_read_spsel(void)
{
	uint32_t spsel;

	__asm__ __volatile__("mrs %0, SPSel\n\t" : "=r" (spsel) :  : "memory");

	return spsel;
}

void raw_write_spsel(uint32_t spsel)
{
	__asm__ __volatile__("msr SPSel, %0\n\t" : : "r" (spsel) : "memory");
}

uint64_t raw_read_sp_el3(void)
{
	uint64_t sp_el3;
	uint32_t spsel;

	spsel = raw_read_spsel();
	if (!spsel)
		raw_write_spsel(1);

	__asm__ __volatile__("mov %0, sp\n\t" : "=r" (sp_el3) :  : "memory");

	if (!spsel)
		raw_write_spsel(spsel);

	return sp_el3;
}

void raw_write_sp_el3(uint64_t sp_el3)
{
	uint32_t spsel;

	spsel = raw_read_spsel();
	if (!spsel)
		raw_write_spsel(1);

	__asm__ __volatile__("mov sp, %0\n\t" : "=r" (sp_el3) :  : "memory");

	if (!spsel)
		raw_write_spsel(spsel);
}

/* SPSR */
uint32_t raw_read_spsr_abt(void)
{
	uint32_t spsr_abt;

	__asm__ __volatile__("mrs %0, SPSR_abt\n\t" : "=r" (spsr_abt) :  : "memory");

	return spsr_abt;
}

void raw_write_spsr_abt(uint32_t spsr_abt)
{
	__asm__ __volatile__("msr SPSR_abt, %0\n\t" : : "r" (spsr_abt) : "memory");
}

uint32_t raw_read_spsr_el1(void)
{
	uint32_t spsr_el1;

	__asm__ __volatile__("mrs %0, SPSR_EL1\n\t" : "=r" (spsr_el1) :  : "memory");

	return spsr_el1;
}

void raw_write_spsr_el1(uint32_t spsr_el1)
{
	__asm__ __volatile__("msr SPSR_EL1, %0\n\t" : : "r" (spsr_el1) : "memory");
}

uint32_t raw_read_spsr_el2(void)
{
	uint32_t spsr_el2;

	__asm__ __volatile__("mrs %0, SPSR_EL2\n\t" : "=r" (spsr_el2) :  : "memory");

	return spsr_el2;
}

void raw_write_spsr_el2(uint32_t spsr_el2)
{
	__asm__ __volatile__("msr SPSR_EL2, %0\n\t" : : "r" (spsr_el2) : "memory");
}

uint32_t raw_read_spsr_el3(void)
{
	uint32_t spsr_el3;

	__asm__ __volatile__("mrs %0, SPSR_EL3\n\t" : "=r" (spsr_el3) :  : "memory");

	return spsr_el3;
}

void raw_write_spsr_el3(uint32_t spsr_el3)
{
	__asm__ __volatile__("msr SPSR_EL3, %0\n\t" : : "r" (spsr_el3) : "memory");
}

uint32_t raw_read_spsr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_spsr(el);
}

void raw_write_spsr_current(uint32_t spsr)
{
	uint32_t el = get_current_el();
	raw_write_spsr(spsr, el);
}

uint32_t raw_read_spsr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_spsr, spsr, uint32_t, el);
}

void raw_write_spsr(uint32_t spsr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_spsr, spsr, el);
}

uint32_t raw_read_spsr_fiq(void)
{
	uint32_t spsr_fiq;

	__asm__ __volatile__("mrs %0, SPSR_fiq\n\t" : "=r" (spsr_fiq) :  : "memory");

	return spsr_fiq;
}

void raw_write_spsr_fiq(uint32_t spsr_fiq)
{
	__asm__ __volatile__("msr SPSR_fiq, %0\n\t" : : "r" (spsr_fiq) : "memory");
}

uint32_t raw_read_spsr_irq(void)
{
	uint32_t spsr_irq;

	__asm__ __volatile__("mrs %0, SPSR_irq\n\t" : "=r" (spsr_irq) :  : "memory");

	return spsr_irq;
}

void raw_write_spsr_irq(uint32_t spsr_irq)
{
	__asm__ __volatile__("msr SPSR_irq, %0\n\t" : : "r" (spsr_irq) : "memory");
}

uint32_t raw_read_spsr_und(void)
{
	uint32_t spsr_und;

	__asm__ __volatile__("mrs %0, SPSR_und\n\t" : "=r" (spsr_und) :  : "memory");

	return spsr_und;
}

void raw_write_spsr_und(uint32_t spsr_und)
{
	__asm__ __volatile__("msr SPSR_und, %0\n\t" : : "r" (spsr_und) : "memory");
}
