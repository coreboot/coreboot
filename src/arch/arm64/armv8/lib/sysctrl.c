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
 * Reference: ARM Architecture Reference Manual, ARMv8-A edition
 * sysctrl.c: This file defines all the library functions for accessing system
 * control registers in Aarch64
 */

#include <stdint.h>

#include <arch/lib_helpers.h>

/* ACTLR */
uint32_t raw_read_actlr_el1(void)
{
	uint64_t actlr_el1;

	__asm__ __volatile__("mrs %0, ACTLR_EL1\n\t" : "=r" (actlr_el1) :  : "memory");

	return actlr_el1;
}

void raw_write_actlr_el1(uint32_t actlr_el1)
{
	__asm__ __volatile__("msr ACTLR_EL1, %0\n\t" : : "r" ((uint64_t)actlr_el1) : "memory");
}

uint32_t raw_read_actlr_el2(void)
{
	uint64_t actlr_el2;

	__asm__ __volatile__("mrs %0, ACTLR_EL2\n\t" : "=r" (actlr_el2) :  : "memory");

	return actlr_el2;
}

void raw_write_actlr_el2(uint32_t actlr_el2)
{
	__asm__ __volatile__("msr ACTLR_EL2, %0\n\t" : : "r" ((uint64_t)actlr_el2) : "memory");
}

uint32_t raw_read_actlr_el3(void)
{
	uint64_t actlr_el3;

	__asm__ __volatile__("mrs %0, ACTLR_EL3\n\t" : "=r" (actlr_el3) :  : "memory");

	return actlr_el3;
}

void raw_write_actlr_el3(uint32_t actlr_el3)
{
	__asm__ __volatile__("msr ACTLR_EL3, %0\n\t" : : "r" ((uint64_t)actlr_el3) : "memory");
}

uint32_t raw_read_actlr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_actlr(el);
}

void raw_write_actlr_current(uint32_t actlr)
{
	uint32_t el = get_current_el();
	raw_write_actlr(actlr, el);
}

uint32_t raw_read_actlr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_actlr, actlr, uint32_t, el);
}

void raw_write_actlr(uint32_t actlr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_actlr, actlr, el);
}

/* AFSR0 */
uint32_t raw_read_afsr0_el1(void)
{
	uint64_t afsr0_el1;

	__asm__ __volatile__("mrs %0, AFSR0_EL1\n\t" : "=r" (afsr0_el1) :  : "memory");

	return afsr0_el1;
}

void raw_write_afsr0_el1(uint32_t afsr0_el1)
{
	__asm__ __volatile__("msr AFSR0_EL1, %0\n\t" : : "r" ((uint64_t)afsr0_el1) : "memory");
}

uint32_t raw_read_afsr0_el2(void)
{
	uint64_t afsr0_el2;

	__asm__ __volatile__("mrs %0, AFSR0_EL2\n\t" : "=r" (afsr0_el2) :  : "memory");

	return afsr0_el2;
}

void raw_write_afsr0_el2(uint32_t afsr0_el2)
{
	__asm__ __volatile__("msr AFSR0_EL2, %0\n\t" : : "r" ((uint64_t)afsr0_el2) : "memory");
}

uint32_t raw_read_afsr0_el3(void)
{
	uint64_t afsr0_el3;

	__asm__ __volatile__("mrs %0, AFSR0_EL3\n\t" : "=r" (afsr0_el3) :  : "memory");

	return afsr0_el3;
}

void raw_write_afsr0_el3(uint32_t afsr0_el3)
{
	__asm__ __volatile__("msr AFSR0_EL3, %0\n\t" : : "r" ((uint64_t)afsr0_el3) : "memory");
}

uint32_t raw_read_afsr0_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_afsr0(el);
}

void raw_write_afsr0_current(uint32_t afsr0)
{
	uint32_t el = get_current_el();
	raw_write_afsr0(afsr0, el);
}

uint32_t raw_read_afsr0(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_afsr0, afsr0, uint32_t, el);
}

void raw_write_afsr0(uint32_t afsr0, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_afsr0, afsr0, el);
}

/* AFSR1 */
uint32_t raw_read_afsr1_el1(void)
{
	uint64_t afsr1_el1;

	__asm__ __volatile__("mrs %0, AFSR1_EL1\n\t" : "=r" (afsr1_el1) :  : "memory");

	return afsr1_el1;
}

