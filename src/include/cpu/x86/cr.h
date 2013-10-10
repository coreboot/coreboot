
/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef CPU_X86_CR_H
#define CPU_X86_CR_H

#if !defined(__ASSEMBLER__)

#include <stdint.h>
#include <arch/cpu.h>

/* ROMCC apparently chokes certain clobber registers. */
#if defined(__ROMCC__)
#define COMPILER_BARRIER
#else
#define COMPILER_BARRIER "memory"
#endif

static alwaysinline uint32_t read_cr0(void)
{
	uint32_t value;
	__asm__ __volatile__ (
		"mov %%cr0, %0"
		: "=r" (value)
		:
		: COMPILER_BARRIER
	);
	return value;
}

static alwaysinline void write_cr0(uint32_t data)
{
	__asm__ __volatile__ (
		"mov %0, %%cr0"
		:
		: "r" (data)
		: COMPILER_BARRIER
	);
}

static alwaysinline uint32_t read_cr4(void)
{
	uint32_t value;
	__asm__ __volatile__ (
		"mov %%cr4, %0"
		: "=r" (value)
		:
		: COMPILER_BARRIER
	);
	return value;
}

static alwaysinline void write_cr4(uint32_t data)
{
	__asm__ __volatile__ (
		"mov %0, %%cr4"
		:
		: "r" (data)
		: COMPILER_BARRIER
	);
}

#endif /* !defined(__ASSEMBLER__) */

/* CR0 flags */
#define CR0_PE		(1 <<  0)
#define CR0_MP		(1 <<  1)
#define CR0_EM		(1 <<  2)
#define CR0_TS		(1 <<  3)
#define CR0_ET		(1 <<  4)
#define CR0_NE		(1 <<  5)
#define CR0_WP		(1 << 16)
#define CR0_AM		(1 << 18)
#define CR0_NW		(1 << 29)
#define CR0_CD		(1 << 30)
#define CR0_PG		(1 << 31)

/* CR4 flags */
#define CR4_VME		(1 <<  0)
#define CR4_PVI		(1 <<  1)
#define CR4_TSD		(1 <<  2)
#define CR4_DE		(1 <<  3)
#define CR4_PSE		(1 <<  4)
#define CR4_PAE		(1 <<  5)
#define CR4_MCE		(1 <<  6)
#define CR4_PGE		(1 <<  7)
#define CR4_PCE		(1 <<  8)
#define CR4_OSFXSR	(1 <<  9)
#define CR4_OSXMMEXCPT	(1 << 10)
#define CR4_VMXE	(1 << 13)
#define CR4_SMXE	(1 << 14)
#define CR4_FSGSBASE	(1 << 16)
#define CR4_PCIDE	(1 << 17)
#define CR4_OSXSAVE	(1 << 18)
#define CR4_SMEP	(1 << 20)

#endif /* CPU_X86_CR_H */
