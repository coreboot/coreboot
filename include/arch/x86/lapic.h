/*
 * This file is part of the coreboot project.
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2005 Stefan Reinauer <stepan@coresystems.de>
 * Copyright (C) 2007 AMD Marc Jones
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
#ifndef CPU_X86_LAPIC_H
#define CPU_X86_LAPIC_H

#include <cpu.h>
#include <lapic_def.h>
#include <msr.h>

/* See if I need to initialize the local apic */
#if defined(CONFIG_SMP) || defined(CONFIG_IOAPIC)
#  define NEED_LAPIC 1
#endif

static inline __attribute__((always_inline)) unsigned long lapic_read(unsigned long reg)
{
	return *((volatile unsigned long *)(LAPIC_DEFAULT_BASE+reg));
}

static inline __attribute__((always_inline)) void lapic_write(unsigned long reg, unsigned long v)
{
	*((volatile unsigned long *)(LAPIC_DEFAULT_BASE+reg)) = v;
}

static inline __attribute__((always_inline)) void lapic_wait_icr_idle(void)
{
	do { } while ( lapic_read( LAPIC_ICR ) & LAPIC_ICR_BUSY );
}



static inline void enable_lapic(void)
{

	struct msr msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.hi &= 0xffffff00;
	msr.lo &= 0x000007ff;
	msr.lo |= LAPIC_DEFAULT_BASE | (1 << 11);
	wrmsr(LAPIC_BASE_MSR, msr);
}

static inline void disable_lapic(void)
{
	struct msr msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	msr.lo &= ~(1 << 11);
	wrmsr(LAPIC_BASE_MSR, msr);
}

static inline __attribute__((always_inline)) unsigned long lapicid(void)
{
	return lapic_read(LAPIC_ID) >> 24;
}

static inline __attribute__((always_inline)) void stop_this_cpu(void)
{

	/* Called by an AP when it is ready to halt and wait for a new task */
	for(;;) {
		hlt();
	}
}

#define xchg(ptr,v) ((__typeof__(*(ptr)))__xchg((unsigned long)(v),(ptr),sizeof(*(ptr))))

struct __xchg_dummy { unsigned long a[100]; };
#define __xg(x) ((struct __xchg_dummy *)(x))

/*
 * Note: no "lock" prefix even on SMP: xchg always implies lock anyway
 * Note 2: xchg has side effect, so that attribute volatile is necessary,
 *	  but generally the primitive is invalid, *ptr is output argument. --ANK
 */
static inline unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
	switch (size) {
		case 1:
			__asm__ __volatile__("xchgb %b0,%1"
				:"=q" (x)
				:"m" (*__xg(ptr)), "0" (x)
				:"memory");
			break;
		case 2:
			__asm__ __volatile__("xchgw %w0,%1"
				:"=r" (x)
				:"m" (*__xg(ptr)), "0" (x)
				:"memory");
			break;
		case 4:
			__asm__ __volatile__("xchgl %0,%1"
				:"=r" (x)
				:"m" (*__xg(ptr)), "0" (x)
				:"memory");
			break;
	}
	return x;
}


extern inline void lapic_write_atomic(unsigned long reg, unsigned long v)
{
	xchg((volatile unsigned long *)(LAPIC_DEFAULT_BASE+reg), v);
}


#ifdef CONFIG_X86_GOOD_APIC
# define FORCE_READ_AROUND_WRITE 0
# define lapic_read_around(x) lapic_read(x)
# define lapic_write_around(x,y) lapic_write((x),(y))
#else
# define FORCE_READ_AROUND_WRITE 1
# define lapic_read_around(x) lapic_read(x)
# define lapic_write_around(x,y) lapic_write_atomic((x),(y))
#endif

static inline int lapic_remote_read(int apicid, int reg, unsigned long *pvalue)
{
	int timeout;
	unsigned long status;
	int result;
	lapic_wait_icr_idle();
	lapic_write_around(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));
	lapic_write_around(LAPIC_ICR, LAPIC_DM_REMRD | (reg >> 4));
	timeout = 0;
	do {
#if 0
		udelay(100);
#endif
		status = lapic_read(LAPIC_ICR) & LAPIC_ICR_RR_MASK;
	} while (status == LAPIC_ICR_RR_INPROG && timeout++ < 1000);

	result = -1;
	if (status == LAPIC_ICR_RR_VALID) {
		*pvalue = lapic_read(LAPIC_RRR);
		result = 0;
	}
	return result;
}


void setup_lapic(void);


#ifdef CONFIG_SMP
struct device;
int start_cpu(struct device *cpu);

#endif /* CONFIG_SMP */

#endif /* CPU_X86_LAPIC_H */