void raw_write_afsr1_el1(uint32_t afsr1_el1)
{
	__asm__ __volatile__("msr AFSR1_EL1, %0\n\t" : : "r" ((uint64_t)afsr1_el1) : "memory");
}

uint32_t raw_read_afsr1_el2(void)
{
	uint64_t afsr1_el2;

	__asm__ __volatile__("mrs %0, AFSR1_EL2\n\t" : "=r" (afsr1_el2) :  : "memory");

	return afsr1_el2;
}

void raw_write_afsr1_el2(uint32_t afsr1_el2)
{
	__asm__ __volatile__("msr AFSR1_EL2, %0\n\t" : : "r" ((uint64_t)afsr1_el2) : "memory");
}

uint32_t raw_read_afsr1_el3(void)
{
	uint64_t afsr1_el3;

	__asm__ __volatile__("mrs %0, AFSR1_EL3\n\t" : "=r" (afsr1_el3) :  : "memory");

	return afsr1_el3;
}

void raw_write_afsr1_el3(uint32_t afsr1_el3)
{
	__asm__ __volatile__("msr AFSR1_EL3, %0\n\t" : : "r" ((uint64_t)afsr1_el3) : "memory");
}

uint32_t raw_read_afsr1_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_afsr1(el);
}

void raw_write_afsr1_current(uint32_t afsr1)
{
	uint32_t el = get_current_el();
	raw_write_afsr1(afsr1, el);
}

uint32_t raw_read_afsr1(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_afsr1, afsr1, uint32_t, el);
}

void raw_write_afsr1(uint32_t afsr1, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_afsr1, afsr1, el);
}

/* AIDR */
uint32_t raw_read_aidr_el1(void)
{
	uint64_t aidr_el1;

	__asm__ __volatile__("mrs %0, AIDR_EL1\n\t" : "=r" (aidr_el1) :  : "memory");

	return aidr_el1;
}

/* AMAIR */
uint64_t raw_read_amair_el1(void)
{
	uint64_t amair_el1;

	__asm__ __volatile__("mrs %0, AMAIR_EL1\n\t" : "=r" (amair_el1) :  : "memory");

	return amair_el1;
}

void raw_write_amair_el1(uint64_t amair_el1)
{
	__asm__ __volatile__("msr AMAIR_EL1, %0\n\t" : : "r" (amair_el1) : "memory");
}

uint64_t raw_read_amair_el2(void)
{
	uint64_t amair_el2;

	__asm__ __volatile__("mrs %0, AMAIR_EL2\n\t" : "=r" (amair_el2) :  : "memory");

	return amair_el2;
}

void raw_write_amair_el2(uint64_t amair_el2)
{
	__asm__ __volatile__("msr AMAIR_EL2, %0\n\t" : : "r" (amair_el2) : "memory");
}

uint64_t raw_read_amair_el3(void)
{
	uint64_t amair_el3;

	__asm__ __volatile__("mrs %0, AMAIR_EL3\n\t" : "=r" (amair_el3) :  : "memory");

	return amair_el3;
}

void raw_write_amair_el3(uint64_t amair_el3)
{
	__asm__ __volatile__("msr AMAIR_EL3, %0\n\t" : : "r" (amair_el3) : "memory");
}

uint64_t raw_read_amair_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_amair(el);
}

void raw_write_amair_current(uint64_t amair)
{
	uint32_t el = get_current_el();
	raw_write_amair(amair, el);
}

uint64_t raw_read_amair(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_amair, amair, uint64_t, el);
}

void raw_write_amair(uint64_t amair, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_amair, amair, el);
}

/* CCSIDR */
uint32_t raw_read_ccsidr_el1(void)
{
	uint64_t ccsidr_el1;

	__asm__ __volatile__("mrs %0, CCSIDR_EL1\n\t" : "=r" (ccsidr_el1) :  : "memory");

	return ccsidr_el1;
}

/* CLIDR */
uint32_t raw_read_clidr_el1(void)
{
	uint64_t clidr_el1;

	__asm__ __volatile__("mrs %0, CLIDR_EL1\n\t" : "=r" (clidr_el1) :  : "memory");

	return clidr_el1;
}

