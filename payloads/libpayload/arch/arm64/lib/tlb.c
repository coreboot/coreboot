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
 * tlb.c: System intructions for TLB maintenance.
 * Reference: ARM Architecture Reference Manual, ARMv8-A edition
 */

#include <stdint.h>

#include <arch/lib_helpers.h>

/* TLBIALL */
void tlbiall_el1(void)
{
	__asm__ __volatile__("tlbi alle1\n\t" : : : "memory");
}

void tlbiall_el2(void)
{
	__asm__ __volatile__("tlbi alle2\n\t" : : : "memory");
}

void tlbiall_el3(void)
{
	__asm__ __volatile__("tlbi alle3\n\t" : : : "memory");
}

void tlbiall_current(void)
{
	uint32_t el = get_current_el();
	tlbiall(el);
}

void tlbiall(uint32_t el)
{
	SWITCH_CASE_TLBI(tlbiall, el);
}

/* TLBIALLIS */
void tlbiallis_el1(void)
{
	__asm__ __volatile__("tlbi alle1is\n\t" : : : "memory");
}

void tlbiallis_el2(void)
{
	__asm__ __volatile__("tlbi alle2is\n\t" : : : "memory");
}

void tlbiallis_el3(void)
{
	__asm__ __volatile__("tlbi alle3is\n\t" : : : "memory");
}

void tlbiallis_current(void)
{
	uint32_t el = get_current_el();
	tlbiallis(el);
}

void tlbiallis(uint32_t el)
{
	SWITCH_CASE_TLBI(tlbiallis, el);
}

/* TLBIVAA */
void tlbivaa_el1(uint64_t va)
{
	__asm__ __volatile__("tlbi vaae1, %0\n\t" : : "r" (va) : "memory");
}
