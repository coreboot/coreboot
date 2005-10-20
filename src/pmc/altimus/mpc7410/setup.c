/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include "ppc.h"
#include "ppcreg.h"
#include "ppc74xx.h"

extern void ppc_init_float_registers(const double *);
/*RODATA static const double dummy_float = 1.0;*/
static const double dummy_float = 1.0;

#define HID0_DCACHE HID0_DCE
#define MSR_DATA MSR_DR

void ppc_setup_cpu(int icache)
{
	int type = ppc_getpvr() >> 16;
	int version = ppc_getpvr() & 0xffff;

	if (type == 0xc) 
	{
		if (version == 0x0200)
			ppc_set1015(0x19000004);
		else if (((version & 0xff00) == 0x0200) && 
			(version != 0x0209))
			ppc_set1015(0x01000000);
	}
	if (icache)
	{
		ppc_sethid0(HID0_NHR | HID0_BHT | HID0_ICE | HID0_ICFI 
			| HID0_BTIC | HID0_DCACHE);
		ppc_sethid0(HID0_DPM | HID0_NHR | HID0_BHT | HID0_ICE 
			| HID0_BTIC | HID0_DCACHE);        
	}
	else
	{
		ppc_sethid0(HID0_DPM | HID0_NHR | HID0_BHT | HID0_BTIC 
			| HID0_DCACHE);
	}
#if 1
	/* if (type == 8 || type == 12) */
	{
		ppc_setmsr(MSR_FP | MSR_DATA);
		ppc_init_float_registers(&dummy_float);
	}
#endif
}

void ppc_enable_dcache(void)
{
	/*
	 * Already enabled in crt0.S
	 */
#if 0
	unsigned hid0 = ppc_gethid0();
	ppc_sethid0(hid0 | HID0_DCFI | HID0_DCE);
	ppc_sethid0(hid0 | HID0_DCE);
#endif
}

void ppc_disable_dcache(void)
{
	unsigned hid0 = ppc_gethid0();
	ppc_sethid0(hid0 & ~HID0_DCE);
}

void ppc_enable_mmu(void)
{
	unsigned msr = ppc_getmsr();
	ppc_setmsr(msr | MSR_DR | MSR_IR); 
}

void make_coherent(void *base, unsigned length)
{
	unsigned hid0 = ppc_gethid0();

	if (hid0 & HID0_DCE)
	{
		unsigned i;
		unsigned offset = 0x1f & (unsigned) base;
		unsigned adjusted_base = (unsigned) base & ~0x1f;
		for(i = 0; i < length + offset; i+= 32)
			__asm__ volatile ("dcbf %1,%0" : : "r" (adjusted_base), "r" (i));
		if (hid0 & HID0_ICE)
		for(i = 0; i < length + offset; i+= 32)
			__asm__ volatile ("icbi %1,%0" : : "r" (adjusted_base), "r" (i));
	}
}