/* CPACR */
uint32_t raw_read_cpacr_el1(void)
{
	uint64_t cpacr_el1;

	__asm__ __volatile__("mrs %0, CPACR_EL1\n\t" : "=r" (cpacr_el1) :  : "memory");

	return cpacr_el1;
}

void raw_write_cpacr_el1(uint32_t cpacr_el1)
{
	__asm__ __volatile__("msr CPACR_EL1, %0\n\t" : : "r" ((uint64_t)cpacr_el1) : "memory");
}

/* CPTR */
uint32_t raw_read_cptr_el2(void)
{
	uint64_t cptr_el2;

	__asm__ __volatile__("mrs %0, CPTR_EL2\n\t" : "=r" (cptr_el2) :  : "memory");

	return cptr_el2;
}

void raw_write_cptr_el2(uint32_t cptr_el2)
{
	__asm__ __volatile__("msr CPTR_EL2, %0\n\t" : : "r" ((uint64_t)cptr_el2) : "memory");
}

uint32_t raw_read_cptr_el3(void)
{
	uint64_t cptr_el3;

	__asm__ __volatile__("mrs %0, CPTR_EL3\n\t" : "=r" (cptr_el3) :  : "memory");

	return cptr_el3;
}

void raw_write_cptr_el3(uint32_t cptr_el3)
{
	__asm__ __volatile__("msr CPTR_EL3, %0\n\t" : : "r" ((uint64_t)cptr_el3) : "memory");
}

/* CSSELR */
uint32_t raw_read_csselr_el1(void)
{
	uint64_t csselr_el1;

	__asm__ __volatile__("mrs %0, CSSELR_EL1\n\t" : "=r" (csselr_el1) :  : "memory");

	return csselr_el1;
}

void raw_write_csselr_el1(uint32_t csselr_el1)
{
	__asm__ __volatile__("msr CSSELR_EL1, %0\n\t" : : "r" ((uint64_t)csselr_el1) : "memory");
}

/* CTR */
uint32_t raw_read_ctr_el0(void)
{
	uint64_t ctr_el0;

	__asm__ __volatile__("mrs %0, CTR_EL0\n\t" : "=r" (ctr_el0) :  : "memory");

	return ctr_el0;
}

/* ESR */
uint32_t raw_read_esr_el1(void)
{
	uint64_t esr_el1;

	__asm__ __volatile__("mrs %0, ESR_EL1\n\t" : "=r" (esr_el1) :  : "memory");

	return esr_el1;
}

void raw_write_esr_el1(uint32_t esr_el1)
{
	__asm__ __volatile__("msr ESR_EL1, %0\n\t" : : "r" ((uint64_t)esr_el1) : "memory");
}

uint32_t raw_read_esr_el2(void)
{
	uint64_t esr_el2;

	__asm__ __volatile__("mrs %0, ESR_EL2\n\t" : "=r" (esr_el2) :  : "memory");

	return esr_el2;
}

void raw_write_esr_el2(uint32_t esr_el2)
{
	__asm__ __volatile__("msr ESR_EL2, %0\n\t" : : "r" ((uint64_t)esr_el2) : "memory");
}

uint32_t raw_read_esr_el3(void)
{
	uint64_t esr_el3;

	__asm__ __volatile__("mrs %0, ESR_EL3\n\t" : "=r" (esr_el3) :  : "memory");

	return esr_el3;
}

void raw_write_esr_el3(uint32_t esr_el3)
{
	__asm__ __volatile__("msr ESR_EL3, %0\n\t" : : "r" ((uint64_t)esr_el3) : "memory");
}

uint32_t raw_read_esr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_esr(el);
}

void raw_write_esr_current(uint32_t esr)
{
	uint32_t el = get_current_el();
	raw_write_esr(esr, el);
}

uint32_t raw_read_esr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_esr, esr, uint32_t, el);
}

void raw_write_esr(uint32_t esr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_esr, esr, el);
}

/* FAR */
uint64_t raw_read_far_el1(void)
{
	uint64_t far_el1;

	__asm__ __volatile__("mrs %0, FAR_EL1\n\t" : "=r" (far_el1) :  : "memory");

	return far_el1;
}

void raw_write_far_el1(uint64_t far_el1)
{
	__asm__ __volatile__("msr FAR_EL1, %0\n\t" : : "r" (far_el1) : "memory");
}

