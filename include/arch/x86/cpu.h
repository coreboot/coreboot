/*
 * This file is part of the LinuxBIOS project.
 *
 * It is based on the Linux kernel files include/asm-i386/processor.h
 * and arch/i386/kernel/cpu/mtrr/state.c.
 *
 * Modifications are:
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 */

/*
 * include/asm-i386/processor.h
 *
 * Copyright (C) 1994 Linus Torvalds
 */

#ifndef ARCH_X86_CPU_H
#define ARCH_X86_CPU_H

#include <types.h>
#include <device/device.h>

#define X86_VENDOR_INTEL	0
#define X86_VENDOR_CYRIX	1
#define X86_VENDOR_AMD		2
#define X86_VENDOR_UMC		3
#define X86_VENDOR_NEXGEN	4
#define X86_VENDOR_CENTAUR	5
#define X86_VENDOR_RISE		6
#define X86_VENDOR_TRANSMETA	7
#define X86_VENDOR_NSC		8
#define X86_VENDOR_NUM		9
#define X86_VENDOR_UNKNOWN	0xff

/*
 * EFLAGS bits
 */
#define X86_EFLAGS_CF	0x00000001 /* Carry Flag */
#define X86_EFLAGS_PF	0x00000004 /* Parity Flag */
#define X86_EFLAGS_AF	0x00000010 /* Auxillary carry Flag */
#define X86_EFLAGS_ZF	0x00000040 /* Zero Flag */
#define X86_EFLAGS_SF	0x00000080 /* Sign Flag */
#define X86_EFLAGS_TF	0x00000100 /* Trap Flag */
#define X86_EFLAGS_IF	0x00000200 /* Interrupt Flag */
#define X86_EFLAGS_DF	0x00000400 /* Direction Flag */
#define X86_EFLAGS_OF	0x00000800 /* Overflow Flag */
#define X86_EFLAGS_IOPL	0x00003000 /* IOPL mask */
#define X86_EFLAGS_NT	0x00004000 /* Nested Task */
#define X86_EFLAGS_RF	0x00010000 /* Resume Flag */
#define X86_EFLAGS_VM	0x00020000 /* Virtual Mode */
#define X86_EFLAGS_AC	0x00040000 /* Alignment Check */
#define X86_EFLAGS_VIF	0x00080000 /* Virtual Interrupt Flag */
#define X86_EFLAGS_VIP	0x00100000 /* Virtual Interrupt Pending */
#define X86_EFLAGS_ID	0x00200000 /* CPUID detection flag */

/**
 * A struct holding the result of the CPUID intruction.
 */
struct cpuid_result {
	u32 eax;
	u32 ebx;
	u32 ecx;
	u32 edx;
};

struct cpu_info {
	struct device cpu;
	unsigned long index;	/* TODO: u32? u64? */
};

struct cpuinfo_x86 {
	u8 x86;			/* CPU family */
	u8 x86_vendor;		/* CPU vendor */
	u8 x86_model;
	u8 x86_mask;
};

/**
 * Generic CPUID function.
 *
 * @param op TODO
 * @return A 'struct cpuid_result' which contains the values for eax, ebx,
 *         ecx, and edx as returned by the CPUID instruction.
 */
static inline struct cpuid_result cpuid(u32 op)
{
	struct cpuid_result r;

	/* Clear %ecx since some CPUs (Cyrix MII) do not set or clear %ecx,
	 * resulting in stale register contents being returned.
	 */
	r.eax = op;
	r.ecx = 0;

	/* ecx is often an input as well as an output. */
	__asm__("cpuid"
		: "=a" (r.eax),
		  "=b" (r.ebx),
		  "=c" (r.ecx),
		  "=d" (r.edx)
		: "0" (r.eax), "2" (r.ecx));

	return r;
}

/*
 * CPUID functions returning a single datum.
 */
static inline u32 cpuid_eax(u32 op) { return cpuid(op).eax; } 
static inline u32 cpuid_ebx(u32 op) { return cpuid(op).ebx; }
static inline u32 cpuid_ecx(u32 op) { return cpuid(op).ecx; }
static inline u32 cpuid_edx(u32 op) { return cpuid(op).edx; }

/**
 * Get the u32 CPUINFO information into a struct cpuinfo_x86.
 *
 * @param c Pointer to a 'cpuinfo_x86' struct.
 * @param tfms TODO
 */
static void inline get_fms(struct cpuinfo_x86 *c, u32 tfms)
{
	c->x86 = (tfms >> 8) & 0xf;
	c->x86_model = (tfms >> 4) & 0xf;
	c->x86_mask = tfms & 0xf;
	if (c->x86 == 0xf)
		c->x86 += (tfms >> 20) & 0xff;
	if (c->x86 >= 0x6)
		c->x86_model += ((tfms >> 16) & 0xf) << 4;
}

static inline unsigned long read_cr0(void)
{
	unsigned long cr0;
	asm volatile("movl %%cr0, %0" : "=r" (cr0));
	return cr0;
}

static inline void write_cr0(unsigned long cr0)
{
	asm volatile("movl %0, %%cr0" : : "r" (cr0));
}

static inline void invd(void)
{
	asm volatile("invd" : : : "memory");
}

static inline void wbinvd(void)
{
	asm volatile("wbinvd");
}

static inline void enable_cache(void)
{
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 &= 0x9fffffff;
	write_cr0(cr0);
}

/**
 * Disable and write back the cache.
 */
static inline void disable_cache(void)
{
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 |= 0x40000000;
	wbinvd();
	write_cr0(cr0);
	wbinvd();
}

/**
 * REP NOP (PAUSE) is a good thing to insert into busy-wait loops.
 */
static inline void cpu_relax(void)
{
	__asm__ __volatile__("rep;nop" : : : "memory");
}

/* Random other functions. These are not architecture-specific, except they
 * really are in many ways. Seperate the PC from the "X86" is hard.
 */
void uart_init(void);
void rtc_init(int invalid);
void isa_dma_init(void);

#endif /* ARCH_X86_CPU_H */
