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