uint64_t raw_read_far_el2(void)
{
	uint64_t far_el2;

	__asm__ __volatile__("mrs %0, FAR_EL2\n\t" : "=r" (far_el2) :  : "memory");

	return far_el2;
}

void raw_write_far_el2(uint64_t far_el2)
{
	__asm__ __volatile__("msr FAR_EL2, %0\n\t" : : "r" (far_el2) : "memory");
}

uint64_t raw_read_far_el3(void)
{
	uint64_t far_el3;

	__asm__ __volatile__("mrs %0, FAR_EL3\n\t" : "=r" (far_el3) :  : "memory");

	return far_el3;
}

void raw_write_far_el3(uint64_t far_el3)
{
	__asm__ __volatile__("msr FAR_EL3, %0\n\t" : : "r" (far_el3) : "memory");
}

uint64_t raw_read_far_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_far(el);
}

void raw_write_far_current(uint64_t far)
{
	uint32_t el = get_current_el();
	raw_write_far(far, el);
}

uint64_t raw_read_far(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_far, far, uint64_t, el);
}

void raw_write_far(uint64_t far, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_far, far, el);
}

/* HCR */
uint64_t raw_read_hcr_el2(void)
{
	uint64_t hcr_el2;

	__asm__ __volatile__("mrs %0, HCR_EL2\n\t" : "=r" (hcr_el2) :  : "memory");

	return hcr_el2;
}

void raw_write_hcr_el2(uint64_t hcr_el2)
{
	__asm__ __volatile__("msr HCR_EL2, %0\n\t" : : "r" (hcr_el2) : "memory");
}

/* AA64PFR0 */
uint64_t raw_read_aa64pfr0_el1(void)
{
	uint64_t aa64pfr0_el1;

	__asm__ __volatile__("mrs %0, ID_AA64PFR0_EL1\n\t" : "=r" (aa64pfr0_el1) :  : "memory");

	return aa64pfr0_el1;
}

/* MAIR */
uint64_t raw_read_mair_el1(void)
{
	uint64_t mair_el1;

	__asm__ __volatile__("mrs %0, MAIR_EL1\n\t" : "=r" (mair_el1) :  : "memory");

	return mair_el1;
}

void raw_write_mair_el1(uint64_t mair_el1)
{
	__asm__ __volatile__("msr MAIR_EL1, %0\n\t" : : "r" (mair_el1) : "memory");
}

uint64_t raw_read_mair_el2(void)
{
	uint64_t mair_el2;

	__asm__ __volatile__("mrs %0, MAIR_EL2\n\t" : "=r" (mair_el2) :  : "memory");

	return mair_el2;
}

void raw_write_mair_el2(uint64_t mair_el2)
{
	__asm__ __volatile__("msr MAIR_EL2, %0\n\t" : : "r" (mair_el2) : "memory");
}

uint64_t raw_read_mair_el3(void)
{
	uint64_t mair_el3;

	__asm__ __volatile__("mrs %0, MAIR_EL3\n\t" : "=r" (mair_el3) :  : "memory");

	return mair_el3;
}

void raw_write_mair_el3(uint64_t mair_el3)
{
	__asm__ __volatile__("msr MAIR_EL3, %0\n\t" : : "r" (mair_el3) : "memory");
}

uint64_t raw_read_mair_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_mair(el);
}

void raw_write_mair_current(uint64_t mair)
{
	uint32_t el = get_current_el();
	raw_write_mair(mair, el);
}

uint64_t raw_read_mair(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_mair, mair, uint64_t, el);
}

void raw_write_mair(uint64_t mair, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_mair, mair, el);
}

/* MIDR */
uint32_t raw_read_midr_el1(void)
{
	uint64_t midr_el1;

	__asm__ __volatile__("mrs %0, MIDR_EL1\n\t" : "=r" (midr_el1) :  : "memory");

	return midr_el1;
}

/* MPIDR */
uint64_t raw_read_mpidr_el1(void)
{
	uint64_t mpidr_el1;

	__asm__ __volatile__("mrs %0, MPIDR_EL1\n\t" : "=r" (mpidr_el1) :  : "memory");

	return mpidr_el1;
}

/* RMR */
uint32_t raw_read_rmr_el1(void)
{
	uint64_t rmr_el1;

	__asm__ __volatile__("mrs %0, RMR_EL1\n\t" : "=r" (rmr_el1) :  : "memory");

	return rmr_el1;
}

