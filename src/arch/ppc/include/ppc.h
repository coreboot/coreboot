/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#ifndef _PPC_H
#define _PPC_H

#define BIG_ENDIAN
#define RODATA __attribute__ ((__section__ (".rodata"))) 

/* stringify is needed for macro expansion */
#define stringify(x) #x

#define ppc_getdcr(reg) ({unsigned int result; \
			__asm__ volatile("mfdcr %0, " stringify(reg) \
			: "=r" (result)); result;})

extern unsigned ppc_getmsr(void);
extern unsigned ppc_gethid0(void);
extern unsigned ppc_gethid1(void);
extern unsigned ppc_getpvr(void);
extern void ppc_setmsr(unsigned);
extern void ppc_sethid0(unsigned);
extern void ppc_set1015(unsigned);

extern void ppc_init_float_registers(const double *);

/* Do CPU specific setup, with optional icache */
extern void ppc_setup_cpu(int icache);

extern void ppc_enable_dcache(void);
extern void ppc_disable_dcache(void);
extern void ppc_enable_mmu(void);

/* Describe which sort of ppc CPU I am */
extern void ppc_identify(void);

/* CPU clock speed in ticks per second */
extern unsigned long get_clock_speed();
#endif
