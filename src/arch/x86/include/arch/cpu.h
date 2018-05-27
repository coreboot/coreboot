/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include <stdint.h>
#include <stddef.h>
#include <rules.h>

/*
 * EFLAGS bits
 */
#define X86_EFLAGS_CF	0x00000001 /* Carry Flag */
#define X86_EFLAGS_PF	0x00000004 /* Parity Flag */
#define X86_EFLAGS_AF	0x00000010 /* Auxiliary carry Flag */
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

struct cpuid_result {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};

/*
 * Generic CPUID function
 */
static inline struct cpuid_result cpuid(int op)
{
	struct cpuid_result result;
	asm volatile(
		"mov %%ebx, %%edi;"
		"cpuid;"
		"mov %%ebx, %%esi;"
		"mov %%edi, %%ebx;"
		: "=a" (result.eax),
		  "=S" (result.ebx),
		  "=c" (result.ecx),
		  "=d" (result.edx)
		: "0" (op)
		: "edi");
	return result;
}

/*
 * Generic Extended CPUID function
 */
static inline struct cpuid_result cpuid_ext(int op, unsigned int ecx)
{
	struct cpuid_result result;
	asm volatile(
		"mov %%ebx, %%edi;"
		"cpuid;"
		"mov %%ebx, %%esi;"
		"mov %%edi, %%ebx;"
		: "=a" (result.eax),
		  "=S" (result.ebx),
		  "=c" (result.ecx),
		  "=d" (result.edx)
		: "0" (op), "2" (ecx)
		: "edi");
	return result;
}

/*
 * CPUID functions returning a single datum
 */
static inline unsigned int cpuid_eax(unsigned int op)
{
	unsigned int eax;

	__asm__("mov %%ebx, %%edi;"
		"cpuid;"
		"mov %%edi, %%ebx;"
		: "=a" (eax)
		: "0" (op)
		: "ecx", "edx", "edi");
	return eax;
}

static inline unsigned int cpuid_ebx(unsigned int op)
{
	unsigned int eax, ebx;

	__asm__("mov %%ebx, %%edi;"
		"cpuid;"
		"mov %%ebx, %%esi;"
		"mov %%edi, %%ebx;"
		: "=a" (eax), "=S" (ebx)
		: "0" (op)
		: "ecx", "edx", "edi");
	return ebx;
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
	unsigned int eax, ecx;

	__asm__("mov %%ebx, %%edi;"
		"cpuid;"
		"mov %%edi, %%ebx;"
		: "=a" (eax), "=c" (ecx)
		: "0" (op)
		: "edx", "edi");
	return ecx;
}

static inline unsigned int cpuid_edx(unsigned int op)
{
	unsigned int eax, edx;

	__asm__("mov %%ebx, %%edi;"
		"cpuid;"
		"mov %%edi, %%ebx;"
		: "=a" (eax), "=d" (edx)
		: "0" (op)
		: "ecx", "edi");
	return edx;
}

#define X86_VENDOR_INVALID    0
#define X86_VENDOR_INTEL      1
#define X86_VENDOR_CYRIX      2
#define X86_VENDOR_AMD        3
#define X86_VENDOR_UMC        4
#define X86_VENDOR_NEXGEN     5
#define X86_VENDOR_CENTAUR    6
#define X86_VENDOR_RISE       7
#define X86_VENDOR_TRANSMETA  8
#define X86_VENDOR_NSC        9
#define X86_VENDOR_SIS       10
#define X86_VENDOR_ANY     0xfe
#define X86_VENDOR_UNKNOWN 0xff

#define CPUID_FEATURE_PAE (1 << 6)
#define CPUID_FEATURE_PSE36 (1 << 17)

int cpu_cpuid_extended_level(void);
int cpu_have_cpuid(void);

void smm_init(void);
void smm_init_completion(void);
void smm_lock(void);
void smm_setup_structures(void *gnvs, void *tcg, void *smi1);

#ifndef __SIMPLE_DEVICE__

struct device;