void raw_write_rmr_el1(uint32_t rmr_el1)
{
	__asm__ __volatile__("msr RMR_EL1, %0\n\t" : : "r" ((uint64_t)rmr_el1) : "memory");
}

uint32_t raw_read_rmr_el2(void)
{
	uint64_t rmr_el2;

	__asm__ __volatile__("mrs %0, RMR_EL2\n\t" : "=r" (rmr_el2) :  : "memory");

	return rmr_el2;
}

void raw_write_rmr_el2(uint32_t rmr_el2)
{
	__asm__ __volatile__("msr RMR_EL2, %0\n\t" : : "r" ((uint64_t)rmr_el2) : "memory");
}

uint32_t raw_read_rmr_el3(void)
{
	uint64_t rmr_el3;

	__asm__ __volatile__("mrs %0, RMR_EL3\n\t" : "=r" (rmr_el3) :  : "memory");

	return rmr_el3;
}

void raw_write_rmr_el3(uint32_t rmr_el3)
{
	__asm__ __volatile__("msr RMR_EL3, %0\n\t" : : "r" ((uint64_t)rmr_el3) : "memory");
}

uint32_t raw_read_rmr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_rmr(el);
}

void raw_write_rmr_current(uint32_t rmr)
{
	uint32_t el = get_current_el();
	raw_write_rmr(rmr, el);
}

uint32_t raw_read_rmr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_rmr, rmr, uint32_t, el);
}

void raw_write_rmr(uint32_t rmr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_rmr, rmr, el);
}

/* RVBAR */
uint64_t raw_read_rvbar_el1(void)
{
	uint64_t rvbar_el1;

	__asm__ __volatile__("mrs %0, RVBAR_EL1\n\t" : "=r" (rvbar_el1) :  : "memory");

	return rvbar_el1;
}

void raw_write_rvbar_el1(uint64_t rvbar_el1)
{
	__asm__ __volatile__("msr RVBAR_EL1, %0\n\t" : : "r" (rvbar_el1) : "memory");
}

uint64_t raw_read_rvbar_el2(void)
{
	uint64_t rvbar_el2;

	__asm__ __volatile__("mrs %0, RVBAR_EL2\n\t" : "=r" (rvbar_el2) :  : "memory");

	return rvbar_el2;
}

void raw_write_rvbar_el2(uint64_t rvbar_el2)
{
	__asm__ __volatile__("msr RVBAR_EL2, %0\n\t" : : "r" (rvbar_el2) : "memory");
}

uint64_t raw_read_rvbar_el3(void)
{
	uint64_t rvbar_el3;

	__asm__ __volatile__("mrs %0, RVBAR_EL3\n\t" : "=r" (rvbar_el3) :  : "memory");

	return rvbar_el3;
}

void raw_write_rvbar_el3(uint64_t rvbar_el3)
{
	__asm__ __volatile__("msr RVBAR_EL3, %0\n\t" : : "r" (rvbar_el3) : "memory");
}

uint64_t raw_read_rvbar_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_rvbar(el);
}

void raw_write_rvbar_current(uint64_t rvbar)
{
	uint32_t el = get_current_el();
	raw_write_rvbar(rvbar, el);
}

uint64_t raw_read_rvbar(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_rvbar, rvbar, uint64_t, el);
}

void raw_write_rvbar(uint64_t rvbar, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_rvbar, rvbar, el);
}

/* Scr */
uint32_t raw_read_scr_el3(void)
{
	uint64_t scr_el3;

	__asm__ __volatile__("mrs %0, SCR_EL3\n\t" : "=r" (scr_el3) :  : "memory");

	return scr_el3;
}

void raw_write_scr_el3(uint32_t scr_el3)
{
	__asm__ __volatile__("msr SCR_EL3, %0\n\t" : : "r" ((uint64_t)scr_el3) : "memory");
}

/* SCTLR */
uint32_t raw_read_sctlr_el1(void)
{
	uint64_t sctlr_el1;

	__asm__ __volatile__("mrs %0, SCTLR_EL1\n\t" : "=r" (sctlr_el1) :  : "memory");

	return sctlr_el1;
}

void raw_write_sctlr_el1(uint32_t sctlr_el1)
{
	__asm__ __volatile__("msr SCTLR_EL1, %0\n\t" : : "r" ((uint64_t)sctlr_el1) : "memory");
}

