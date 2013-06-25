#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include <stdint.h>
#include <arch/rules.h>

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
static inline struct cpuid_result cpuid_ext(int op, unsigned ecx)
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

int cpu_phys_address_size(void);
int cpu_have_cpuid(void);

#ifndef __SIMPLE_DEVICE__

struct device;

struct cpu_device_id {
	unsigned vendor;
	unsigned device;
};

struct cpu_driver {
	struct device_operations *ops;
	struct cpu_device_id *id_table;
	struct acpi_cstate *cstates;
};

struct cpu_driver *find_cpu_driver(struct device *cpu);

struct thread;

struct cpu_info {
	struct device *cpu;
	unsigned int index;
#if CONFIG_COOP_MULTITASKING
	struct thread *thread;
#endif
};

static inline struct cpu_info *cpu_info(void)
{
	struct cpu_info *ci;
	__asm__("andl %%esp,%0; "
		"orl  %2, %0 "
		:"=r" (ci)
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
        uint8_t    x86;            /* CPU family */
        uint8_t    x86_vendor;     /* CPU vendor */
        uint8_t    x86_model;
        uint8_t    x86_mask;
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

#endif /* ARCH_CPU_H */
