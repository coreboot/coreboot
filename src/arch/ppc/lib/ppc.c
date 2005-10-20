/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include "ppc.h"
#include "ppcreg.h"

unsigned ppc_getmsr(void)
{
	unsigned result;   
	__asm__ volatile ("mfmsr %0" : "=r" (result));
	return result;
}

unsigned ppc_gethid0(void)
{
	unsigned result;
	__asm__ volatile ("mfspr %0,1008" : "=r" (result));
	return result;
}

unsigned ppc_gethid1(void)
{
	unsigned result;
	__asm__ volatile ("mfspr %0,1009" : "=r" (result));
	return result;
}

void ppc_sethid0(unsigned value)
{
	__asm__ volatile ("mtspr 1008,%0" : : "r" (value));
}

unsigned ppc_getpvr(void)
{
	unsigned result;
	__asm__("mfspr %0, 287" : "=r" (result));
	return result;
}

void ppc_setmsr(unsigned value)
{
	__asm__ volatile ("mtmsr %0; sync" :: "r" (value));   
}

void ppc_set1015(unsigned value)
{
	__asm__ volatile ("mtspr 1015,%0" : : "r" (value));
}