uint32_t raw_read_sctlr_el2(void)
{
	uint64_t sctlr_el2;

	__asm__ __volatile__("mrs %0, SCTLR_EL2\n\t" : "=r" (sctlr_el2) :  : "memory");

	return sctlr_el2;
}

void raw_write_sctlr_el2(uint32_t sctlr_el2)
{
	__asm__ __volatile__("msr SCTLR_EL2, %0\n\t" : : "r" ((uint64_t)sctlr_el2) : "memory");
}

uint32_t raw_read_sctlr_el3(void)
{
	uint64_t sctlr_el3;

	__asm__ __volatile__("mrs %0, SCTLR_EL3\n\t" : "=r" (sctlr_el3) :  : "memory");

	return sctlr_el3;
}

void raw_write_sctlr_el3(uint32_t sctlr_el3)
{
	__asm__ __volatile__("msr SCTLR_EL3, %0\n\t" : : "r" ((uint64_t)sctlr_el3) : "memory");
}

uint32_t raw_read_sctlr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_sctlr(el);
}

void raw_write_sctlr_current(uint32_t sctlr)
{
	uint32_t el = get_current_el();
	raw_write_sctlr(sctlr, el);
}

uint32_t raw_read_sctlr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_sctlr, sctlr, uint32_t, el);
}

void raw_write_sctlr(uint32_t sctlr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_sctlr, sctlr, el);
}

/* TCR */
uint64_t raw_read_tcr_el1(void)
{
	uint64_t tcr_el1;

	__asm__ __volatile__("mrs %0, TCR_EL1\n\t" : "=r" (tcr_el1) :  : "memory");

	return tcr_el1;
}

void raw_write_tcr_el1(uint64_t tcr_el1)
{
	__asm__ __volatile__("msr TCR_EL1, %0\n\t" : : "r" (tcr_el1) : "memory");
}

uint32_t raw_read_tcr_el2(void)
{
	uint64_t tcr_el2;

	__asm__ __volatile__("mrs %0, TCR_EL2\n\t" : "=r" (tcr_el2) :  : "memory");

	return tcr_el2;
}

void raw_write_tcr_el2(uint32_t tcr_el2)
{
	__asm__ __volatile__("msr TCR_EL2, %0\n\t" : : "r" ((uint64_t)tcr_el2) : "memory");
}

uint32_t raw_read_tcr_el3(void)
{
	uint64_t tcr_el3;

	__asm__ __volatile__("mrs %0, TCR_EL3\n\t" : "=r" (tcr_el3) :  : "memory");

	return tcr_el3;
}

void raw_write_tcr_el3(uint32_t tcr_el3)
{
	__asm__ __volatile__("msr TCR_EL3, %0\n\t" : : "r" ((uint64_t)tcr_el3) : "memory");
}


/*
 * IMPORTANT: TCR_EL1 is 64-bit whereas TCR_EL2 and TCR_EL3 are 32-bit. Thus,
 * 64-bit is used to read/write for tcr_current. tcr_el2 and tcr_el3 handle them
 * with appropriate 32-bit types.
 */
uint64_t raw_read_tcr_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_tcr(el);
}

void raw_write_tcr_current(uint64_t tcr)
{
	uint32_t el = get_current_el();
	raw_write_tcr(tcr, el);
}

uint64_t raw_read_tcr(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_tcr, tcr, uint64_t, el);
}

void raw_write_tcr(uint64_t tcr, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_tcr, tcr, el);
}

/* TTBR0 */
uint64_t raw_read_ttbr0_el1(void)
{
	uint64_t ttbr0_el1;

	__asm__ __volatile__("mrs %0, TTBR0_EL1\n\t" : "=r" (ttbr0_el1) :  : "memory");

	return ttbr0_el1;
}

void raw_write_ttbr0_el1(uint64_t ttbr0_el1)
{
	__asm__ __volatile__("msr TTBR0_EL1, %0\n\t" : : "r" (ttbr0_el1) : "memory");
}

uint64_t raw_read_ttbr0_el2(void)
{
	uint64_t ttbr0_el2;

	__asm__ __volatile__("mrs %0, TTBR0_EL2\n\t" : "=r" (ttbr0_el2) :  : "memory");

	return ttbr0_el2;
}

