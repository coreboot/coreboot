/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _PPC_H
#define _PPC_H

#define BIG_ENDIAN
#define RODATA __attribute__ ((__section__ (".rodata"))) 

/* stringify is needed for macro expansion */
#define stringify(x) #x

#define mfdcr(reg)	({unsigned int result; \
			__asm__ volatile("mfdcr %0, " stringify(reg) \
				  	 : "=r" (result)); result;})

#define mtdcr(reg, v)	asm volatile("mtdcr " stringify(reg) ",%0" \
			: : "r" (v))

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
#endif
