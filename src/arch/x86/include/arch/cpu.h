/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include <types.h>

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

static inline unsigned int cpuid_get_max_func(void)
{
	return cpuid_eax(0);
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
#define X86_VENDOR_HYGON     11
#define X86_VENDOR_ANY     0xfe
#define X86_VENDOR_UNKNOWN 0xff

#define CPUID_FEATURE_PAE (1 << 6)
#define CPUID_FEATURE_PSE36 (1 << 17)
#define CPUID_FEAURE_HTT (1 << 28)

// Intel leaf 0x4, AMD leaf 0x8000001d EAX

#define CPUID_CACHE(x, res) \
	(((res) >> CPUID_CACHE_##x##_SHIFT) & CPUID_CACHE_##x##_MASK)

#define CPUID_CACHE_FULL_ASSOC_SHIFT 9
#define CPUID_CACHE_FULL_ASSOC_MASK 0x1
#define CPUID_CACHE_FULL_ASSOC(res) CPUID_CACHE(FULL_ASSOC, (res).eax)

#define CPUID_CACHE_SELF_INIT_SHIFT 8
#define CPUID_CACHE_SELF_INIT_MASK 0x1
#define CPUID_CACHE_SELF_INIT(res) CPUID_CACHE(SELF_INIT, (res).eax)

#define CPUID_CACHE_LEVEL_SHIFT 5
#define CPUID_CACHE_LEVEL_MASK 0x7
#define CPUID_CACHE_LEVEL(res) CPUID_CACHE(LEVEL, (res).eax)

#define CPUID_CACHE_TYPE_SHIFT 0
#define CPUID_CACHE_TYPE_MASK 0x1f
#define CPUID_CACHE_TYPE(res) CPUID_CACHE(TYPE, (res).eax)

// Intel leaf 0x4, AMD leaf 0x8000001d EBX

#define CPUID_CACHE_WAYS_OF_ASSOC_SHIFT 22
#define CPUID_CACHE_WAYS_OF_ASSOC_MASK 0x3ff
#define CPUID_CACHE_WAYS_OF_ASSOC(res) CPUID_CACHE(WAYS_OF_ASSOC, (res).ebx)

#define CPUID_CACHE_PHYS_LINE_SHIFT 12
#define CPUID_CACHE_PHYS_LINE_MASK 0x3ff
#define CPUID_CACHE_PHYS_LINE(res) CPUID_CACHE(PHYS_LINE, (res).ebx)

#define CPUID_CACHE_COHER_LINE_SHIFT 0
#define CPUID_CACHE_COHER_LINE_MASK 0xfff
#define CPUID_CACHE_COHER_LINE(res) CPUID_CACHE(COHER_LINE, (res).ebx)

// Intel leaf 0x4, AMD leaf 0x8000001d ECX

#define CPUID_CACHE_NO_OF_SETS_SHIFT 0
#define CPUID_CACHE_NO_OF_SETS_MASK 0xffffffff
#define CPUID_CACHE_NO_OF_SETS(res) CPUID_CACHE(NO_OF_SETS, (res).ecx)

unsigned int cpu_cpuid_extended_level(void);
int cpu_have_cpuid(void);

static inline bool cpu_is_amd(void)
{
	return CONFIG(CPU_AMD_AGESA) || CONFIG(CPU_AMD_PI) || CONFIG(SOC_AMD_COMMON);
}

static inline bool cpu_is_intel(void)
{
	return CONFIG(CPU_INTEL_COMMON) || CONFIG(SOC_INTEL_COMMON);
}

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
#if CONFIG(COOP_MULTITASKING)
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

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static __always_inline void cpu_relax(void)
{
	__asm__ __volatile__("rep;nop" : : : "memory");
}

#define asmlinkage __attribute__((regparm(0)))

/*
 * The car_stage_entry() is the symbol jumped to for each stage
 * after bootblock using cache-as-ram.
 */
asmlinkage void car_stage_entry(void);

/*
 * Get processor id using cpuid eax=1
 * return value in EAX register
 */
uint32_t cpu_get_cpuid(void);

/*
 * Get processor feature flag using cpuid eax=1
 * return value in ECX register
 */
uint32_t cpu_get_feature_flags_ecx(void);

/*
 * Get processor feature flag using cpuid eax=1
 * return value in EDX register
 */
uint32_t cpu_get_feature_flags_edx(void);

/*
 * Previously cpu_index() implementation assumes that cpu_index()
 * function will always getting called from coreboot context
 * (ESP stack pointer will always refer to coreboot).
 *
 * But with MP_SERVICES_PPI implementation in coreboot this
 * assumption might not be true, where FSP context (stack pointer refers
 * to FSP) will request to get cpu_index().
 *
 * Hence new logic to use cpuid to fetch lapic id and matches with
 * cpus_default_apic_id[] variable to return correct cpu_index().
 */
int cpu_index(void);

#endif /* ARCH_CPU_H */