struct cpu_device_id {
	unsigned int vendor;
	unsigned int device;
};

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
	struct acpi_cstate *cstates;
};

struct cpu_driver *find_cpu_driver(struct device *cpu);

struct thread;

struct cpu_info {
	struct device *cpu;
	unsigned int index;
#if IS_ENABLED(CONFIG_COOP_MULTITASKING)
	struct thread *thread;
#endif
};

static inline struct cpu_info *cpu_info(void)
{
	struct cpu_info *ci;
	__asm__(
#ifdef __x86_64__
		"and %%rsp,%0; "
		"or  %2, %0 "
#else
		"andl %%esp,%0; "
		"orl  %2, %0 "
#endif
		: "=r" (ci)
		: "0" (~(CONFIG_STACK_SIZE - 1)),
		"r" (CONFIG_STACK_SIZE - sizeof(struct cpu_info))
	);
	return ci;
}

static inline unsigned long cpu_index(void)
{
	struct cpu_info *ci;
	ci = cpu_info();
	return ci->index;
}
#endif

#ifndef __ROMCC__ // romcc is segfaulting in some cases
struct cpuinfo_x86 {
	uint8_t	x86;		/* CPU family */
	uint8_t	x86_vendor;	/* CPU vendor */
	uint8_t	x86_model;
	uint8_t	x86_mask;
};

static inline void get_fms(struct cpuinfo_x86 *c, uint32_t tfms)
{
	c->x86 = (tfms >> 8) & 0xf;
	c->x86_model = (tfms >> 4) & 0xf;
	c->x86_mask = tfms & 0xf;
	if (c->x86 == 0xf)
		c->x86 += (tfms >> 20) & 0xff;
	if (c->x86 >= 0x6)
		c->x86_model += ((tfms >> 16) & 0xF) << 4;

}
#endif

#define asmlinkage __attribute__((regparm(0)))
#define alwaysinline inline __attribute__((always_inline))

#ifndef __ROMCC__
/*
 * When using CONFIG_C_ENVIRONMENT_BOOTBLOCK the car_stage_entry()
 * is the symbol jumped to for each stage after bootblock using
 * cache-as-ram.
 */
asmlinkage void car_stage_entry(void);

/*
 * Support setting up a stack frame consisting of MTRR information
 * for use in bootstrapping the caching attributes after cache-as-ram
 * is torn down.
 */

struct postcar_frame {
	uintptr_t stack;
	uint32_t upper_mask;
	int max_var_mtrrs;
	int num_var_mtrrs;
};

/*
 * Initialize postcar_frame object allocating stack size in cbmem
 * with the provided size. Returns 0 on success, < 0 on error.
 */
int postcar_frame_init(struct postcar_frame *pcf, size_t stack_size);

/*
 * Initialize postcar_frame object with a fixed stacktop in low memory.
 */
void postcar_frame_init_lowmem(struct postcar_frame *pcf);

/*
 * Add variable MTRR covering the provided range with MTRR type.
 */
void postcar_frame_add_mtrr(struct postcar_frame *pcf,
				uintptr_t addr, size_t size, int type);

/*
 * Add variable MTRR covering the memory-mapped ROM with given MTRR type.
 */
void postcar_frame_add_romcache(struct postcar_frame *pcf, int type);

/*
 * Push used MTRR and Max MTRRs on to the stack
 * and return pointer to stack top.
 */
void *postcar_commit_mtrrs(struct postcar_frame *pcf);

/*
 * Load and run a program that takes control of execution that
 * tears down CAR and loads ramstage. The postcar_frame object
 * indicates how to set up the frame. If caching is enabled at
 * the time of the call it is up to the platform code to handle
 * coherency with dirty lines in the cache using some mechansim
 * such as platform_prog_run() because run_postcar_phase()
 * utilizes prog_run() internally.
 */
void run_postcar_phase(struct postcar_frame *pcf);

/*
 * Systems without a native coreboot cache-as-ram teardown may implement
 * this to use an alternate method.
 */
void late_car_teardown(void);

#endif

#endif /* ARCH_CPU_H */