void raw_write_ttbr0_el2(uint64_t ttbr0_el2)
{
	__asm__ __volatile__("msr TTBR0_EL2, %0\n\t" : : "r" (ttbr0_el2) : "memory");
}

uint64_t raw_read_ttbr0_el3(void)
{
	uint64_t ttbr0_el3;

	__asm__ __volatile__("mrs %0, TTBR0_EL3\n\t" : "=r" (ttbr0_el3) :  : "memory");

	return ttbr0_el3;
}

void raw_write_ttbr0_el3(uint64_t ttbr0_el3)
{
	__asm__ __volatile__("msr TTBR0_EL3, %0\n\t" : : "r" (ttbr0_el3) : "memory");
}

uint64_t raw_read_ttbr0_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_ttbr0(el);
}

void raw_write_ttbr0_current(uint64_t ttbr0)
{
	uint32_t el = get_current_el();
	raw_write_ttbr0(ttbr0, el);
}

uint64_t raw_read_ttbr0(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_ttbr0, ttbr0, uint64_t, el);
}

void raw_write_ttbr0(uint64_t ttbr0, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_ttbr0, ttbr0, el);
}

/* TTBR1 */
uint64_t raw_read_ttbr1_el1(void)
{
	uint64_t ttbr1_el1;

	__asm__ __volatile__("mrs %0, TTBR1_EL1\n\t" : "=r" (ttbr1_el1) :  : "memory");

	return ttbr1_el1;
}

void raw_write_ttbr1_el1(uint64_t ttbr1_el1)
{
	__asm__ __volatile__("msr TTBR1_EL1, %0\n\t" : : "r" (ttbr1_el1) : "memory");
}

/* VBAR */
uint64_t raw_read_vbar_el1(void)
{
	uint64_t vbar_el1;

	__asm__ __volatile__("mrs %0, VBAR_EL1\n\t" : "=r" (vbar_el1) :  : "memory");

	return vbar_el1;
}

void raw_write_vbar_el1(uint64_t vbar_el1)
{
	__asm__ __volatile__("msr VBAR_EL1, %0\n\t" : : "r" (vbar_el1) : "memory");
}

uint64_t raw_read_vbar_el2(void)
{
	uint64_t vbar_el2;

	__asm__ __volatile__("mrs %0, VBAR_EL2\n\t" : "=r" (vbar_el2) :  : "memory");

	return vbar_el2;
}

void raw_write_vbar_el2(uint64_t vbar_el2)
{
	__asm__ __volatile__("msr VBAR_EL2, %0\n\t" : : "r" (vbar_el2) : "memory");
}

uint64_t raw_read_vbar_el3(void)
{
	uint64_t vbar_el3;

	__asm__ __volatile__("mrs %0, VBAR_EL3\n\t" : "=r" (vbar_el3) :  : "memory");

	return vbar_el3;
}

void raw_write_vbar_el3(uint64_t vbar_el3)
{
	__asm__ __volatile__("msr VBAR_EL3, %0\n\t" : : "r" (vbar_el3) : "memory");
}

uint64_t raw_read_vbar_current(void)
{
	uint32_t el = get_current_el();
	return raw_read_vbar(el);
}

void raw_write_vbar_current(uint64_t vbar)
{
	uint32_t el = get_current_el();
	raw_write_vbar(vbar, el);
}

uint64_t raw_read_vbar(uint32_t el)
{
	SWITCH_CASE_READ(raw_read_vbar, vbar, uint64_t, el);
}

void raw_write_vbar(uint64_t vbar, uint32_t el)
{
	SWITCH_CASE_WRITE(raw_write_vbar, vbar, el);
}

uint32_t raw_read_cntfrq_el0(void)
{
	uint64_t cntfrq_el0;

	__asm__ __volatile__("mrs %0, CNTFRQ_EL0\n\t" : "=r" (cntfrq_el0) : : "memory");
	return cntfrq_el0;
}

void raw_write_cntfrq_el0(uint32_t cntfrq_el0)
{
	__asm__ __volatile__("msr CNTFRQ_EL0, %0\n\t" : : "r" ((uint64_t)cntfrq_el0) : "memory");
}

uint64_t raw_read_cntpct_el0(void)
{
	uint64_t cntpct_el0;

	__asm__ __volatile__("mrs %0, CNTPCT_EL0\n\t" : "=r" (cntpct_el0) : : "memory");
	return cntpct_el0;
}
